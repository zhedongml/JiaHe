#include "ExposureWidget.h"
#include <QTimer>
#include <QValidator>
#include <QtConcurrent>
#include <QFuture>
#include "PrjCommon/WaitWidget.h"
#include "FilterwheelWidget.h"
#include <Core/icore.h>
#include <fstream>
#include <QMessageBox>
#include "PrjCommon/service/helper.h"

ExposureWidget::ExposureWidget(QString toolBoxName, QWidget *parent) : Core::IToolBox(toolBoxName, parent)
{
    ui.setupUi(this);
    QDoubleValidator *dbV = new QDoubleValidator(this);
    dbV->setBottom(0.00);
    dbV->setDecimals(3);
    dbV->setTop(5000.00);
    ui.exposuretimetxt->setValidator(dbV);
    connect(MLColorimeterMode::Instance(), SIGNAL(updateCameraGrayLevelSignal(int)), this, SLOT(ShowGrayValue(int)));
    connect(MLColorimeterMode::Instance(), &MLColorimeterMode::updateCameraImageSignal, this, &ExposureWidget::updateCameraImage);
    QtConcurrent::run(this, &ExposureWidget::calculateThread);
    
    ui.statusLabel->setText("");
    ui.comboBox_binningMode->setCurrentIndex(1);

    qRegisterMetaType<Result>("Result");
    connect(this, SIGNAL(sendAEStatus(Result)), this, SLOT(UpdateAEStatus(Result)));
    //ui.curBinningBtn->setVisible(false);
    //ui.curtBitdepthBtn->setVisible(false);
    //ui.btn_binningMode->setVisible(false);
    ui.statusLabel->setWordWrap(true);
    connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(connectStatus(bool)));
    connect(Core::ICore::instance(), SIGNAL(currentFocusChange(QString)), this, SLOT(updateCurrentFocus(QString)));
    //bool b = connect(&watcher, SIGNAL(finished()), this, SLOT(setEnableFinish()));
    bool b2 = connect(this, SIGNAL(sendFocusUi(bool)), this, SLOT(setFocusUi(bool)));
    bool k=connect(ui.widget_2, &FilterwheelWidget::sendRefreshBtn, this,
            [=](bool enable) { ui.btnRefresh->setEnabled(enable);
        });
    int u ;

    connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, this, [this]() { 
        m_threadStart = false; 
        });
    readFile();

    setMinimumSize(0, 0);
}

ExposureWidget::~ExposureWidget()
{
    m_threadStart = false;
    Sleep(500);
}

void ExposureWidget::ShowGrayValue(int gray)
{
    ui.graylevelbar->setValue(gray * 100 / 255);
    double exposurtime = MLColorimeterMode::Instance()->GetExposureTime();
    ui.curExposureTimeTxt->setText(QString::number(exposurtime, 'f', 3));
}

void ExposureWidget::updateCameraImage(const cv::Mat &img)
{
    QMutexLocker lock(&m_mutex);
    if (m_images.size() == 3)
    {
        m_images.pop_front();
    }
    m_images.push_back(img);
}

void ExposureWidget::on_SetExposureTime_clicked()
{
    ui.statusLabel->setText("");

    QString exposureTime = ui.exposuretimetxt->text();
    ExposureSetting setting;
    setting.Mode = ExposureMode::Fixed;
    setting.ExposureTime = exposureTime.toDouble();
    Result ret = MLColorimeterMode::Instance()->SetExposureTime(setting);
    if (!ret.success)
    {
        ui.statusLabel->setText("AutoExposure Status:" + QString::fromStdString(ret.errorMsg));
    }
}

void ExposureWidget::on_exposuretimetxt_returnPressed()
{
    ui.statusLabel->setText("");

    QString exposureTime = ui.exposuretimetxt->text();
    MLColorimeterMode::Instance()->SetExposureTime(exposureTime.toDouble());
}

void ExposureWidget::on_curBinningSelectorBtn_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::BinningSelector bin = MLColorimeterMode::Instance()->GetMonocular()->ML_GetBinningSelector();
        switch (bin)
        {
        case ML::CameraV2::BinningSelector::Sensor:
            ui.biningselectorCombox->setCurrentIndex(0);
            break;
        case ML::CameraV2::BinningSelector::Logic:
            ui.biningselectorCombox->setCurrentIndex(1);
            break;
        default:
            break;
        }
        //connect(ui.biningCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_biningCombox_activated(int)));
    }
}

void ExposureWidget::on_curBinningBtn_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::Binning bin = MLColorimeterMode::Instance()->GetMonocular()->ML_GetBinning();
        //disconnect(ui.biningCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_biningCombox_activated(int)));
        switch (bin)
        {
        case ML::CameraV2::Binning::ONE_BY_ONE:
            ui.biningCombox->setCurrentIndex(0);
            break;
        case ML::CameraV2::Binning::TWO_BY_TWO:
            ui.biningCombox->setCurrentIndex(1);
            break;
        case ML::CameraV2::Binning::FOUR_BY_FOUR:
            ui.biningCombox->setCurrentIndex(2);
            break;
        case ML::CameraV2::Binning::EIGHT_BY_EIGHT:
            break;
        default:
            break;
        }
        //connect(ui.biningCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_biningCombox_activated(int)));
    }
}

void ExposureWidget::on_curtBitdepthBtn_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        //disconnect(ui.bitdepthCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_bitdepthCombox_activated(int)));
        ML::CameraV2::MLPixelFormat type = MLColorimeterMode::Instance()->GetMonocular()->ML_GetPixelFormat();
        switch (type)
        {
        case ML::CameraV2::MLPixelFormat::MLMono8:
            ui.bitdepthCombox->setCurrentIndex(0);
            break;
        case ML::CameraV2::MLPixelFormat::MLMono10:
        case ML::CameraV2::MLPixelFormat::MLMono12:
        case ML::CameraV2::MLPixelFormat::MLMono16:
            ui.bitdepthCombox->setCurrentIndex(1);
            break;
        default:
            break;
        }
        //connect(ui.bitdepthCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_bitdepthCombox_activated(int)));
    }
}

void ExposureWidget::on_btnRefresh_clicked()
{
    on_curBinningSelectorBtn_clicked();
    on_curBinningBtn_clicked();
    on_curtBitdepthBtn_clicked();
    on_btn_binningMode_clicked();
    ui.widget_2->on_cur_NDbtn_clicked();
    ui.widget_2->on_cur_XYZbtn_clicked();
}

void ExposureWidget::setEnableFinish()
{
    cameraSetEnable(true);
    ui.btnRefresh->setEnabled(true);
}

void ExposureWidget::on_biningCombox_activated(int index)
{
    //cameraSetEnable(false);
    //ui.btnRefresh->setEnabled(false);
    //QFuture<void> future = QtConcurrent::run([=] {
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::Binning bin = ML::CameraV2::Binning::ONE_BY_ONE;
        switch (index)
        {
        case 0:
            bin = ML::CameraV2::Binning::ONE_BY_ONE;
            break;
        case 1:
            bin = ML::CameraV2::Binning::TWO_BY_TWO;
            break;
        case 2:
            bin = ML::CameraV2::Binning::FOUR_BY_FOUR;
            break;
        default:
            break;
        }

        Result ret = MLColorimeterMode::Instance()->SetBinning(bin);
        if (!ret.success)
        {
            QMessageBox::warning(this, "Error", "Measure camera set binning error.", QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
    //});
    //watcher.setFuture(future);
}

void ExposureWidget::on_biningselectorCombox_activated(int index)
{
    //cameraSetEnable(false);
    //ui.btnRefresh->setEnabled(false);
    //QFuture<void> future = QtConcurrent::run([=] {
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::BinningSelector selector = ML::CameraV2::BinningSelector::Sensor;
        switch (index)
        {
        case 0:
            selector = ML::CameraV2::BinningSelector::Sensor;
            break;
        case 1:
            selector = ML::CameraV2::BinningSelector::Logic;
            break;
        default:
            break;
        }

        Result ret = MLColorimeterMode::Instance()->SetBinningSelector(selector);
        if (!ret.success)
        {
            QMessageBox::warning(this, "Error", "Measure camera set binning selector error.", QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
    //});
    //watcher.setFuture(future);
}

void ExposureWidget::on_bitdepthCombox_activated(int index)
{
    //cameraSetEnable(false);
    //ui.btnRefresh->setEnabled(false);
    //QFuture<void> future = QtConcurrent::run([=] {
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::MLPixelFormat format = ML::CameraV2::MLPixelFormat::MLMono8;
        switch (index)
        {
        case 0:
            format = ML::CameraV2::MLPixelFormat::MLMono8;
            break;
        case 1:
            format = ML::CameraV2::MLPixelFormat::MLMono12;
            break;
        default:
            break;
        }

        Result ret = MLColorimeterMode::Instance()->SetBitDepth(format);
        if (!ret.success)
        {
            QMessageBox::warning(this, "Error", "Measure camera set error.", QMessageBox::Ok,
                                 QMessageBox::NoButton);
        }
    }
}

void ExposureWidget::on_comboBox_binningMode_activated(int index)
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        ML::CameraV2::BinningMode format = ML::CameraV2::BinningMode::AVERAGE;
        switch (index)
        {
        case 0:
            format = ML::CameraV2::BinningMode::SUM;
            break;
        case 1:
            format = ML::CameraV2::BinningMode::AVERAGE;
            break;
        default:
            break;
        }

        Result ret = MLColorimeterMode::Instance()->SetBinningMode(format);
        if (!ret.success)
        {
            QMessageBox::warning(this, "Error", "Measure camera set error.", QMessageBox::Ok,
                                    QMessageBox::NoButton);
        }
    }
}

void ExposureWidget::on_btn_binningMode_clicked()
{
    GrabberBase *cam = MLColorimeterMode::Instance()->GetCamera();
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        BinningMode type = cam->GetBinningMode();
        switch (type)
        {
        case ML::CameraV2::BinningMode::SUM :
            ui.comboBox_binningMode->setCurrentIndex(0);
            break;
        case ML::CameraV2::BinningMode::AVERAGE:
            ui.comboBox_binningMode->setCurrentIndex(1);
            break;
        default:
            break;
        }
    }
}

void ExposureWidget::on_autoexposurebtn_clicked()
{
    ui.statusLabel->setText("AutoExposure...");
    WaitWidget::instance()->startAnimation();
    //qRegisterMetaType<MLCameraState>("ML::AutoExposureStatus");
    QFuture<void> future = QtConcurrent::run([=] {
        ExposureSetting exposure = { ExposureMode::Auto, ui.initTime->text().toDouble() };
        Result ret = MLColorimeterMode::Instance()->SetExposureTime(exposure);
        emit sendAEStatus(ret);
    });
}

void ExposureWidget::UpdateAEStatus(Result status)
{
    if(status.success){
        ui.statusLabel->setText("AutoExposure Fnished!");
        ui.exposuretimetxt->setText(QString::number((double)MLColorimeterMode::Instance()->GetBinning()));

    }else{
        ui.statusLabel->setText("AutoExposure Status:" + QString::fromStdString(status.errorMsg));
    }
    MLColorimeterMode::Instance()->StopGrabbing(false);
    WaitWidget::instance()->stopAnimation();
}

void ExposureWidget::connectStatus(bool connected)
{
    on_curBinningSelectorBtn_clicked();
    on_curBinningBtn_clicked();
    on_curtBitdepthBtn_clicked();
    on_btn_binningMode_clicked();
}

void ExposureWidget::cameraSetEnable(bool enable)
{
    ui.biningCombox->setEnabled(enable);
    ui.bitdepthCombox->setEnabled(enable);
    ui.comboBox_binningMode->setEnabled(enable);
}

void ExposureWidget::calculateThread()
{
    while (m_threadStart)
    {
        if (MLColorimeterMode::Instance()->IsConnect())
        {
            cv::Mat img = getImage();
            if (img.empty())
            {
                continue;
            }
            int gray = calculateGray(img);
            emit MLColorimeterMode::Instance()->updateCameraGrayLevelSignal(gray);
        }
        Sleep(400);
    }
}

int ExposureWidget::calculateGray(const cv::Mat &curImge)
{
    try
    {
        int sum = 0;
        Mat img;
        if (curImge.empty())
        {
            return 0;
        }
        curImge.copyTo(img);

        int max_num_ = m_maxNumberGray;
        if (img.type() == CV_8UC1)
        {
            int gray = Helper::GetGrayValue2(img, max_num_);
            return gray;
        }
        else if (img.type() == CV_16UC1)
        {
            int gray = Helper::GetGrayValue2(img, max_num_);
            gray = gray * 255.0 / 4095.0;
            return gray;
        }
    }
    catch (exception e)
    {
        qWarning() << "ExposureWidget::calculateGray exception " + QString::fromStdString(e.what());
        return 0;
    }
    
}

cv::Mat ExposureWidget::getImage()
{
    QMutexLocker lock(&m_mutex);
    cv::Mat res;
    if (!m_images.isEmpty())
    {
        try
        {
            res = m_images.front();
            m_images.pop_front();
        }
        catch (...)
        {
        }
    }
    return res;
}

Result ExposureWidget::readFile(const QString &fileName)
{
    std::ifstream jsonFile(fileName.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Open config file failed, %1.").arg(fileName).toStdString());
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();

    Json jsonObj = Json::parse(contents);

    auto AEConfig = jsonObj["AutoExposure"];
    for (auto &it : AEConfig.items())
    {
        m_maxNumberGray = it.value()["IsUseROIs"]["pixelTotalNumber"].get<int>();
        break;
    }
    return Result();
}
