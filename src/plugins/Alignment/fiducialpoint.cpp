#include "fiducialpoint.h"
#include <QGraphicsScene>
#include <QCursor>
FiducialPoint::FiducialPoint(QAbstractGraphicsShapeItem* parent, QPointF p, int radius)
    : QAbstractGraphicsShapeItem(parent)
    , m_point(p), m_radius(radius)
{
    this->setPos(m_point);
    this->setFlags(QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemIsFocusable);
    this->setCursor(Qt::OpenHandCursor);
    m_pen_noSelected.setColor(QColor(255, 0, 0));
    m_pen_noSelected.setWidth(1);
    m_pen_isSelected.setColor(QColor(255, 0, 255));
    m_pen_isSelected.setWidth(2);
    this->setPen(m_pen_noSelected);
    menu = new QMenu;
    removeAction = new QAction("Delete Fiducial", menu);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(delSelf()));
}
FiducialPoint::~FiducialPoint()
{
    if (menu != nullptr)
        delete menu;
}
void FiducialPoint::delSelf()
{
    emit deleteFiducial();
    //delete this;
}
QRectF FiducialPoint::boundingRect() const
{
    //return QRectF(-2, -2, 4, 4);
    return QRectF(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
}

void FiducialPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QFont font;
    font.setPointSize(30);
    painter->setFont(font);

    painter->setPen(this->pen());
    painter->setBrush(this->brush());
    //this->setPos(m_point);
    painter->drawEllipse(-2, -2, 4, 4);
    painter->drawEllipse(-m_radius, -m_radius, m_radius * 2, m_radius * 2);
    painter->drawText(-m_radius - 5, -m_radius - 5, QString("(%1, %2)").arg(m_point.x()).arg(m_point.y()));
}

void FiducialPoint::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton) {
        qreal dx = event->scenePos().x() - event->lastScenePos().x();
        qreal dy = event->scenePos().y() - event->lastScenePos().y();
        moveBy(dx, dy); 

        m_point.setX(m_point.x() + dx);
        m_point.setY(m_point.y() + dy);
        emit postionChanged(m_point.x(), m_point.y(), id);

        this->scene()->update();
    }
}

void FiducialPoint::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
}

void FiducialPoint::focusInEvent(QFocusEvent* event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_isSelected);
}

void FiducialPoint::focusOutEvent(QFocusEvent* event)
{
    Q_UNUSED(event);
    this->setPen(m_pen_noSelected);
}
