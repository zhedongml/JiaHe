#include "logmanager.h"
#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include "outputpane.h"
#include "statusbarmanager.h"
#include "styledbar.h"
#include "outputwindow.h"
#include "coreconstants.h"
#include "loggingwrapper.h"

namespace Core {
    static LogManager* m_instance = nullptr;

    void LogManager::create()
    {
        m_instance = new LogManager;
    }

    void LogManager::destroy()
    {
        delete m_instance;
        m_instance = nullptr;
    }

    LogManager* LogManager::instance()
    {
        return m_instance;
    }

	LogManager::LogManager(QWidget* parent)
		: QWidget(parent)
	{
        m_titleLabel = new QLabel("Log");
        m_closeButton = new QToolButton;
        m_minMaxButton = new QToolButton;
        m_minimizeIcon = QIcon(":/image/images/arrowdown.png");
        m_maximizeIcon = QIcon(":/image/images/arrowup.png");
        setWindowTitle(tr("Output"));

        m_titleLabel->setContentsMargins(5, 0, 5, 0);

        m_clearAction = new QAction(this);
        m_clearAction->setIcon(QIcon(":/image/images/clean_pane_small.png"));
        m_clearAction->setText(tr("Clear"));
        connect(m_clearAction, &QAction::triggered, this, &LogManager::clearPage);

        auto mainlayout = new QVBoxLayout;
        mainlayout->setSpacing(0);
        mainlayout->setContentsMargins(0, 0, 0, 0);
        m_toolBar = new StyledBar;
        auto toolLayout = new QHBoxLayout(m_toolBar);
        toolLayout->setContentsMargins(0, 0, 0, 0);
        toolLayout->setSpacing(0);
        toolLayout->addWidget(m_titleLabel);
        m_clearButton = new QToolButton;
        m_clearButton->setIcon(QIcon(":/image/images/clean_pane_small.png"));
        m_clearButton->setDefaultAction(m_clearAction);
        toolLayout->addWidget(m_clearButton);
        m_minMaxButton->setIcon(m_minimizeIcon);
        connect(m_minMaxButton, &QAbstractButton::clicked, this, &LogManager::slotHide);
        toolLayout->addWidget(m_minMaxButton);
        mainlayout->addWidget(m_toolBar);
        setLayout(mainlayout);
        QAction* paction = new QAction(this);
        m_toggleButton = new OutputPaneToggleButton(1, "Log Viewer", paction);

        connect(m_toggleButton, &QAbstractButton::clicked, this, [] {
            m_instance->buttonTriggered(1);
            });

        StatusBarManager::addStatusBarWidget(m_toggleButton, StatusBarManager::Second);

        Core::Context context(Constants::MAIN_WINDOW);
        m_outputWindow = new Core::OutputWindow(context, "");
        m_outputWindow->setWindowTitle("Log Viewer");
        m_outputWindow->setReadOnly(true);
        m_outputWindow->setUndoRedoEnabled(false);
        m_outputWindow->setMaxCharCount(Core::Constants::DEFAULT_MAX_CHAR_COUNT);

        QPalette p = m_outputWindow->palette();
        QColor activeHighlight = p.color(QPalette::Active, QPalette::Highlight);
        p.setColor(QPalette::Highlight, activeHighlight);
        QColor activeHighlightedText = p.color(QPalette::Active, QPalette::HighlightedText);
        p.setColor(QPalette::HighlightedText, activeHighlightedText);
        m_outputWindow->setPalette(p);
        m_outputWindow->appendMessage("Init log.\r\n", OutputFormat::ErrorMessageFormat);
        mainlayout->addWidget(m_outputWindow);

        LoggingWrapper::instance()->setAppOutPutPane(m_outputWindow);
        //this->show();
	}

    void LogManager::initialize()
    {

        OutputPanePlaceHolder* ph = OutputPanePlaceHolder::getCurrent();
        
        QFontMetrics titleFm = m_instance->m_titleLabel->fontMetrics();
        int minTitleWidth = 0;
        /*const int n = g_outputPanes.size();

        int shortcutNumber = 1;
        const Id baseId = "QtCreator.Pane.";
        for (int i = 0; i != n; ++i) {
            OutputPaneData& data = g_outputPanes[i];
            IOutputPane* outPane = data.pane;
            const int idx = m_instance->m_outputWidgetPane->addWidget(outPane->outputWidget(m_instance));
            QTC_CHECK(idx == i);

            connect(outPane, &IOutputPane::showPage, m_instance, [idx](int flags) {
                m_instance->showPage(idx, flags);
                });
            connect(outPane, &IOutputPane::hidePage, m_instance, &OutputPaneManager::slotHide);

            connect(outPane, &IOutputPane::togglePage, m_instance, [idx](int flags) {
                if (OutputPanePlaceHolder::isCurrentVisible() && m_instance->currentIndex() == idx)
                    m_instance->slotHide();
                else
                    m_instance->showPage(idx, flags);
                });

            connect(outPane, &IOutputPane::navigateStateUpdate, m_instance, [idx, outPane] {
                if (m_instance->currentIndex() == idx) {
                    m_instance->m_prevAction->setEnabled(outPane->canNavigate()
                        && outPane->canPrevious());
                    m_instance->m_nextAction->setEnabled(outPane->canNavigate() && outPane->canNext());
                }
                });

            QWidget* toolButtonsContainer = new QWidget(m_instance->m_opToolBarWidgets);
            auto toolButtonsLayout = new QHBoxLayout;
            toolButtonsLayout->setContentsMargins(0, 0, 0, 0);
            toolButtonsLayout->setSpacing(0);
            foreach(QWidget * toolButton, outPane->toolBarWidgets())
                toolButtonsLayout->addWidget(toolButton);
            toolButtonsLayout->addStretch(5);
            toolButtonsContainer->setLayout(toolButtonsLayout);

            m_instance->m_opToolBarWidgets->addWidget(toolButtonsContainer);

            minTitleWidth = qMax(minTitleWidth, titleFm.horizontalAdvance(outPane->displayName()));

            QString suffix = outPane->displayName().simplified();
            suffix.remove(QLatin1Char(' '));
            data.id = baseId.withSuffix(suffix);
            data.action = new QAction(outPane->displayName(), m_instance);
            Command* cmd = ActionManager::registerAction(data.action, data.id);

            mpanes->addAction(cmd, "Coreplugin.OutputPane.PanesGroup");

            cmd->setDefaultKeySequence(paneShortCut(shortcutNumber));
            auto button = new OutputPaneToggleButton(shortcutNumber,
                outPane->displayName(),
                cmd->action());
            data.button = button;

            connect(outPane, &IOutputPane::flashButton, button, [button] { button->flash(); });
            connect(outPane,
                &IOutputPane::setBadgeNumber,
                button,
                &OutputPaneToggleButton::setIconBadgeNumber);

            ++shortcutNumber;
            m_instance->m_buttonsWidget->layout()->addWidget(data.button);
            connect(data.button, &QAbstractButton::clicked, m_instance, [i] {
                m_instance->buttonTriggered(i);
                });

            bool visible = outPane->priorityInStatusBar() != -1;
            data.button->setVisible(visible);
            data.buttonVisible = visible;

            connect(data.action, &QAction::triggered, m_instance, [i] {
                m_instance->shortcutTriggered(i);
                });
        }

        m_instance->m_titleLabel->setMinimumWidth(
            minTitleWidth + m_instance->m_titleLabel->contentsMargins().left()
            + m_instance->m_titleLabel->contentsMargins().right());
        m_instance->m_buttonsWidget->layout()->addWidget(m_instance->m_manageButton);
        connect(m_instance->m_manageButton,
            &QAbstractButton::clicked,
            m_instance,
            &OutputPaneManager::popupMenu);

        m_instance->readSettings();*/
    }

    void LogManager::toggleMaximized()
    {
        OutputPanePlaceHolder* ph = OutputPanePlaceHolder::getCurrent();

        if (!ph->isVisible()) // easier than disabling/enabling the action
            return;
        ph->setMaximized(!ph->isMaximized());
    }
    LogManager::~LogManager() = default;
    void LogManager::buttonTriggered(int idx)
    {
        if (OutputPanePlaceHolder::isCurrentVisible()) {
            // we should toggle and the page is already visible and we are actually closeable
            slotHide();
        }
        else {
            showPage(idx);
        }
    }
    void Core::LogManager::clearPage()
    {
        m_outputWindow->clear();
    }
    void LogManager::slotHide()
    {
        OutputPanePlaceHolder* ph = OutputPanePlaceHolder::getCurrent();
        if (ph) {
            emit ph->visibilityChangeRequested(false);
            ph->setVisible(false);
            ph->showWidget(this, false);
        }
    }
    void LogManager::showPage(int idx)
    {
        OutputPanePlaceHolder* ph = OutputPanePlaceHolder::getCurrent();
        ph->setVisible(true);
        ph->showWidget(this, true);
        /*ensurePageVisible(idx);
        IOutputPane* out = g_outputPanes.at(idx).pane;
        if ( IOutputPane::WithFocus) {
            if (out->canFocus())
                out->setFocus();
            ICore::raiseWindow(m_outputWidgetPane);
        }

        if (flags & IOutputPane::EnsureSizeHint)
            ph->ensureSizeHintAsMinimum();*/
    }
    void LogManager::focusInEvent(QFocusEvent* e)
    {

    }
    BadgeLabel::BadgeLabel()
    {
        m_font = QApplication::font();
        m_font.setBold(true);
        m_font.setPixelSize(11);
    }

    void BadgeLabel::paint(QPainter* p, int x, int y, bool isChecked)
    {
        const QRectF rect(QRect(QPoint(x, y), m_size));
        p->save();
        p->setPen(Qt::NoPen);
        p->setRenderHint(QPainter::Antialiasing, true);
        p->drawRoundedRect(rect, m_padding, m_padding, Qt::AbsoluteSize);

        p->setFont(m_font);
        p->drawText(rect, Qt::AlignCenter, m_text);

        p->restore();
    }

    void BadgeLabel::setText(const QString& text)
    {
        m_text = text;
        calculateSize();
    }

    QString BadgeLabel::text() const
    {
        return m_text;
    }

    QSize BadgeLabel::sizeHint() const
    {
        return m_size;
    }

    void BadgeLabel::calculateSize()
    {
        const QFontMetrics fm(m_font);
        m_size = fm.size(Qt::TextSingleLine, m_text);
        m_size.setWidth(m_size.width() + m_padding * 1.5);
        m_size.setHeight(2 * m_padding + 1); // Needs to be uneven for pixel perfect vertical centering in the button
    }
    ///////////////////////////////////////////////////////////////////////
    //
    // OutputPaneToolButton
    //
    ///////////////////////////////////////////////////////////////////////

    OutputPaneToggleButton::OutputPaneToggleButton(int number, const QString& text,
        QAction* action, QWidget* parent)
        : QToolButton(parent)
        , m_number(QString::number(number))
        , m_text(text)
        , m_action(action)
        , m_flashTimer(new QTimeLine(1000, this))
    {
        setFocusPolicy(Qt::NoFocus);
        setCheckable(true);
        QFont fnt = QApplication::font();
        setFont(fnt);
        if (m_action)
            connect(m_action, &QAction::changed, this, &OutputPaneToggleButton::updateToolTip);

        m_flashTimer->setDirection(QTimeLine::Forward);
        m_flashTimer->setEasingCurve(QEasingCurve::SineCurve);
        m_flashTimer->setFrameRange(0, 92);
        auto updateSlot = QOverload<>::of(&QWidget::update);
        connect(m_flashTimer, &QTimeLine::valueChanged, this, updateSlot);
        connect(m_flashTimer, &QTimeLine::finished, this, updateSlot);
        updateToolTip();
    }

    void OutputPaneToggleButton::updateToolTip()
    {
        setToolTip(m_action->toolTip());
    }

    QSize OutputPaneToggleButton::sizeHint() const
    {
        ensurePolished();

        QSize s = fontMetrics().size(Qt::TextSingleLine, m_text);

        // Expand to account for border image
        s.rwidth() += 15 + 1 + 3 + 3;

        if (!m_badgeNumberLabel.text().isNull())
            s.rwidth() += m_badgeNumberLabel.sizeHint().width() + 1;

        return s;
    }

    void OutputPaneToggleButton::paintEvent(QPaintEvent*)
    {
        const QFontMetrics fm = fontMetrics();
        const int baseLine = (height() - fm.height() + 1) / 2 + fm.ascent();
        const int numberWidth = fm.horizontalAdvance(m_number);

        QPainter p(this);

        QStyleOption styleOption;
        styleOption.initFrom(this);
        const bool hovered = (styleOption.state & QStyle::State_MouseOver);

        QColor c("black");
  
        if (hovered)
            c = QColor("#6B7378");
        else if (isDown() || isChecked())
            c = QColor("#393D40");

        if (c != QColor("black"))
            p.fillRect(rect(), c);
       
       
        if (m_flashTimer->state() == QTimeLine::Running)
        {
            QColor c = QColor("#6B7378");
            c.setAlpha(m_flashTimer->currentFrame());
            QRect r = rect();
            p.fillRect(r, c);
        }

        p.setFont(font());
        p.setPen(QColor("#818C91"));
        //TODO: May not used
        //p.drawText((15 - numberWidth) / 2, baseLine, m_number);
        if (!isChecked())
            p.setPen(QColor("#3B4042"));

        //TODO: May
        p.setPen(QColor("#ffffff"));

        int leftPart = 15 + 3;
        int labelWidth = 0;
        if (!m_badgeNumberLabel.text().isEmpty()) {
            const QSize labelSize = m_badgeNumberLabel.sizeHint();
            labelWidth = labelSize.width() + 3;
            m_badgeNumberLabel.paint(&p, width() - labelWidth, (height() - labelSize.height()) / 2, isChecked());
        }
        p.drawText(leftPart, baseLine, fm.elidedText(m_text, Qt::ElideRight, width() - leftPart - 1 - labelWidth));
    }

    void OutputPaneToggleButton::checkStateSet()
    {
        //Stop flashing when button is checked
        QToolButton::checkStateSet();
        m_flashTimer->stop();
    }

    void OutputPaneToggleButton::flash(int count)
    {
        setVisible(true);
        //Start flashing if button is not checked
        if (!isChecked()) {
            m_flashTimer->setLoopCount(count);
            if (m_flashTimer->state() != QTimeLine::Running)
                m_flashTimer->start();
            update();
        }
    }

    void OutputPaneToggleButton::setIconBadgeNumber(int number)
    {
        QString text = (number ? QString::number(number) : QString());
        m_badgeNumberLabel.setText(text);
        updateGeometry();
    }


}