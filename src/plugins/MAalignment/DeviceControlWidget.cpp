#include "DeviceControlWidget.h"
#include "piMotionActor\ConfigMAInfo.h"
#include <QtConcurrent>
#include <PrjCommon/metricsdata.h>
#include <piMotionActor/configItem.h>
#include <piMotionActor/MoveLimit.h>
#include <QMessagebox>

DeviceControlWidget::DeviceControlWidget(QString toolboxName, QWidget *parent)
    : IToolBox(toolboxName, parent)
{

    if (toolboxName == "Motion2D-Tilt Projector")
    {
        type=ACS2DPro;
        MetricsData::instance()->setMotion2DType(type);
    }

    else if (toolboxName == "Dut Tilt Station")
    {
        type=ACS2DDUT;
        MetricsData::instance()->setMotion2DType(type);
    }

    else if (toolboxName == "Motion2D-Tilt Imaging")
    {
        type = ACS2DCameraTilt;
        MetricsData::instance()->setMotion2DType(type);
    }
    ui.setupUi(this);
    init();
}

DeviceControlWidget::~DeviceControlWidget()
{
}

float DeviceControlWidget::getStep()
{
    QString s= ui.comboBox_step->currentText();
    float step= ui.comboBox_step->currentText().toFloat();
    return ui.comboBox_step->currentText().toFloat();
}

float DeviceControlWidget::getSpeed()
{
    return ui.lineEdit_speed->text().toFloat();
}

float DeviceControlWidget::getAccSpeed()
{
    return ui.acc_speed->text().toFloat();
}

float DeviceControlWidget::getDecSpeed()
{
    return ui.dec_speed->text().toFloat();
}
void DeviceControlWidget::sliderValueChangedSlot(int value)
{
    double curValue = value / 10.0;
    Motion2DModel::getInstance(type)->SetSpeed(curValue);
    ui.lineEdit_speed->setText(QString::number(curValue));
}

void DeviceControlWidget::decSliderValueChangedSlot(int value)
{
    //Motion2DModel::getInstance(type)->SetDecSpeed(value);
    ui.dec_speed->setText(QString::number(value));
}

void DeviceControlWidget::accSliderValueChangedSlot(int value)
{
    //Motion2DModel::getInstance(type)->SetAccSpeed(value);
    ui.acc_speed->setText(QString::number(value));
}

void DeviceControlWidget::on_btn_absMove_clicked()
{
    if (ui.lineEdit_x->text().trimmed() == "" || ui.lineEdit_y->text().trimmed() == "")
    {
        return;
    }
    float x = ui.lineEdit_x->text().toFloat();
    float y = ui.lineEdit_y->text().toFloat();
    setEnabled(false);
    QFuture<Result> future =
        QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Motion2DMoveAbs, x, y);
    watcher.setFuture(future);
}

void DeviceControlWidget::on_btn_zero_clicked()
{
    setEnabled(false);
    ui.btn_stop->setEnabled(false);
    Result res = isHoming();
    if (res.success)
    {
        setEnabled(true);
        ui.btn_stop->setEnabled(true);
        return;
    }
    if (res.errorMsg == "Havn't homing")
    {
        bool re=messageShow();
        if (re)
        {
            Result resul = MoveLimit::instance()->moveAbsToSafe((motion3DType)1);
            if (!resul.success)
            {
                ui.HomingStatus->setText("Initial moving is failed." + QString::fromStdString(resul.errorMsg));
                ui.HomingStatus->setWordWrap(true);
                setEnabled(true);
                ui.btn_stop->setEnabled(true);
                return;
            }
            else
            {
                ui.HomingStatus->setText("Initial moving succeed!");
            }
        }
    }
    ui.label_status->setText("Connected");
    QFuture<Result> future = QtConcurrent::run(Motion2DModel::getInstance(type), &Motion2DModel::Home);
    watcher.setFuture(future);  
}

void DeviceControlWidget::on_btn_stop_clicked()
{
    setEnabled(true);
    Motion2DModel::getInstance(type)->Motion2DMoveStop();
}

void DeviceControlWidget::handleFinished()
{
    emit sendDeviceMoveFinshed(true);
    bool connected = Motion2DModel::getInstance(type)->GetIsConnected();
    this->setEnabled(true);

    Result result = watcher.future().result();
    if (!result.success)
    {
        QMessageBox::critical(this, "Device move error", QString::fromStdString(result.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

void DeviceControlWidget::updateMotion2DStatusSlot(MLMotionState status)
{
    if (status == CORE::MLMotionState::kMLStatusDisConnected)
    {
        ui.label_status->setText("Not Connected.");
        Motion2DModel::getInstance(type)->Motion2DUnsubscribeMotionEvent(MLMotionEvent::kPositionChanged);
    }
    else{
        ui.label_status->setText("Connected.");

        Motion2DModel::getInstance(type)->Motion2DSubscribeMotionEvent(MLMotionEvent::kPositionChanged);
        Motion2DModel::getInstance(type)->SetSpeed(this->getSpeed());
    }
}

void DeviceControlWidget::updateMotion2DPositionOriginalSlot(float x, float y)
{
    if (Motion2DModel::getInstance(type)->Motion2DisConnected())
    {
        ui.label_coordOriginal->setText(QString(("(%1, %2)")).arg(QString::number(x, 'f', 3)).arg(QString::number(y, 'f', 3)));
    }
}

void DeviceControlWidget::init()
{
    if (type)
    {
        ui.btn_zero->hide();
        ui.isHoming->hide();
        ui.HomingStatus->hide();
    }
    ui.widget_motion->setTitle("");
    ui.widget_motion->setParentWidget(this);
    ui.label_status->setText("Not Connected.");
    ui.label_status->setWordWrap(true);
    connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

    initSolider();
    initCombox();
    ui.widget_2->setVisible(false);
    ui.widget_6->setVisible(false);
    //initTilt();
    
    if (type == ACS2DPro)
    {
    ui.groupBox_4->setTitle("Motion-Tilt Projector");
    }
    else if (type == ACS2DDUT)
    {
    ui.groupBox_4->setTitle("DUT Tilt Station");
    }
    else if (type == ACS2DCameraTilt)
    {
        ui.groupBox_4->setTitle("Motion-Tilt Imaging");
    }
    bool re = connect(ui.widget_motion, SIGNAL(sendMoveFinshed(bool)), this, SLOT(setDeviceMoveFinshed(bool)));
    /* connect(Motion2DModel::getInstance(), SIGNAL(updateMotion2DPositionSignal(float, float)), this,
            SLOT(update2d(float, float)));*/
    bool v =
        connect(ui.widget_motion, &MotionMoveWidget::moveEnabled, this, [=](bool isEnable) { setDisenable(isEnable); });

    connect(Motion2DModel::getInstance(type), SIGNAL(updateMotion2DStatusSignal(MLMotionState)), this, SLOT(updateMotion2DStatusSlot(MLMotionState)));
    connect(Motion2DModel::getInstance(type), SIGNAL(updateMotion2DPositionOriginalSignal(float, float)), this,
            SLOT(updateMotion2DPositionOriginalSlot(float, float)));

    QPixmap pixmap(":/image/image/direction.png");
    ui.markLabel->setPixmap(
        pixmap.scaledToWidth(
            ui.markLabel->width(),
            Qt::SmoothTransformation
        )
    );
}

void DeviceControlWidget::initSolider()
{
    QSlider *pSlider = ui.hSlider_speed;
    pSlider->setOrientation(Qt::Horizontal);
    pSlider->setMinimum(5);
    pSlider->setMaximum(30);
    pSlider->setSingleStep(1);
    pSlider->setPageStep(1);
    pSlider->setValue(10);
    // pSlider->setValue(RobotModel::getInstance()->getLinespeedNormal());
    ui.hSlider_acc_speed->setMinimum(0);
    ui.hSlider_acc_speed->setMaximum(5);
    ui.hSlider_acc_speed->setSingleStep(1);
    ui.hSlider_acc_speed->setPageStep(1);
    ui.hSlider_acc_speed->setValue(1);

    ui.hSlider_dec_speed->setMinimum(0);
    ui.hSlider_dec_speed->setMaximum(5);
    ui.hSlider_dec_speed->setSingleStep(1);
    ui.hSlider_dec_speed->setPageStep(1);
    ui.hSlider_dec_speed->setValue(1);
    double cur = pSlider->value();
    ui.lineEdit_speed->setText(QString::number(cur/10.0));
    ui.lineEdit_speed->setEnabled(false);
    ui.acc_speed->setText(QString::number(ui.hSlider_acc_speed->value()));
    ui.acc_speed->setEnabled(false);
    ui.dec_speed->setText(QString::number(ui.hSlider_dec_speed->value()));
    ui.dec_speed->setEnabled(false);
    connect(pSlider, &QSlider::valueChanged, this, &DeviceControlWidget::sliderValueChangedSlot);
    connect(ui.hSlider_acc_speed, &QSlider::valueChanged, this, &DeviceControlWidget::accSliderValueChangedSlot);
    connect(ui.hSlider_dec_speed, &QSlider::valueChanged, this, &DeviceControlWidget::decSliderValueChangedSlot);
}

void DeviceControlWidget::setDisenable(bool isEnable)
{
    ui.btn_absMove->setEnabled(isEnable);
    ui.btn_zero->setEnabled(isEnable);
}

void DeviceControlWidget::initCombox()
{
    ui.comboBox_step->addItem("0.001");
    ui.comboBox_step->addItem("0.01");
    ui.comboBox_step->addItem("0.1");
    ui.comboBox_step->addItem("0.5");
    ui.comboBox_step->addItem("1");
    ui.comboBox_step->addItem("5");

    ui.comboBox_step->setCurrentIndex(1);
}

void DeviceControlWidget::setEnabled(bool enabled)
{
    ui.btn_absMove->setEnabled(enabled);
    ui.btn_zero->setEnabled(enabled);
    ui.widget_motion->setEnabled(enabled);
    ui.btn_stop->setEnabled(true);
}

Result DeviceControlWidget::isHoming()
{
    return Motion2DModel::getInstance(type)->isHoming();
}

bool DeviceControlWidget::messageShow()
{

    QMessageBox msgBox;
    msgBox.setText("The DUT Control system havn't move to initial position!Do you want to doing the step?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes)
    {
        return true;
    }
    return false;
}
//void DeviceControlWidget::initTilt()
//{
//    ui.label_slow->setText("Speed(бу/s)");
//    ui.label_step->setText("Step(бу)");
//    ui.label_4->setText("X(бу): ");
//    ui.label_5->setText("Y(бу): ");
//}

void DeviceControlWidget::setDeviceMoveFinshed(bool finshed)
{
    emit sendDeviceMoveFinshed(finshed);
}

void DeviceControlWidget::on_isHoming_clicked()
{
    Result res= isHoming();
    if (!res.success)
    {
        ui.HomingStatus->setText("false"+QString::fromStdString(res.errorMsg));
        return;
    }
    ui.HomingStatus->setText("Already homing!");
}

Result DeviceControlWidget::on_btn_connect_clicked()
{
    QString ipEtc = ConfigItem::instance()->getMotion2DIpAndOther(type);
    Result result = Motion2DModel::getInstance(type)->connectAndInit(ipEtc);
    if (!result.success)
    {
        ui.label_status->setText("Disconnected."+ QString::fromStdString(result.errorMsg));
        return result;
    }    
    if (!type)
    {
        Result res = isHoming();
        ui.label_status->setText("Connected."+QString::fromStdString(res.errorMsg));
    } 
    else
    ui.label_status->setText("Connected.");
    return Result();
}

void DeviceControlWidget::on_btn_disconnect_clicked()
{
    Motion2DModel::getInstance(type)->disconnect();
    ui.label_status->setText("Not Connected.");
}

void DeviceControlWidget::update2d(float x, float y)
{
}
