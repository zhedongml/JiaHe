#include "DeviceControlWidget2D.h"
#include <QtConcurrent>
#include <QMessageBox>
#include <plugins/PrjCommon/metricsdata.h>
#include "piMotionActor/configItem.h"
#include "piMotionActor/MoveLimitJX.h"

DeviceControlWidget2D::DeviceControlWidget2D(QString toolboxName, QWidget* parent)
    : IToolBox(toolboxName, parent)
{
    m_type = ACS2DDUTPlane;
    ui.setupUi(this);
    init();
}

DeviceControlWidget2D::~DeviceControlWidget2D()
{
}

float DeviceControlWidget2D::getStep()
{
    return ui.comboBox_step->currentText().toFloat();
}

float DeviceControlWidget2D::getSpeed()
{
    return ui.lineEdit_speed->text().toFloat();
}

void DeviceControlWidget2D::setDisenable(bool isEnable)
{
    ui.btn_absMove->setEnabled(isEnable);
}

void DeviceControlWidget2D::updateMotionStatusSlot(MLMotionState status)
{
    if (status == MLMotionState::kMLStatusDisConnected)
    {
        Motion2DModel::getInstance(m_type)->Motion2DUnsubscribeMotionEvent(MLMotionEvent::kPositionChanged);
        ui.label_status->setText("Not Connected.");
    }
    else
    {
        ui.label_status->setText("Connected.");

        Motion2DModel::getInstance(m_type)->Motion2DSubscribeMotionEvent(MLMotionEvent::kPositionChanged);
        Motion2DModel::getInstance(m_type)->SetSpeed(getSpeed());
    }
}

void DeviceControlWidget2D::sliderValueChangedSlot(int value) {
    Motion2DModel::getInstance(m_type)->SetSpeed(value);
    ui.lineEdit_speed->setText(QString::number(value));
}

void DeviceControlWidget2D::on_btn_absMove_clicked()
{
    if (ui.lineEdit_x->text().trimmed() == ""
        || ui.lineEdit_y->text().trimmed() == "")
    {
        return;
    }
    cv::Point2f coord;
    coord.x = ui.lineEdit_x->text().toFloat();
    coord.y = ui.lineEdit_y->text().toFloat();

    ui.widget_motion->setEnabled(false);
    setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &DeviceControlWidget2D::moveAbs, coord, m_type);
    watcher.setFuture(future);
}

void DeviceControlWidget2D::on_btn_stop_clicked()
{
    Motion2DModel::getInstance(m_type)->Motion2DMoveStop();
}

void DeviceControlWidget2D::on_btn_toSafe_clicked()
{
    setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(this, &DeviceControlWidget2D::moveAbsSafePos, m_type);
    watcher.setFuture(future);
}

void DeviceControlWidget2D::handleFinished()
{
    this->setEnabled(true);

    Result result = watcher.future().result();
    if (!result.success)
    {
        QMessageBox::critical(this, "Device move error", QString::fromStdString(result.errorMsg), QMessageBox::Ok,
            QMessageBox::NoButton);
    }
}

void DeviceControlWidget2D::init()
{
    ui.widget_motion->setTitle("");
    ui.widget_motion->setParentWidget(this);
    ui.label_status->setText("Not Connected");
    ui.label_status->setWordWrap(true);

    ui.groupBox->setTitle("DUT Move Station");
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

    initSolider();
    initCombox();
    bool v = connect(ui.widget_motion, &MotionMoveWidget2D::moveEnabled, this, [=](bool isEnable)
        { setDisenable(isEnable); });

    connect(Motion2DModel::getInstance(m_type), SIGNAL(updateMotion2DStatusSignal(MLMotionState)), this, SLOT(updateMotionStatusSlot(MLMotionState)));
}

void DeviceControlWidget2D::initSolider()
{
    QSlider* pSlider = ui.hSlider_speed;
    pSlider->setOrientation(Qt::Horizontal);
    pSlider->setMinimum(1);
    pSlider->setMaximum(20);
    pSlider->setSingleStep(1);
    pSlider->setPageStep(1);

    cv::Point3f speed = MoveLimitConfigJX::instance()->getMotion3DMoveInitSpeed();
    pSlider->setValue(speed.x);
    //pSlider->setValue(RobotModel::getInstance()->getLinespeedNormal());

    ui.lineEdit_speed->setText(QString::number(pSlider->value()));
    ui.lineEdit_speed->setEnabled(false);
    connect(pSlider, &QSlider::valueChanged, this, &DeviceControlWidget2D::sliderValueChangedSlot);
}

Result DeviceControlWidget2D::moveAbs(const cv::Point2f& coord, int m_type)
{
    return MoveLimitJX::dutMoveAbsNoCollision(coord);
}

Result DeviceControlWidget2D::moveAbsSafePos(int m_type)
{
    return MoveLimitJX::moveDutAbsToSafe();
}

void DeviceControlWidget2D::initCombox()
{
    ui.comboBox_step->addItem("0.001");
    ui.comboBox_step->addItem("0.01");
    ui.comboBox_step->addItem("0.1");
    ui.comboBox_step->addItem("0.5");
    ui.comboBox_step->addItem("1");
    ui.comboBox_step->addItem("5");
    ui.comboBox_step->addItem("10");
    ui.comboBox_step->addItem("20");
    ui.comboBox_step->addItem("30");

    ui.comboBox_step->setCurrentIndex(3);
}

void DeviceControlWidget2D::setEnabled(bool enabled)
{
    ui.btn_absMove->setEnabled(enabled);
    ui.widget_motion->setEnabled(enabled);
}

Result DeviceControlWidget2D::on_btn_connect_clicked()
{
    QString ipEtc = ConfigItem::instance()->getMotion2DIpAndOther(m_type);
    Result result = Motion2DModel::getInstance(m_type)->connectAndInit(ipEtc);
    if (!result.success)
    {
        std::string errMsg = result.errorMsg;
        int index = errMsg.find_first_of(":");
        result.errorMsg = errMsg.substr(0, index);
        ui.label_status->setText("Disconnected." + QString::fromStdString(result.errorMsg));
        return result;
    }

    ui.label_status->setText("Connected");
    return Result();
}

void DeviceControlWidget2D::on_btn_disconnect_clicked()
{
    Motion2DModel::getInstance(m_type)->disconnect();
    ui.label_status->setText("Not Connected");
}



