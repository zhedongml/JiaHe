#include "DistanceSensorWidget.h"
#include <plugins/PrjCommon/service/ml_motion.h>
#include "piMotionActor/ConfigItem.h"
#include "piMotionActor/calibrationconfig.h"
#include "DistanceCheck.h"
#include <QMessageBox>
#include "PLCControl/PLCController.h"

DistanceSensorWidget::DistanceSensorWidget(QWidget *parent):
    QWidget(parent)
{
    ui.setupUi(this);

    ui.status->setText("Not connected.");
    MLKeyenceCL::MakeRangeFinder(m_type)->subscribe(RFMotionEvent::kStateChanged, this);
    MLKeyenceCL::MakeRangeFinder(m_type)->subscribe(RFMotionEvent::kPositionChanged, this);

    connect(this, SIGNAL(stateChangedSignal(bool, bool)), this, SLOT(stateChangedSlot(bool, bool)));
    connect(this, SIGNAL(positionSignal(double)), this, SLOT(positionSlot(double)));

    //ui.btn_HPSCF->hide();
}

DistanceSensorWidget::~DistanceSensorWidget()
{
    //delete MLKeyenceCL::MakeRangeFinder();
}

void DistanceSensorWidget::setType(MLKeyenceCLType type)
{
    m_type = type;

    if(KEYENCE_TYPE_DOWN == type)
    {
        ui.widget_up->hide();
    }else{
        DistanceCheckInfo info = DistanceCheck::instance().getDistanceCheckInfo();
        ui.label_targetDis->setText(QString::number(info.targetDistance, 'f', 3));
        ui.label_allowOffset->setText("+/-" + QString::number(info.offset, 'f', 3));
        setCheckLabel(0);
    }
}

void DistanceSensorWidget::setTitle(const QString& title)
{
    ui.groupBox->setTitle(title);
}

void DistanceSensorWidget::on_btn_connect_clicked()
{
    KeyenceDviceInfo info = ConfigItem::instance()->getKeyenceType();
    bool ret = MLKeyenceCL::MakeRangeFinder(m_type)->Connect(info.type.toStdString().c_str(), info.id);
    if (!ret)
    {
        ui.status->setText("Connect failed.");
    }
    else
    {
        PLCController::instance()->keyenceLight(true);
        ui.status->setText("Connected.");
    }
}

void DistanceSensorWidget::on_btn_disconnect_clicked()
{
    bool res = MLKeyenceCL::MakeRangeFinder(m_type)->Disconnect();
    if (!res)
    {
        ui.status->setText("Disconnect failed.");
	}
	else
	{
		PLCController::instance()->keyenceLight(false);
		ui.status->setText("Not Connected.");
	}
}

void DistanceSensorWidget::stateChangedSlot(bool old_state, bool new_state)
{
    if (old_state != new_state)
    {
        if (new_state)
        {
            ui.status->setText("Connection success.");
        }
        else
        {
            ui.status->setText("Disconnected.");
            ui.lineEdit_distance->setText("");
            setCheckLabel(0);
        }
    }
}

void DistanceSensorWidget::positionSlot(double position)
{
    KeyenceInfo info = ConfigItem::instance()->getKeyenceInfo();
    if (position > info.keyenceRangeMax || position < info.keyenceRangeMin)
    {
        ui.lineEdit_distance->setText("----");
        return;
    }

    if (position > 100000 || position < -100000)
    {
        ui.lineEdit_distance->setText("----");
        setCheckLabel(0);
    }else{
        float distance = info.keyenceZeroPos - position;
        ui.lineEdit_distance->setText(QString::number(distance, 'f', 5));
        return;

        DistanceCheckInfo info = DistanceCheck::instance().getDistanceCheckInfo();
        if (KEYENCE_TYPE_UP == m_type) {
            if(abs(info.targetDistance - distance) > abs(info.offset)){
                setCheckLabel(-1, QString("DUT distance offset is too large, offset is %1.").arg(QString::number(info.offset, 'f', 3)));
            }else{
                setCheckLabel(1);
            }
        }
    }
}

void DistanceSensorWidget::on_btn_update_clicked()
{
    if(KEYENCE_TYPE_UP == m_type){
        double distance;
        Result ret = DistanceManage().getKeyenceToDutDistance(distance, KEYENCE_TYPE_UP);
        if (!ret.success) {
            QMessageBox::critical(this, "Update error", QString::fromStdString(ret.errorMsg));
            return;
        }

        ret = DistanceCheck::instance().updateTargetDistance(distance);
        if (!ret.success) {
            QMessageBox::critical(this, "Update error", QString::fromStdString(ret.errorMsg));
            return;
        }

        DistanceCheckInfo info = DistanceCheck::instance().getDistanceCheckInfo();
        ui.label_targetDis->setText(QString::number(info.targetDistance, 'f', 3));
    }
}

void DistanceSensorWidget::NotifyMotionStateChanged(bool old_state, bool new_state)
{
    emit stateChangedSignal(old_state, new_state);
}

void DistanceSensorWidget::NotifyMotionPosition(double position)
{
    emit positionSignal(position);
}

void DistanceSensorWidget::setCheckLabel(int status, const QString& text)
{
    if(status == -1){
        ui.label_offsetStatus->setText(text);
        ui.label_offsetStatus->setStyleSheet("color: #FF4C4C;");
    }
    else if(status == 1){
        ui.label_offsetStatus->setText("DUT distance is within a rang.");
        ui.label_offsetStatus->setStyleSheet("color: #32CD32  ;");
    }
    else{
        ui.label_offsetStatus->setText("DUT distance is unknown.");
        ui.label_offsetStatus->setStyleSheet("color:rgb(0,0,0);");
    }
}
