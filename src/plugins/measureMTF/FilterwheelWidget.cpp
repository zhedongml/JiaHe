#include "MLColorimeterMode.h"
#include "FilterwheelWidget.h"
#include "pluginsystem/Services.h"
#include <QThread>
#include <QTimer>
#include "PrjCommon/service/McCommon.h"
#include <QtConcurrent/qtconcurrentrun.h>

FilterwheelWidget::FilterwheelWidget(QWidget *parent) 
    : QWidget(parent)
{
    ui.setupUi(this);

    stopFlag = false;

    m_isNDAlarm = false;
    m_isXYZAlarm = false;
    m_checkStateTimer = new QTimer(this);
    connect(m_checkStateTimer, &QTimer::timeout, this, &FilterwheelWidget::on_checkStateTimer_timeout);
    m_checkStateTimer->start(1000);

    ui.groupBox_6->hide();
    connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(connectStatus(bool)));
    //ui.cur_XYZbtn->setVisible(false);
    //ui.cur_NDbtn->setVisible(false);
    bool k =connect(&watcher1, SIGNAL(finished()), this, SLOT(handleFinished()));
    int u = 8;

    ui.ndcomboBox->clear();
    ui.ndcomboBox->addItem("ND0");
    ui.ndcomboBox->addItem("ND1");
    ui.ndcomboBox->addItem("ND2");
    ui.ndcomboBox->addItem("ND3");

    ui.xyzcomboBox->clear();
    ui.xyzcomboBox->addItem("X");
    ui.xyzcomboBox->addItem("Y");
    ui.xyzcomboBox->addItem("Z");
    ui.xyzcomboBox->addItem("Clear");
    ui.xyzcomboBox->addItem("Block");
    setMinimumSize(0, 0);
}

FilterwheelWidget::~FilterwheelWidget()
{}

void FilterwheelWidget::on_openPushButton_clicked()
{
    //MLResult result = m_Intrument->ConnectAll();
}

void FilterwheelWidget::on_cur_NDbtn_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        // TODO:test empty index is error
        QString curND = MLColorimeterMode::Instance()->GetNDFilter();
        ui.ndcomboBox->setCurrentText(curND);
    }

}
void FilterwheelWidget::on_cur_XYZbtn_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    { 
        //TODO:test empty index is error
        //ML::ColorFilter curXYZ = MLColorimeterMode::Instance()->getXYZFilter();
        //ui.xyzcomboBox->setCurrentIndex(curXYZ);
        ui.xyzcomboBox->setCurrentText(MLColorimeterMode::Instance()->GetXYZFilter());
    }
}
void FilterwheelWidget::on_xyzcomboBox_clicked()
{
    if (MLColorimeterMode::Instance()->IsConnect())
    {
        QString curColor = MLColorimeterMode::Instance()->GetXYZFilter();
        ui.xyzcomboBox->setCurrentText(curColor);
    }
}
void FilterwheelWidget::on_ndcomboBox_activated(int index)
{
    filterSetEnable(false);
    //MLColorimeterMode::Instance()->setNdFilter(ui.ndcomboBox->currentText().trimmed());
    QFuture<void> future = QtConcurrent::run(
        [=] { MLColorimeterMode::Instance()->MLColorimeterMode::SetNDFilter(ui.ndcomboBox->currentText().trimmed());
        });
    watcher1.setFuture(future);//watcher1
}

void FilterwheelWidget::on_xyzcomboBox_activated(int index)
{
    filterSetEnable(false);
    QFuture<void> future = QtConcurrent::run([=] { MLColorimeterMode::Instance()->SetColorFilter(ui.xyzcomboBox->currentText().trimmed()); });
    watcher1.setFuture(future); // watcher1
    //MLColorimeterMode::Instance()->setXxzFilter(ui.xyzcomboBox->currentText().trimmed());
}

void FilterwheelWidget::handleFinished()
{
    filterSetEnable(true);
}

void FilterwheelWidget::on_startMoveBtn_clicked()
{

    QThread *subThread = QThread::create([=]{

        stopFlag = false;

        while (!stopFlag)
        {

        }


        
    });

    subThread->start();
}

void FilterwheelWidget::on_stop_clicked()
{
    stopFlag = true;
}

void FilterwheelWidget::filterSetEnable(bool enable)
{
    ui.ndcomboBox->setEnabled(enable);
    ui.xyzcomboBox->setEnabled(enable);
    emit sendRefreshBtn(enable);
}

void FilterwheelWidget::on_checkStateTimer_timeout()
{
    //int alarmCode = m_Intrument->getAlarmCode();
    //if (alarmCode < 0)
    //{
    //    //ui.NDFilterAlarm->setStyleSheet("background-color: rgb(0, 0, 255);color : rgb(255, 255, 255);");
    //    //ui.XYZFilterAlarm->setStyleSheet("background-color: rgb(0, 0, 255);color : rgb(255, 255, 255);");
    //    //ui.CylinderAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
    //    //ui.DegreeAlarm->setStyleSheet("background-color: rgb(0, 0, 255);");
    //    //ui.XYZFilterAlarm->setStyleSheet("");
    //}
    //else
    //{
    //    m_isNDAlarm = alarmCode & 0x01;
    //    m_isXYZAlarm = alarmCode & 0x02;
    //    //m_isCylinderAlarm = alarmCode & 0x04;
    //    //m_isDegreeAlarm = alarmCode & 0x08;
    //    if (m_isNDAlarm & m_isXYZAlarm)
    //    {
    //        ui.FilterAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
    //        ui.FilterAlarm->setText("NDFilterAlarm and XYZFilterAlarm is alarming!");
    //    }
    //    else if (m_isNDAlarm)
    //    {
    //        ui.FilterAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
    //        ui.FilterAlarm->setText("NDFilterAlarm is alarming!");
    //    }
    //    else if (m_isXYZAlarm)
    //    {
    //        ui.FilterAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
    //        ui.FilterAlarm->setText("XYZFilterAlarm is alarming!");
    //    }
    //    else
    //    {
    //        ui.FilterAlarm->setStyleSheet("");
    //        ui.FilterAlarm->setText("Filter Alarm isn't alarming.");
    //    }

    //  /*  if (m_isCylinderAlarm)
    //    {
    //        ui.CylinderAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
    //    }
    //    else
    //    {
    //        ui.CylinderAlarm->setStyleSheet("background-color: rgb(0, 255, 0);");
    //    }

    //    if (m_isDegreeAlarm)
    //    {
    //        ui.DegreeAlarm->setStyleSheet("background-color: rgb(255, 0, 0);");
    //    }
    //    else
    //    {
    //        ui.DegreeAlarm->setStyleSheet("background-color: rgb(0, 255, 0);");
    //    }*/
    //}
}

void FilterwheelWidget::on_clearAlarm_clicked()
{
    //bool ret = m_Intrument->clearAlarm();
}

void FilterwheelWidget::connectStatus(bool connected)
{
    on_cur_NDbtn_clicked();
    on_xyzcomboBox_clicked();
}
