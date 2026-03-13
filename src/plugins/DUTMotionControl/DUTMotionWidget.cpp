#include "DUTMotionWidget.h"
#include "WaitWidget.h"
#include "OrientalMotorControl.h"
#include  "loggingwrapper.h"
#include <QtConcurrent>
#include <QMessageBox>
#include <QButtonGroup>
#include <QRegExpValidator>

DUTMotionWidget::DUTMotionWidget(QString toolBoxName, QWidget *parent) : IToolBox(toolBoxName, parent)
{
    ui.setupUi(this);
    init();
    ui.groupBox_5->hide();
    ui.groupBox_7->hide();
}

DUTMotionWidget::~DUTMotionWidget()
{
}

void DUTMotionWidget::init()
{
    ui.label_status->setText("Not connected.");
    connect(&watcher, &QFutureWatcher<Result>::finished, this, &DUTMotionWidget::handleFinished);
    connect(&watcher_, &QFutureWatcher<Result>::finished, this, &DUTMotionWidget::moveFinished);

    ui.rotationAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
    ui.tiltAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
    ui.tipAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->addButton(ui.rel_btn, 0);
    m_buttonGroup->addButton(ui.abs_btn, 1);
    connect(m_buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(RadioButtonSwitch(QAbstractButton*)));
    m_buttonGroup->button(0)->click();

    ui.x_line->setPlaceholderText("input degree");
    ui.y_line->setPlaceholderText("input degree");
    ui.z_line->setPlaceholderText("input degree");

    //m_checkStateTimer = new QTimer(this);
    //connect(m_checkStateTimer, &QTimer::timeout, this, &DUTMotionWidget::on_checkStateTimer_timeout);
    //m_checkStateTimer->start(2000);
}

void DUTMotionWidget::on_btn_connect_clicked()
{
    /*if (OrientalMotorControl::getInstance()->IsConnected())
    {
        ui.label_status->setText("Connected.");
        return;
    }*/
    setEnabled(false);
    //WaitWidget::instance()->startAnimation();
    //ui.label_status->setText(" Waiting for connection......");
    QFuture<Result> future = QtConcurrent::run([=]() { return OrientalMotorControl::getInstance()->Connect(); });
    watcher.setFuture(future);
}

void DUTMotionWidget::on_btn_disconnect_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        return;
    }
    Result ret = OrientalMotorControl::getInstance()->Disconnect();
    if (ret.success)
    {
        ui.label_status->setText("Disconnect Success.");
    }
}

void DUTMotionWidget::on_z_home_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    QFuture<Result> future = QtConcurrent::run([=]() {
        return OrientalMotorControl::getInstance()->HomingSync(OrientalAxle::DZ);
    });
    watcher_.setFuture(future);
}

void DUTMotionWidget::on_y_home_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    QFuture<Result> future = QtConcurrent::run([=]() {
        return OrientalMotorControl::getInstance()->HomingSync(OrientalAxle::DY);
        });
    watcher_.setFuture(future);
}

void DUTMotionWidget::on_x_home_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    QFuture<Result> future = QtConcurrent::run([=]() {
        return OrientalMotorControl::getInstance()->HomingSync(OrientalAxle::DX);
        });
    watcher_.setFuture(future);
}

void DUTMotionWidget::on_z_refresh_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    double degree = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DZ);
    ui.z_pos->setText(QString::number(degree, 'f', 4));
}

void DUTMotionWidget::on_y_refresh_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    double degree = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DY);
    ui.y_pos->setText(QString::number(degree, 'f', 4));
}

void DUTMotionWidget::on_x_refresh_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        LoggingWrapper::instance()->warn("module is not connected");
        return;
    }
    double degree = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DX);
    ui.x_pos->setText(QString::number(degree, 'f', 4));
}

void DUTMotionWidget::on_z_stop_clicked()
{
    Result ret = OrientalMotorControl::getInstance()->StopMove(OrientalAxle::DZ);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Stop DZ Moving Failed"), QString::fromStdString(ret.errorMsg));
        return;
    }
    on_z_refresh_clicked();
}

void DUTMotionWidget::on_y_stop_clicked()
{
    Result ret = OrientalMotorControl::getInstance()->StopMove(OrientalAxle::DY);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Stop DY Moving Failed"), QString::fromStdString(ret.errorMsg));
        return;
    }
    on_y_refresh_clicked();
}

void DUTMotionWidget::on_x_stop_clicked()
{
    Result ret = OrientalMotorControl::getInstance()->StopMove(OrientalAxle::DX);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Stop DX Moving Failed"), QString::fromStdString(ret.errorMsg));
        return;
    }
    on_x_refresh_clicked();
}

void DUTMotionWidget::handleFinished()
{
    setEnabled(true);
    if(watcher.result().success)
    {
        ui.label_status->setText("Connected.");
        on_z_refresh_clicked();
        on_x_refresh_clicked();
        on_y_refresh_clicked();
    }
    else
    {
        ui.label_status->setText("Connected Failed.");
    }
}

void DUTMotionWidget::moveFinished()
{
    if (!watcher_.result().success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(watcher_.result().errorMsg));
    }
}

void DUTMotionWidget::on_z_move_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), "Motor is not connected.");
        return;
    }
    float degree = ui.z_line->text().toFloat();
    //QFuture<Result> future = QtConcurrent::run([=]() {
    //        return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DZ, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DZ, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Z_refresh_clicked();
}

void DUTMotionWidget::on_x_move_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), "Motor is not connected.");
        return;
    }
    float degree = ui.x_line->text().toFloat();
    //QFuture<Result> future = QtConcurrent::run([=]() {
    //        return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DX, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DX, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_X_refresh_clicked();
}

void DUTMotionWidget::on_y_move_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), "Motor is not connected.");
        return;
    }
    float degree = ui.y_line->text().toFloat();
    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DY, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DY, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Y_refresh_clicked();
}

void DUTMotionWidget::on_checkStateTimer_timeout()
{
    return;
    //if (!OrientalMotorControl::getInstance()->IsConnected())
    //{
    //    return;
    //}
    int alarmCodeX= OrientalMotorControl::getInstance()->IsAlarm(OrientalAxle::DX);
    int alarmCodeY = OrientalMotorControl::getInstance()->IsAlarm(OrientalAxle::DY);
    int alarmCodeZ = OrientalMotorControl::getInstance()->IsAlarm(OrientalAxle::DZ);

    if (alarmCodeX < 0|| alarmCodeY < 0|| alarmCodeZ < 0)
    {
        ui.rotationAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
        ui.tiltAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
        ui.tipAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
    }
    else
    {
        //m_isRotationAlarm = alarmCodeX;
        //m_isTiltAlarm = alarmCodeY;
        //m_isTipAlarm = alarmCodeZ;
        if (alarmCodeX)
        {
            ui.rotationAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else
        {
            ui.rotationAlarm->setStyleSheet("background-color: rgb(0, 255, 0);");
        }

        if (alarmCodeY)
        {
            ui.tiltAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else
        {
            ui.tiltAlarm->setStyleSheet("background-color: rgb(0, 255, 0);");
        }

        if (alarmCodeZ)
        {
            ui.tipAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
        }
        else
        {
            ui.tipAlarm->setStyleSheet("background-color: rgb(0, 255, 0);");
        }
    }

     //update pos status
     //onUpdateFilterStatus();
}

void DUTMotionWidget::on_clearAlarm_clicked()
{
    if (!OrientalMotorControl::getInstance()->IsConnected())
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), "Motor is not connected.");
        return;
    }
    Result ret = OrientalMotorControl::getInstance()->ClearAlarm();
    if (!ret.success)
        QMessageBox::warning(this, tr("Error"), "Clear Alarm Failed");
}

void DUTMotionWidget::setEnabled(bool enabled)
{
    ui.btn_connect->setEnabled(enabled);
    ui.btn_disconnect->setEnabled(enabled);
}

void DUTMotionWidget::RadioButtonSwitch(QAbstractButton* button)
{
    int id = m_buttonGroup->id(button);
    if (id == 0)
    {
        //ui.groupBox_4->setTitle("relative");
        m_buttonGroup->button(0)->setChecked(true);
        ui.x_sub->setHidden(false);
        ui.y_sub->setHidden(false);
        ui.z_sub->setHidden(false);
        ui.x_add->setHidden(false);
        ui.y_add->setHidden(false);
        ui.z_add->setHidden(false);
        ui.x_move->setHidden(true);
        ui.y_move->setHidden(true);
        ui.z_move->setHidden(true);
    }
    else
    {
        //ui.groupBox_4->setTitle("absolute");
        ui.x_sub->setHidden(true);
        ui.y_sub->setHidden(true);
        ui.z_sub->setHidden(true);
        ui.x_add->setHidden(true);
        ui.y_add->setHidden(true);
        ui.z_add->setHidden(true);
        ui.x_move->setHidden(false);
        ui.y_move->setHidden(false);
        ui.z_move->setHidden(false);
    }
}

void DUTMotionWidget::on_x_sub_clicked()
{
    float degree = ui.x_pos->text().toFloat() - ui.x_line->text().toFloat();

    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DX, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DX, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_X_refresh_clicked();
}

void DUTMotionWidget::on_y_sub_clicked()
{
    float degree = ui.y_pos->text().toFloat() - ui.y_line->text().toFloat();

    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DY, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DY, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Y_refresh_clicked();
}

void DUTMotionWidget::on_z_sub_clicked()
{
    float degree = ui.z_pos->text().toFloat() - ui.z_line->text().toFloat();

    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DZ, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DZ, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Z_refresh_clicked();
}

void DUTMotionWidget::on_x_add_clicked()
{
    float degree = ui.x_pos->text().toFloat() + ui.x_line->text().toFloat();

    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    return OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DX, degree);
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DX, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_X_refresh_clicked();
}

void DUTMotionWidget::on_y_add_clicked()
{
    float degree = ui.y_pos->text().toFloat() + ui.y_line->text().toFloat();

    //QFuture<Result> future = QtConcurrent::run([=]() {
    //    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DY, degree);
    //    on_Y_refresh_clicked();
    //    return ret;
    //    });
    //watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DY, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Y_refresh_clicked();
}

void DUTMotionWidget::on_z_add_clicked()
{
    float degree = ui.z_pos->text().toFloat() + ui.z_line->text().toFloat();

	//QFuture<Result> future = QtConcurrent::run([=]() {
	//	Result ret = OrientalMotorControl::getInstance()->MoveByDegreeSync(OrientalAxle::DZ, degree);
	//	on_Z_refresh_clicked();
	//	return ret;
	//	});
	//watcher_.setFuture(future);
    Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DZ, degree);
    if (!ret.success)
    {
        QMessageBox::warning(this, tr("Dut Motion Moving Error"), QString::fromStdString(ret.errorMsg));
        return;
    }
    //on_Z_refresh_clicked();
}