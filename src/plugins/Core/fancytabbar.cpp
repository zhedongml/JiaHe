#include "fancytabbar.h"

#include <QPainter>
#include <QDebug>
#include <QLinearGradient>
#include <QMouseEvent>
#include "theme/theme_p.h"
#include "theme/stylehelper.h"
#include <QPixmapCache>
#include <QToolTip>
#include <QStyleOption>
#include "coreconstants.h"
using namespace Core;
static const int kMenuButtonWidth = 16;
FancyTabBar::FancyTabBar(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true); // Needed for hover events
}

QSize FancyTabBar::tabSizeHint(bool minimum) const
{
    if (m_iconsOnly) {
        return { Core::Constants::MODEBAR_ICONSONLY_BUTTON_SIZE,
                    Core::Constants::MODEBAR_ICONSONLY_BUTTON_SIZE / (minimum ? 3 : 1) };
    }

    QFont boldFont(font());
    boldFont.setPointSizeF(StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    const QFontMetrics fm(boldFont);
    const int spacing = 8;
    const int width = 60 + spacing + 2;
    int maxLabelwidth = 0;
    for (auto tab : qAsConst(m_tabs)) {
        const int width = fm.horizontalAdvance(tab->text);
        if (width > maxLabelwidth)
            maxLabelwidth = width;
    }
    const int iconHeight = minimum ? 0 : 32;
    return { qMax(width, maxLabelwidth + 4), iconHeight + spacing + fm.height() };
}

void FancyTabBar::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    if (creatorTheme()->flag(Theme::FlatToolBars)) {
        // draw background of upper part of left tab widget
        // (Welcome, ... Help)
        p.fillRect(event->rect(), StyleHelper::baseColor());
    }

    for (int i = 0; i < count(); ++i)
        if (i != currentIndex())
            paintTab(&p, i);

    // paint active tab last, since it overlaps the neighbors
    if (currentIndex() != -1)
        paintTab(&p, currentIndex());
}

// Handle hover events for mouse fade ins
void FancyTabBar::mouseMoveEvent(QMouseEvent* event)
{
    int newHover = -1;
    for (int i = 0; i < count(); ++i) {
        const QRect area = tabRect(i);
        if (area.contains(event->pos())) {
            newHover = i;
            break;
        }
    }
    if (newHover == m_hoverIndex)
        return;

    if (validIndex(m_hoverIndex))
        m_tabs[m_hoverIndex]->fadeOut();

    m_hoverIndex = newHover;

    if (validIndex(m_hoverIndex)) {
        m_tabs[m_hoverIndex]->fadeIn();
        m_hoverRect = tabRect(m_hoverIndex);
    }
}

bool FancyTabBar::event(QEvent* event)
{
    if (event->type() == QEvent::ToolTip) {
        if (validIndex(m_hoverIndex)) {
            const QString tt = tabToolTip(m_hoverIndex);
            if (!tt.isEmpty()) {
                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tt, this);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

// Resets hover animation on mouse enter
void FancyTabBar::enterEvent(QEvent* event)
{
    Q_UNUSED(event)
        m_hoverRect = QRect();
    m_hoverIndex = -1;
}

// Resets hover animation on mouse enter
void FancyTabBar::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
        m_hoverIndex = -1;
    m_hoverRect = QRect();
    for (auto tab : qAsConst(m_tabs))
        tab->fadeOut();
}

QSize FancyTabBar::sizeHint() const
{
    const QSize sh = tabSizeHint();
    return { sh.width(), sh.height() * int(m_tabs.count()) };
}

QSize FancyTabBar::minimumSizeHint() const
{
    const QSize sh = tabSizeHint(true);
    return { sh.width(), sh.height() * int(m_tabs.count()) };
}

QRect FancyTabBar::tabRect(int index) const
{
    QSize sh = tabSizeHint();

    if (sh.height() * m_tabs.count() > height())
        sh.setHeight(height() / m_tabs.count());

    return { 0, index * sh.height(), sh.width(), sh.height() };
}

void FancyTabBar::mousePressEvent(QMouseEvent* event)
{
    event->accept();
    for (int index = 0; index < m_tabs.count(); ++index) {
        const QRect rect = tabRect(index);
        if (rect.contains(event->pos())) {
            if (isTabEnabled(index)) {
                if (m_tabs.at(index)->hasMenu
                    && ((!m_iconsOnly && rect.right() - event->pos().x() <= kMenuButtonWidth)
                        || event->button() == Qt::RightButton)) {
                    // menu arrow clicked or right-click
                    emit menuTriggered(index, event);
                }
                else {
                    if (index != m_currentIndex) {
                        emit currentAboutToChange(index);
                        m_currentIndex = index;
                        update();
                        // update tab bar before showing widget
                        QMetaObject::invokeMethod(this, [this]() {
                            emit currentChanged(m_currentIndex);
                            }, Qt::QueuedConnection);
                    }
                }
            }
            break;
        }
    }
}

static void paintSelectedTabBackground(QPainter* painter, const QRect& spanRect)
{
    const int verticalOverlap = 2; // Grows up and down for the overlaps
    const int dpr = painter->device()->devicePixelRatio();
    const QString cacheKey = QLatin1String(Q_FUNC_INFO) + QString::number(spanRect.width())
        + QLatin1Char('x') + QString::number(spanRect.height())
        + QLatin1Char('@') + QString::number(dpr);
    QPixmap selection;
    if (!QPixmapCache::find(cacheKey, &selection)) {
        selection = QPixmap(QSize(spanRect.width(), spanRect.height() + 2 * verticalOverlap) * dpr);
        selection.fill(Qt::transparent);
        selection.setDevicePixelRatio(dpr);
        QPainter p(&selection);
        p.translate(QPoint(0, verticalOverlap));

        const QRect rect(QPoint(), spanRect.size());
        const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

        //background
        p.save();
        QLinearGradient grad(rect.topLeft(), rect.topRight());
        grad.setColorAt(0, QColor(255, 255, 255, 140));
        grad.setColorAt(1, QColor(255, 255, 255, 210));
        p.fillRect(rect, grad);
        p.restore();

        //shadows
        p.setPen(QColor(0, 0, 0, 110));
        p.drawLine(borderRect.topLeft() + QPointF(1, -1), borderRect.topRight() - QPointF(0, 1));
        p.drawLine(borderRect.bottomLeft(), borderRect.bottomRight());
        p.setPen(QColor(0, 0, 0, 40));
        p.drawLine(borderRect.topLeft(), borderRect.bottomLeft());

        //highlights
        p.setPen(QColor(255, 255, 255, 50));
        p.drawLine(borderRect.topLeft() + QPointF(0, -2), borderRect.topRight() - QPointF(0, 2));
        p.drawLine(borderRect.bottomLeft() + QPointF(0, 1),
            borderRect.bottomRight() + QPointF(0, 1));
        p.setPen(QColor(255, 255, 255, 40));
        p.drawLine(borderRect.topLeft() + QPointF(0, 0), borderRect.topRight());
        p.drawLine(borderRect.topRight() + QPointF(0, 1), borderRect.bottomRight() - QPointF(0, 1));
        p.drawLine(borderRect.bottomLeft() + QPointF(0, -1),
            borderRect.bottomRight() - QPointF(0, 1));
        p.end();

        QPixmapCache::insert(cacheKey, selection);
    }
    painter->drawPixmap(spanRect.topLeft() + QPoint(0, -verticalOverlap), selection);
}

static void paintIcon(QPainter* painter, const QRect& rect,
    const QIcon& icon,
    bool enabled, bool selected)
{
    const QIcon::Mode iconMode = enabled ? (selected ? QIcon::Active : QIcon::Normal)
        : QIcon::Disabled;
    QRect iconRect(0, 0, Core::Constants::MODEBAR_ICON_SIZE, Core::Constants::MODEBAR_ICON_SIZE);
    iconRect.moveCenter(rect.center());
    iconRect = iconRect.intersected(rect);
    if (!enabled && !creatorTheme()->flag(Theme::FlatToolBars))
        painter->setOpacity(0.7);
    StyleHelper::drawIconWithShadow(icon, iconRect, painter, iconMode);

    if (selected && creatorTheme()->flag(Theme::FlatToolBars)) {
        painter->setOpacity(1.0);
        QRect accentRect = rect;
        accentRect.setWidth(2);
        painter->fillRect(accentRect, creatorTheme()->color(Theme::IconsBaseColor));
    }
}

static void paintIconAndText(QPainter* painter, const QRect& rect,
    const QIcon& icon, const QString& text,
    bool enabled, bool selected)
{
    QFont boldFont(painter->font());
    boldFont.setPointSizeF(StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    painter->setFont(boldFont);

    const bool drawIcon = rect.height() > 36;
    if (drawIcon) {
        const int textHeight =
            painter->fontMetrics().boundingRect(rect, Qt::TextWordWrap, text).height();
        const QRect tabIconRect(rect.adjusted(0, 4, 0, -textHeight));
        const QIcon::Mode iconMode = enabled ? (selected ? QIcon::Active : QIcon::Normal)
            : QIcon::Disabled;
        QRect iconRect(0, 0, Core::Constants::MODEBAR_ICON_SIZE, Core::Constants::MODEBAR_ICON_SIZE);
        iconRect.moveCenter(tabIconRect.center());
        iconRect = iconRect.intersected(tabIconRect);
        if (!enabled && !creatorTheme()->flag(Theme::FlatToolBars))
            painter->setOpacity(0.7);
        StyleHelper::drawIconWithShadow(icon, iconRect, painter, iconMode);
    }

    painter->setOpacity(1.0); //FIXME: was 0.7 before?
    if (selected && creatorTheme()->flag(Theme::FlatToolBars)) {
        QRect accentRect = rect;
        accentRect.setWidth(2);
        painter->fillRect(accentRect, creatorTheme()->color(Theme::IconsBaseColor));
    }
    if (enabled) {
        painter->setPen(
            selected ? creatorTheme()->color(Theme::FancyTabWidgetEnabledSelectedTextColor)
            : creatorTheme()->color(Theme::FancyTabWidgetEnabledUnselectedTextColor));
    }
    else {
        painter->setPen(
            selected ? creatorTheme()->color(Theme::FancyTabWidgetDisabledSelectedTextColor)
            : creatorTheme()->color(Theme::FancyTabWidgetDisabledUnselectedTextColor));
    }

    painter->translate(0, -1);
    QRect tabTextRect(rect);
    tabTextRect.translate(0, drawIcon ? -2 : 1);
    const int textFlags = Qt::AlignCenter | (drawIcon ? Qt::AlignBottom : Qt::AlignVCenter)
        | Qt::TextWordWrap;
    painter->drawText(tabTextRect, textFlags, text);
}

void FancyTabBar::paintTab(QPainter* painter, int tabIndex) const
{
    if (!validIndex(tabIndex)) {
        qWarning("invalid index");
        return;
    }
    painter->save();

    const FancyTab* tab = m_tabs.at(tabIndex);
    const QRect rect = tabRect(tabIndex);
    const bool selected = (tabIndex == m_currentIndex);
    const bool enabled = isTabEnabled(tabIndex);

    if (selected) {
        if (creatorTheme()->flag(Theme::FlatToolBars)) {
            // background color of a fancy tab that is active
            painter->fillRect(rect, creatorTheme()->color(Theme::FancyTabBarSelectedBackgroundColor));
        }
        else {
            paintSelectedTabBackground(painter, rect);
        }
    }

    const qreal fader = tab->fader();
    if (fader > 0 && !selected && enabled) {
        painter->save();
        painter->setOpacity(fader);
        if (creatorTheme()->flag(Theme::FlatToolBars))
            painter->fillRect(rect, creatorTheme()->color(Theme::FancyToolButtonHoverColor));
        else
        {
            const QSize logicalSize = rect.size();
            const QString cacheKey = QLatin1String(Q_FUNC_INFO) + QString::number(logicalSize.width())
                + QLatin1Char('x') + QString::number(logicalSize.height());
            QPixmap overlay;
            if (!QPixmapCache::find(cacheKey, &overlay)) {
                const int dpr = painter->device()->devicePixelRatio();
                overlay = QPixmap(logicalSize * dpr);
                overlay.fill(Qt::transparent);
                overlay.setDevicePixelRatio(dpr);

                const QColor hoverColor = creatorTheme()->color(Theme::FancyToolButtonHoverColor);
                const QRect rect(QPoint(), logicalSize);
                const QRectF borderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

                QLinearGradient grad(rect.topLeft(), rect.topRight());
                grad.setColorAt(0, Qt::transparent);
                grad.setColorAt(0.5, hoverColor);
                grad.setColorAt(1, Qt::transparent);

                QPainter p(&overlay);
                p.fillRect(rect, grad);
                p.setPen(QPen(grad, 1.0));
                p.drawLine(borderRect.topLeft(), borderRect.topRight());
                p.drawLine(borderRect.bottomLeft(), borderRect.bottomRight());
                p.end();

                QPixmapCache::insert(cacheKey, overlay);
            }
            painter->drawPixmap(rect.topLeft(), overlay);
        }
        painter->restore();
    }

    if (m_iconsOnly)
        paintIcon(painter, rect, tab->icon, enabled, selected);
    else
        paintIconAndText(painter, rect, tab->icon, tab->text, enabled, selected);

    // menu arrow
    if (tab->hasMenu && !m_iconsOnly) {
        QStyleOption opt;
        opt.initFrom(this);
        opt.rect = rect.adjusted(rect.width() - kMenuButtonWidth, 0, -8, 0);
        StyleHelper::drawArrow(QStyle::PE_IndicatorArrowRight, painter, &opt);
    }
    painter->restore();
}

void FancyTabBar::setCurrentIndex(int index)
{
    if (isTabEnabled(index) && index != m_currentIndex) {
        emit currentAboutToChange(index);
        m_currentIndex = index;
        update();
        emit currentChanged(m_currentIndex);
    }
}

void FancyTabBar::setIconsOnly(bool iconsOnly)
{
    m_iconsOnly = iconsOnly;
    updateGeometry();
}

void FancyTabBar::setTabEnabled(int index, bool enable)
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0) {
        m_tabs[index]->enabled = enable;
        update(tabRect(index));
    }
}

bool FancyTabBar::isTabEnabled(int index) const
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0)
        return m_tabs[index]->enabled;

    return false;
}
FancyTab*  FancyTabBar::getTab(int index)
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);
    return m_tabs[index];
}
void FancyTabBar::insertTab(int index, const QIcon& icon, const QString& label, bool hasMenu, STACKPOSTION type, int id)
{
    auto tab = new FancyTab(this);
    tab->icon = icon;
    tab->text = label;
    tab->hasMenu = hasMenu;
    switch (type) {
    case STACKPOSTION::FULLPAGE:
        tab->fullpageStackId = id;
        break;
    case STACKPOSTION::LOGICANDRESULTBOTH:
        tab->resultPartStackId = id;
        tab->logicPartStackId = id;
        break;
    case STACKPOSTION::RESULTPAGE:
        tab->resultPartStackId = id;
        break;
    case STACKPOSTION::LOGICPAGE:
        tab->logicPartStackId = id;
        break;
    default:
        break;

    }
    m_tabs.insert(index, tab);
    if (m_currentIndex >= index)
        ++m_currentIndex;
    updateGeometry();
}
void  FancyTabBar::removeTab(int index)
{
    FancyTab* tab = m_tabs.takeAt(index);
    int logicStackId = tab->logicPartStackId;
    int resultStackId = tab->resultPartStackId;
    int fullStackId = tab->fullpageStackId;
    for (int i = index + 1; i < m_tabs.size(); i++) {
        FancyTab* temp = m_tabs.at(i);
        if (fullStackId >= 0) {
            temp->fullpageStackId--;
        }
        else {
            if (logicStackId >= 0) {
                temp->logicPartStackId--;
            }
            if (resultStackId >= 0) {
                temp->resultPartStackId--;
            }
        }
    }
    delete tab;
    updateGeometry();
}