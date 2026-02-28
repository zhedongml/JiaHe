#include "MLColorimeterMode.h"
#include "MetricsWidgetBase.h"
#include <QMessageBox>

MetricsWidgetBase::MetricsWidgetBase(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : QWidget(parent), m_viewer(viewer), m_menu(menu)
{
    initSence();
}

MetricsWidgetBase::~MetricsWidgetBase()
{
}

void MetricsWidgetBase::setFovCenter(const QMap<int, cv::Point2f>& fovCenterMap)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    m_fovCenterMap = fovCenterMap;
}

void MetricsWidgetBase::setMeasureWidgetUI(Ui::MeasureWidgetclass* ui)
{
    m_measureWidgetUi = ui;
}

void MetricsWidgetBase::graphViewcustom(const QPoint& pos)
{
    m_curpost.setX(pos.x());
    m_curpost.setY(pos.y());
}

void MetricsWidgetBase::cropperMousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Cropper* crp = (Cropper*)sender();
    drawIndex(crp);
}

void MetricsWidgetBase::loadImg(int x, int y, int width, int height)
{
    Cropper* cropper = (Cropper*)sender();
    drawIndex(cropper);
}

void MetricsWidgetBase::addAction()
{
    QPoint point = m_cropperWidthHeight;
    cv::Mat image = getCurrentImage();
    if (!image.empty()) {
        // point = point / (round((double)7920 / image.cols));
        int binnum = sqrt((11264 * 9200) / (image.total()));
        point = point / binnum;
    }

    QPointF scenePos = m_viewer->mapToScene(m_curpost);
    addOneCropper(scenePos.x(), scenePos.y(), point.x(), point.y());
}

void MetricsWidgetBase::deleteAction()
{
    QGraphicsItem* item = m_viewer->itemAt(m_curpost);
    Cropper* cropper = dynamic_cast<Cropper*>(item);
    if (cropper != NULL)
    {
        deleteCropper(cropper);
    }
}

void MetricsWidgetBase::deleteAllAction()
{
    for (Cropper* cropper : m_crossAreas)
    {
        if (cropper != NULL)
        {
            deleteCropper(cropper);
        }
    }

    m_viewer->ClearALLROI();
}

void MetricsWidgetBase::deleteCropper(Cropper* cropper)
{
    m_crossAreas.removeOne(cropper);

    QGraphicsSimpleTextItem* txtitem = (QGraphicsSimpleTextItem*)cropper->property("INDEX_ITEM").value<void*>();
    if (txtitem != NULL)
    {
        delete txtitem;
        txtitem = NULL;
    }

    delete cropper;
    cropper = NULL;
}

void MetricsWidgetBase::accordionChanged(ContentPane* cpane)
{
    if (m_cpane == cpane)
    {
        m_isCurrentAccordion = true;
        menuUpdate(m_menu, true);
    }
    else
    {
        m_isCurrentAccordion = false;
        menuUpdate(m_menu, false);
        deleteAllAction();
    }
    initClear();
}

void MetricsWidgetBase::setDialogViewer(CameraViewer* viewer)
{
    m_dialogViewer = viewer;
}

ROIParaNew MetricsWidgetBase::fovROI()
{
    ROIParaNew param;
    param.step = m_measureWidgetUi->doubleSpinBox_fovROIStep->value();
    param.cenX = m_measureWidgetUi->doubleSpinBox_fovCenterX->value();
    param.cenY = m_measureWidgetUi->doubleSpinBox_fovCenterY->value();
    param.width = m_measureWidgetUi->doubleSpinBox_fovWidthX->value();
    param.height = m_measureWidgetUi->doubleSpinBox_fovHeightY->value();
    Result ret /*= MLColorimeterMode::instance()->getRotationMirror(param.rotationAngle, param.mirror)*/;
    if (!ret.success)
    {
        QMessageBox::critical(this, "Get rotation and mirror info error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
    }
    return param;
}

void MetricsWidgetBase::initSence()
{
    connect(m_viewer, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(graphViewcustom(const QPoint&)));
}

void MetricsWidgetBase::initAccordionBase(QAccordion* accordion, QString name, int maxHeight, bool openContentPane)
{
    accordion->setProperty(IQ_METRICS_TEST_MODE_OFFLINE, true);

    m_accordion = accordion;
    int index = accordion->addContentPane(name);
    QFrame* frame = accordion->getContentPane(index)->getContentFrame();
    accordion->getContentPane(index)->setMaximumHeight(maxHeight);

    frame->setLayout(new QVBoxLayout());
    frame->layout()->addWidget(this);

    m_cpane = accordion->getContentPane(index);
    QObject::connect(accordion, SIGNAL(contentPaneClicked(ContentPane*)), this, SLOT(accordionChanged(ContentPane*)));

    if (openContentPane)
    {
        accordionChanged(m_cpane);
        m_cpane->openContentPane();
        m_isCurrentAccordion = true;
    }
}

void MetricsWidgetBase::drawIndex(Cropper* cropper)
{
    QGraphicsSimpleTextItem* oldTxtitem = (QGraphicsSimpleTextItem*)cropper->property("INDEX_ITEM").value<void*>();
    if (oldTxtitem != NULL)
    {
        delete oldTxtitem;
        oldTxtitem = NULL;
    }

    QPen txtpen;
    // txtpen.setColor(QColor(255, 127, 80));
    txtpen.setColor(QColor(0, 255, 255));

    int fontSize = 50 * cropper->height / 200;
    if (fontSize > 50)
    {
        fontSize = 50;
    }
    if (fontSize < 5)
    {
        fontSize = 5;
    }

    int index = cropper->Index;
    QGraphicsSimpleTextItem* txtitem = new QGraphicsSimpleTextItem(QString("").number(index).append("#"));
    txtitem->setPen(txtpen);
    txtitem->setFont(QFont("Arial", fontSize));
    txtitem->setFlag(QGraphicsItem::ItemIsMovable);
    txtitem->setPos(cropper->Xgen + 5, cropper->Ygen);

    QGraphicsScene* scene = m_viewer->GetScene();
    scene->addItem(txtitem);

    cropper->setProperty("INDEX_ITEM", QVariant::fromValue((void*)txtitem));
}

int MetricsWidgetBase::getNewIndex(QList<Cropper*> crossAreas)
{
    int index = 0;
    while (index <= CROPPER_INDEX_MAX)
    {
        bool same = false;
        for (Cropper* copper : crossAreas)
        {
            if (index == copper->Index)
            {
                same = true;
            }
        }
        if (!same)
        {
            return index;
        }
        ++index;
    }
}

void MetricsWidgetBase::addOneCropper(int x, int y, int width, int height)
{
    int index = getNewIndex(m_crossAreas);
    addOneCropper(index, x, y, width, height);
}

void MetricsWidgetBase::addOneCropper(int index, int x, int y, int width, int height)
{
    if (m_crossAreas.size() >= CROPPER_INDEX_MAX)
    {
        return;
    }
    QPointF scenePos = m_viewer->mapToScene(m_curpost);

    QString curImgPath = "";
    Cropper* crp = new Cropper(this, curImgPath, x, y, width, height);
    crp->setZValue(1);

    crp->Index = index;

    QGraphicsScene* scene = m_viewer->GetScene();
    scene->addItem(crp);
    m_crossAreas.append(crp);
    connect(crp, SIGNAL(ROIRegionChange(int, int, int, int)), this, SLOT(loadImg(int, int, int, int)));
    connect(crp, SIGNAL(mousePressEventSignal(QGraphicsSceneMouseEvent*)), this,
        SLOT(cropperMousePressEvent(QGraphicsSceneMouseEvent*)));

    drawIndex(crp);
}

void MetricsWidgetBase::addOneCropper(int index, cv::Rect rect)
{
    addOneCropper(index, rect.x, rect.y, rect.width, rect.height);
}

void MetricsWidgetBase::menuUpdate(QMenu* menu, bool isCrossArea)
{
    if (isCrossArea)
    {
        for (QAction* action : menu->actions())
        {
            if (!action->data().isNull() && action->data() == MENU_NAME)
            {
                action->setVisible(true);
            }
            else
            {
                action->setVisible(false);
            }
        }
    }
}

bool MetricsWidgetBase::isOffline()
{
    return m_accordion->property(IQ_METRICS_TEST_MODE_OFFLINE).toBool();
}

cv::Mat MetricsWidgetBase::getCurrentImage(bool isRawImage)
{
    cv::Mat image;
    if (isOffline())
    {
        image = m_viewer->currentImage().clone();
    }
    else
    {
        cv::Mat rawImage;
        Result ret /*= MLColorimeterMode::instance()->takeImage(rawImage)*/;
        if (!ret.success) {
            return cv::Mat();
        }

        //m_viewer->Enqueue(rawImage);

        if (isRawImage) {
            image = rawImage;
        }
        else {
            Result ret /*= McBinocularMode::instance()->calculateYImage(image)*/;
            if (!ret.success)
            {
                return cv::Mat();
            }
        }
    }
    return image;
}

Result MetricsWidgetBase::judgeImages(const QList<cv::Mat>& images, bool isRawImage)
{
    if (images.size() == 0) {
        return Result(false, "Images is NULL.");
    }

    if (images.size() > 1) {
        cv::Size imgSize = images.at(0).size();
        for (const cv::Mat& image : images)
        {
            if (imgSize != image.size())
            {
                return Result(false, "All images have different sizes.");
            }
        }
    }

    if (!isRawImage)
    {
        for (const cv::Mat& image : images)
        {
            if (image.depth() != 32)
            {
                return Result(false, "Please load the calibrated image, not raw images.");
            }
        }
    }
    return Result();
}

QMap<int, cv::Point2f> MetricsWidgetBase::fovCenter()
{
    return m_fovCenterMap;
}

Result MetricsWidgetBase::judgeImages(const cv::Mat& image, bool isRawImage)
{
    QList<cv::Mat> imgList;
    imgList.append(image);
    return judgeImages(imgList, isRawImage);
}

Result MetricsWidgetBase::judgeImages(const QMap<QString, cv::Mat>& imageMap, bool isRawImage)
{
    QList<cv::Mat> imgList;
    for (QMap<QString, cv::Mat>::const_iterator iter = imageMap.begin(); iter != imageMap.end(); ++iter)
    {
        imgList.append(iter.value());
    }

    return judgeImages(imgList, isRawImage);
}
