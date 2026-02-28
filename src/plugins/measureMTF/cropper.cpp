#include "cropper.hpp"
#include <QDebug>

double dRand(double Min, double Max)
{
    return Min + ((double)rand() / RAND_MAX) * (Max - Min);
}

QPixmap equalizeHistogram(QPixmap& pxm, int widthStart,
    int widthEnd, int UBoundNorm)
{
    QImage img = pxm.toImage();
    int imgH = img.height();
    int numPix = (widthEnd - widthStart) * imgH;

    //Histogram calculation
    std::vector<int> pdf;//Probability density function
    for (int i = widthStart; i < widthEnd; i++)
        for (int j = 0; j < imgH; j++)
            pdf.push_back(qGray(img.pixel(i, j)));
    std::sort(pdf.begin(), pdf.end());
    int pdfLen = pdf.size();

    int accumul = 0;
    std::map<int, int> cdf;//Cumulative distribution function
    for (int i = 0; i < pdfLen - 1; i++)
    {
        ++accumul;
        if (pdf[i] != pdf[i + 1])
            cdf.insert(std::pair<int, int>(pdf[i], accumul));
    }
    cdf.insert(std::pair<int, int>(pdf[pdfLen - 1], ++accumul));

    //Equalize histogram
    int cdfMin = cdf.begin()->second;
    int diviser = numPix - cdfMin;
    int Pxl, newPxl;
    for (int i = widthStart; i < widthEnd; i++)
        for (int j = 0; j < imgH; j++)
        {
            Pxl = qGray(img.pixel(i, j));
            newPxl = static_cast<double>((cdf[Pxl] - cdfMin)) /
                static_cast<double>(diviser) * UBoundNorm;
            img.setPixel(i, j, qRgb(newPxl, newPxl, newPxl));
        }
    return QPixmap::fromImage(img);
}

void showRotationTooltip(QGraphicsSceneMouseEvent* event, qreal angle, qreal ancangle, bool anchored)
{
    QPointF pos = event->screenPos();
    QPoint p;
    p.setX(static_cast<int>(pos.x()));
    p.setY(static_cast<int>(pos.y()));
    qreal rAngle = anchored ? ceil(angle - ancangle) : ceil(angle);
    QString tooltip = "Rotation: " + QString::number(rAngle);
    QToolTip::showText(p, tooltip);
}

void showRotationTooltip(QGraphicsSceneWheelEvent* event, qreal angle, qreal ancangle, bool anchored)
{
    QPointF pos = event->screenPos();
    QPoint p;
    p.setX(static_cast<int>(pos.x()));
    p.setY(static_cast<int>(pos.y()));
    qreal rAngle = anchored ? ceil(angle - ancangle) : ceil(angle);
    QString tooltip = "Rotation: " + QString::number(rAngle);
    QToolTip::showText(p, tooltip);
}

Cropper::Cropper(QObject* parent, QString imgpath, int startx, int starty, int width, int hegiht,int index) :
    width(width), height(hegiht), RotateAngle(0), anchored(false), Xgen(startx), Ygen(starty), AreaIndex(index)
{
    this->imgpath = imgpath.left(-1);
    this->setParent(parent);
    setFlag(ItemSendsGeometryChanges);
    setAcceptDrops(true);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);


    LSide = new Resizer(this, LEFT);

    QObject::connect(LSide, SIGNAL(sizeChanged(qreal, qreal, Position)),
        this, SLOT(changeSize(qreal, qreal, Position)));

    RSide = new Resizer(this, RIGHT);
    QObject::connect(RSide, SIGNAL(sizeChanged(qreal, qreal, Position)),
        this, SLOT(changeSize(qreal, qreal, Position)));

    USide = new Resizer(this, UPPER);
    QObject::connect(USide, SIGNAL(sizeChanged(qreal, qreal, Position)),
        this, SLOT(changeSize(qreal, qreal, Position)));

    BSide = new Resizer(this, BOTTOM);
    QObject::connect(BSide, SIGNAL(sizeChanged(qreal, qreal, Position)),
        this, SLOT(changeSize(qreal, qreal, Position)));
    setPos(Xgen + width / 2, Ygen + height / 2);
    getSamples();
}
Cropper::~Cropper() {

}
void Cropper::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    QPen pen;
    pen.setWidth(0.1);

    pen.setColor(QColor(255, 0, 0, 255));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen);
    painter->drawRect(boundingRect());
}

QRectF Cropper::boundingRect() const
{
    return QRectF(-width / 2, -height / 2, width, height);
}

void Cropper::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    int a = 0;
    emit mousePressEventSignal(event);
}

void Cropper::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)//Left button performs drag and drop
    {
        qreal dx = event->scenePos().x() - event->lastScenePos().x();
        qreal dy = event->scenePos().y() - event->lastScenePos().y();
        moveBy(dx, dy);
    }
    else//Right button performs rotation
    {
        return;
        qreal Xcur = event->pos().x();
        qreal Ycur = event->pos().y();
        qreal Xprev = event->lastPos().x();
        qreal Yprev = event->lastPos().y();

        qreal d = Xprev * Ycur - Xcur * Yprev;
        qreal a = sqrt(Xprev * Xprev + Yprev * Yprev);
        qreal b = sqrt(Xcur * Xcur + Ycur * Ycur);

        RotateAngle += asin(d / (a * b)) * 180 / PI;
        if (RotateAngle > 360) RotateAngle = 360;
        else if (RotateAngle < -360) RotateAngle = -360;
        setRotation(RotateAngle);
        showRotationTooltip(event, RotateAngle, AnchoredAngle, anchored);
    }
    getSamples();
}

void Cropper::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (anchored)
    {
        anchored = false;
        LSide->anchored = false;
        LSide->update();

        RSide->anchored = false;
        RSide->update();

        BSide->anchored = false;
        BSide->update();

        USide->anchored = false;
        USide->update();
    }
    else
    {
        AnchoredAngle = RotateAngle;
        anchored = true;
        LSide->anchored = true;
        LSide->update();

        RSide->anchored = true;
        RSide->update();

        BSide->anchored = true;
        BSide->update();

        USide->anchored = true;
        USide->update();
    }
    showRotationTooltip(event, RotateAngle, AnchoredAngle, anchored);
}

void Cropper::changeSize(qreal dx, qreal dy, Position position)
{
    qreal RadRotateAngle = RotateAngle * PI / 180;
    prepareGeometryChange();
    switch (position)
    {
    case RIGHT:
        if (width > MINWIDTH && width + dx < MINWIDTH)
            dx = MINWIDTH - width - dx;
        else if (width <= MINWIDTH && dx < 0)
            dx = 0;

        width += dx;
        moveBy(0.5 * dx * cos(RadRotateAngle), 0.5 * dx * sin(RadRotateAngle));
        USide->width += dx;
        BSide->width += dx;
        break;

    case LEFT:
        if (width > MINWIDTH && width - dx < MINWIDTH)
            dx = width - dx - MINWIDTH;
        else if (width <= MINWIDTH && dx > 0)
            dx = 0;

        width -= dx;
        moveBy(0.5 * dx * cos(RadRotateAngle), 0.5 * dx * sin(RadRotateAngle));
        USide->width -= dx;
        BSide->width -= dx;
        break;

    case UPPER:
        if (height > MINHEIGHT && height - dy < MINHEIGHT)
            dy = height - dy - MINHEIGHT;
        else if (height <= MINHEIGHT && dy > 0)
            dy = 0;

        height -= dy;
        moveBy(-0.5 * dy * sin(RadRotateAngle), 0.5 * dy * cos(RadRotateAngle));
        LSide->height -= dy;
        RSide->height -= dy;
        break;

    case BOTTOM:
        if (height > MINHEIGHT && height + dy < MINHEIGHT)
            dy = MINHEIGHT - height - dy;
        else if (height <= MINHEIGHT && dy < 0)
            dy = 0;

        height += dy;
        moveBy(-0.5 * dy * sin(RadRotateAngle), 0.5 * dy * cos(RadRotateAngle));
        LSide->height += dy;
        RSide->height += dy;
        break;
    case STARTX:
        moveBy(dx * cos(RadRotateAngle), dx * sin(RadRotateAngle));
        break;
    case STARTY:
        moveBy(dy * sin(RadRotateAngle), dy * cos(RadRotateAngle));
        break;
    }
    updateItemsPos();
    getSamples();
}

void Cropper::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    return;
    event->setAccepted(true);
    qint64 numSteps = event->delta() / 120;
    RotateAngle += numSteps;
    if (RotateAngle > 360) RotateAngle = 360;
    else if (RotateAngle < -360) RotateAngle = -360;

    setRotation(RotateAngle);
    showRotationTooltip(event, RotateAngle, AnchoredAngle, anchored);
    getSamples();
}

void Cropper::updateItemsPos()
{
    USide->updatePosition();
    BSide->updatePosition();
    LSide->updatePosition();
    RSide->updatePosition();
    update();
}

void Cropper::getSamples()
{
    ////////////////Extract ROI///////////////////////////
    qreal w = width;
    qreal h = height;

    QTransform general = QTransform().rotate(-RotateAngle);

    QPointF p[4];
    // 0,0----->x
    //  |
    //  | y
    p[0] = QPoint(0, 0);
    p[1] = QPointF(w, 0);
    p[2] = QPointF(w, h);
    p[3] = QPointF(0, h);

    QPointF offsGen(0, 0);
    for (int i = 0; i < 4; ++i)
    {
        QPointF tempGen = general.map(p[i]);
        offsGen.setX(qMin(offsGen.x(), tempGen.x()));
        offsGen.setY(qMin(offsGen.y(), tempGen.y()));
    }
    // general.scale(1.0/CurrentScale, 1.0/CurrentScale);
     //QPointF LTgen = general.map(mapToScene(boundingRect().topLeft())) - offsGen;
    QPointF pt = boundingRect().topLeft();
    QPointF LTgen = mapToScene(pt);
    Xgen = static_cast<int>(LTgen.x());
    Ygen = static_cast<int>(LTgen.y());
    emit ROIRegionChange(Xgen, Ygen, w, h);
}

void Cropper::delself()
{
    emit deleteROI();
    //delete this;
}

QPixmap Cropper::convertToGray(QPixmap& pxmp)
{
    QImage image = pxmp.toImage();
    QRgb col;
    int gray;
    int width = pxmp.width();
    int height = pxmp.height();
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            col = image.pixel(i, j);
            gray = qGray(col);
            image.setPixel(i, j, qRgb(gray, gray, gray));
        }
    }
    return QPixmap::fromImage(image);
}

void Cropper::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QMenu menu;
    QAction* removeAction = new QAction("Delete ROI", &menu);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(delself()));
    menu.addAction(removeAction);
    menu.exec(event->screenPos());

}

void Cropper::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    emit mouseHoverEnter();
}
