#ifndef CROPPER_HPP
#define CROPPER_HPP

#include <QObject>
#include <qglobal.h>
#include <QGraphicsItem>
#include <QtGui>
#include "./resizer.hpp"
#include <cmath>
#include "./defines.hpp"
#include <QToolTip>
#include <QMenu>

class Cropper : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Cropper(QObject* parent = 0, QString imgpath = "", int startx = 50, int starty = 50, int width = 55, int hegiht = 55,int index = 0);
    ~Cropper();
    qreal CurrentScale = 1.0;
    bool anchored;
    int Xgen;
    int Ygen;
    qreal width;
    qreal height;
    qreal RotateAngle;
    qreal AnchoredAngle;
    int AreaIndex;

    Resizer* RSide;
    Resizer* LSide;
    Resizer* USide;
    Resizer* BSide;
    QString imgpath = "";
    int Index = -1;
    virtual void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget);

    virtual QRectF boundingRect() const;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    void updateItemsPos();
    QPixmap convertToGray(QPixmap& pxmp);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
public slots:
    void changeSize(qreal dx, qreal dy, Position position);
    void getSamples();
    void delself();


signals:
    void ROIRegionChange(int x, int y, int width, int height);
    void deleteROI();
    void mouseHoverEnter();
    void mousePressEventSignal(QGraphicsSceneMouseEvent *event);
};

#endif // CROPPER_HPP

