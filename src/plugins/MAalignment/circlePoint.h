#pragma once

#include <QAbstractGraphicsShapeItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QMenu>
class CirclePoint :public QObject, public QAbstractGraphicsShapeItem
{
	Q_OBJECT

public:
	CirclePoint(QAbstractGraphicsShapeItem* parent, QPointF p, int radius = 50);
	~CirclePoint();
    QPointF getPoint() { return m_point; }
    void setPoint(QPointF p) { m_point = p; }
    void setId(int idx) { id = idx; };
    int getId() { return id; };
    void SetTitle(QString str);

  protected:
    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    virtual void focusInEvent(QFocusEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
public slots:
    void delSelf();
private:
    QPointF m_point;
    QPen m_pen_isSelected;
    QPen m_pen_noSelected;
    int id =0;
    QMenu* menu = nullptr;
    QAction* removeAction = nullptr;
    int m_radius = 200;
    QString title = "";
  signals:
    void postionChanged(qreal, qreal,int);
    void deleteFiducial();
};
