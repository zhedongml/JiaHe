#include "MtfWidget.h"
#include "NineSquareMTFModel.h"
//#include "ml_multiCrossHairDetection.h"
#include"ImageDetection/ml_multiCrossHairDetection.h"
#include"ImageDetection/MTFRectangleDetect.h"
#include "PrjCommon/metricsdata.h"


MtfWidget::MtfWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent)
    : MetricsWidgetBase(viewer, menu, parent)
{
    ui.setupUi(this);
    m_mtfDialog = nullptr;

    initUI();
    initMenu(menu);
    initAccordion((QAccordion*)parent);

    //connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}

MtfWidget::~MtfWidget()
{
    if (m_mtfDialog != nullptr) {
        delete m_mtfDialog;
        m_mtfDialog = nullptr;
    }
}

void MtfWidget::on_btn_calculate_clicked()
{
    if (m_mtfDialog != nullptr) {
        delete m_mtfDialog;
        m_mtfDialog = nullptr;
    }

    m_mtfDialog = new MtfDialog(m_viewer, this);
    m_mtfDialog->setTableShowFreq(getTableFreqs());
    Result ret = calculateMetrics();

    if (!ret.success)
    {
        QMessageBox::critical(this, "MTF calculate error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
        return;
    }

    m_mtfDialog->exec();
    m_viewer->ClearALLROI();
}

void MtfWidget::on_comboBox_fov_activated(int index)
{
    cv::Mat image = m_viewer->currentImage();
    if (image.empty())
    {
        return;
    }

    if (sqrt(11264 * 9200 / image.total()) > 1)
    {
        QMessageBox::critical(this, "MTF switch FOV error", "Please check image binning 1X1.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }

    deleteAllAction();

    // Get AOI by algorithm
   // NineSquareMTFModel *mtfModel = NineSquareMTFModel::getInstance();
   // std::vector<cv::Rect> aoiRects = mtfModel->autoIdentifyNineCrossRect(m_viewer->currentImage());
  //  MLImageDetection::MultiCrossHairDetection multi;
    // TODO: to add the LEFT and right flag,now is 0 left
//    MultiCrossHairRe re = multi.getMuliCrossHairCenterByDistance(m_viewer->currentImage(), MetricsData::instance()->getReticleEyeType(), 1);
    MLImageDetection::MTFRectangleDetect multi;
    MTFRectRe re= multi.getMtfRectangleRect(m_viewer->currentImage());

    std::vector<cv::Rect> aoiRects = re.rectVec;
    for (int j = 0; j < aoiRects.size(); j++)
    {
        addOneCropper(j, aoiRects[j]);
    }
}

void MtfWidget::radioToggled(bool checked)
{
    deleteAllAction();

    QRadioButton* radio = (QRadioButton*)sender();
    if (ui.radioBtn_fov == radio) {
        if (checked) {
            on_comboBox_fov_activated(ui.comboBox_fov->currentIndex());
        }
        ui.comboBox_fov->setEnabled(checked);
    }
    else {
        ui.comboBox_fov->setEnabled(false);
    }

    if (ui.radioBtn_edge == radio) {
        ROIPosConfig roiConfig = MTFModelConfig::instance()->getROIPosCfg();
        m_cropperWidthHeight.setX(roiConfig.roiWidth);
        m_cropperWidthHeight.setY(roiConfig.roiHeight);
    }
    else {
        m_cropperWidthHeight.setX(300);
        m_cropperWidthHeight.setY(300);
    }
}

void MtfWidget::initUI()
{
    ui.comboBox_fov->clear();
    ui.comboBox_fov->addItem("0");
    ui.comboBox_fov->addItem("+/-7");
    ui.comboBox_fov->addItem("+/-14");
    ui.comboBox_fov->addItem("+/-20");
    ui.comboBox_fov->addItem("+/-25");
    ui.comboBox_fov->addItem("ALL");

    connect(ui.radioBtn_fov, SIGNAL(toggled(bool)), this, SLOT(radioToggled(bool)));
    connect(ui.radioBtn_rect, SIGNAL(toggled(bool)), this, SLOT(radioToggled(bool)));
    connect(ui.radioBtn_edge, SIGNAL(toggled(bool)), this, SLOT(radioToggled(bool)));

    ui.radioBtn_edge->setChecked(true);
    ui.comboBox_fov->setEnabled(false);

    CROPPER_INDEX_MAX = 14;
    MENU_NAME = "menuMtf";

    ui.lineEdit_freqMin->setText("0");
    ui.lineEdit_freqMax->setText("50");
    ui.lineEdit_freqInterval->setText("7.5");

    ui.comboBox_fov->hide();
}

void MtfWidget::initAccordion(QAccordion* accordion)
{
    initAccordionBase(accordion, "MTF Calculate", 500, true);
}

void MtfWidget::initMenu(QMenu* menu)
{
    QAction* addCrossAreaAction = new QAction("Add ROI", menu);
    QAction* delCrossAreaAction = new QAction("Delete ROI", menu);
    QAction* deleteAllCrossAreaAction = new QAction("Delete All ROI", menu);

    addCrossAreaAction->setData(MENU_NAME);
    delCrossAreaAction->setData(MENU_NAME);
    deleteAllCrossAreaAction->setData(MENU_NAME);

    menu->addAction(addCrossAreaAction);
    menu->addAction(delCrossAreaAction);
    menu->addAction(deleteAllCrossAreaAction);

    menuUpdate(menu, false);

    connect(addCrossAreaAction, SIGNAL(triggered()), this, SLOT(addAction()));
    connect(delCrossAreaAction, SIGNAL(triggered()), this, SLOT(deleteAction()));
    connect(deleteAllCrossAreaAction, SIGNAL(triggered()), this, SLOT(deleteAllAction()));
}

QMap<int, cv::Rect> MtfWidget::getRois()
{
    QMap<int, cv::Rect> rois;
    for (int i = 0; i < m_crossAreas.size(); ++i) {
        Cropper* cropper = m_crossAreas.at(i);
        rois.insert(cropper->Index, cv::Rect(cropper->Xgen, cropper->Ygen, cropper->width, cropper->height));
    }
    return rois;
}

MTF_FOV_TYPE MtfWidget::currentFov()
{
    MTF_FOV_TYPE type = ALL;
    if (ui.comboBox_fov->currentIndex() == 0)
    {
        type = CENTER;
    }
    else if (ui.comboBox_fov->currentIndex() == 1)
    {
        type = EIGHT;
    }
    else if (ui.comboBox_fov->currentIndex() == 2)
    {
        type = SIXTEEN;
    }
    else if (ui.comboBox_fov->currentIndex() == 3)
    {
        type = TWENTY;
    }
    else if (ui.comboBox_fov->currentIndex() == 4)
    {
        type = TWENTYFIVE;
    }
    return type;
}

std::vector<std::string> MtfWidget::getTableFreqs()
{
    float min = ui.lineEdit_freqMin->text().trimmed().toFloat();
    float max = ui.lineEdit_freqMax->text().trimmed().toFloat();
    float interval = ui.lineEdit_freqInterval->text().trimmed().toFloat();

    std::vector<std::string> freqs;
    for (float i = min; i < max; i += interval)
    {
        freqs.push_back(QString::number(i, 'f', 2).toStdString());
    }
    freqs.push_back(QString::number(max, 'f', 2).toStdString());

    return freqs;
}

Result MtfWidget::calculateMetrics()
{
    try {
        cv::Mat rawImage = getCurrentImage();
        if (rawImage.empty())
        {
            return Result(false, "Please load the image first.");
        }

        if (sqrt(11264 * 9200 / rawImage.total()) > 1)
        {
            return Result(false, "Please check image binning 1X1.");
        }

        QMap<int, cv::Rect> rect = getRois();

        Result ret;
        if (ui.radioBtn_fov->isChecked())
        {
            ret = m_mtfDialog->calculateRect(rawImage, rect);
        }
        else if (ui.radioBtn_rect->isChecked())
        {
            ret = m_mtfDialog->calculateRect(rawImage, rect);
        }
        else
        {
            ret = m_mtfDialog->calculateEdge(rawImage, rect);
        }
        return ret;
    }
    catch (std::exception e)
    {
        return Result(false, "Calculate MTF exception, " + std::string(e.what()));
    }
}


