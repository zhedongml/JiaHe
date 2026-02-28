#include "CmWidget.h"
#include "pluginsystem/Services.h"
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <QtConcurrent>
#include <QMessageBox>
#include "CmConfig.h"

CmWidget::CmWidget(QString toolboxName, QWidget *parent) : IToolBox(toolboxName, parent)
{
    ui.setupUi(this);
    init();

    QObject::connect(&watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
}
CmWidget::~CmWidget()
{
}

void CmWidget::on_btn_astigma_clicked()
{
    setBtnEnabled(false);

    QString astigma = ui.comboBox_astigma->currentText();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::setAstigma, astigma);
    watcher.setFuture(future);
}

void CmWidget::on_comboBox_astigma_activated(const QString &text)
{
    on_btn_astigma_clicked();
}

void CmWidget::on_btn_degreeAbs_clicked()
{
    setBtnEnabled(false);

    int degree = ui.spinBox_degreeAbs->value();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::degreeAbs, degree);
    watcher.setFuture(future);
}

void CmWidget::on_btn_clearAlarm_clicked()
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        QMessageBox::warning(this, "Warning", "Measure camera is not connected.", QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }

    Result ret = MLColorimeterMode::Instance()->ClearAlarmFilter();
    if(!ret.success){
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }
}

void CmWidget::on_btn_stop_clicked()
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        QMessageBox::warning(this, "Warning", "Measure camera is not connected.", QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }

    Result ret = MLColorimeterMode::Instance()->GetMonocular()->ML_StopModuleMovement();
    if (!ret.success)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }
}

void CmWidget::on_btn_refresh_clicked()
{
    Result ret = refreshData();
    if (!ret.success)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }
}

void CmWidget::on_btn_degreeRelReduce_clicked()
{
    setBtnEnabled(false);

    int degree = ui.spinBox_degreeRel->value();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::degreeRel, -degree);
    watcher.setFuture(future);
}

void CmWidget::on_btn_degreeRelAdd_clicked()
{
    setBtnEnabled(false);

    int degree = ui.spinBox_degreeRel->value();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::degreeRel, degree);
    watcher.setFuture(future);
}

void CmWidget::on_btn_stepRelReduce_clicked()
{
    setBtnEnabled(false);

    int steps = ui.spinBox_stepRel->value();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::stepRel, -steps);
    watcher.setFuture(future);
}

void CmWidget::on_btn_stepRelAdd_clicked()
{
    setBtnEnabled(false);

    int steps = ui.spinBox_stepRel->value();
    QFuture<Result> future = QtConcurrent::run(this, &CmWidget::stepRel, steps);
    watcher.setFuture(future);
}

void CmWidget::threadFinished()
{
    setBtnEnabled(true);

    QFuture<Result> future = watcher.future();
    Result ret = future.result();
    if (!ret.success)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
    }
    refreshData();
}

void CmWidget::connectStatus(bool connected)
{
    if (connected)
    {
        refreshData();
    }
    else
    {
        ui.label_astigma->setText("");
        ui.label_degree->setText("");
    }
}

void CmWidget::init()
{
    //ui.widget->hide();

    ui.comboBox_astigma->clear();
    QList<QString> names = CmConfig::instance()->getCylinderNameList();
    ui.comboBox_astigma->addItems(names);

    //int cycleSteps;
    //Result ret = CmConfig::instance()->getPulsePerCycle(cycleSteps);
    //if(!ret.success){
    //    qCritical() << QString::fromStdString(ret.errorMsg);
    //}else{
    //    ui.label_tips->setText(ui.label_tips->text() + QString::number(cycleSteps));
    //}

    ui.spinBox_degreeAbs->setMinimum(0);
    ui.spinBox_degreeAbs->setMaximum(360);
    ui.spinBox_degreeRel->setMinimum(0);
    ui.spinBox_degreeRel->setMaximum(360);
    ui.spinBox_stepRel->setMinimum(1);
    ui.spinBox_stepRel->setMaximum(1000);

    ui.label->hide();
    ui.btn_stepRelReduce->hide();
    ui.spinBox_stepRel->hide(); 
    ui.btn_stepRelAdd->hide();
    ui.label_tips->hide();
    ui.label_degreeTitle_2->hide();
    ui.label_steps->hide();

    connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(connectStatus(bool)));
}

void CmWidget::setBtnEnabled(bool enabled)
{
    ui.btn_degreeAbs->setEnabled(enabled);
    ui.btn_degreeRelReduce->setEnabled(enabled);
    ui.btn_degreeRelAdd->setEnabled(enabled);
    ui.btn_stepRelReduce->setEnabled(enabled);
    ui.btn_stepRelAdd->setEnabled(enabled);

    ui.btn_clearAlarm->setEnabled(enabled);
    ui.btn_refresh->setEnabled(enabled);

    ui.comboBox_astigma->setEnabled(enabled);
}

Result CmWidget::setAstigma(QString astigma)
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        return Result(false, "Measure camera is not connected.");
    }

    Result ret = MLColorimeterMode::Instance()->GetMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), 0);
    return ret;
}

Result CmWidget::degreeAbs(int degree)
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        return Result(false, "Measure camera is not connected.");
    }

    QString astigma;
    int degreeCurrent;
    Result ret /*= MLColorimeterMode::Instance()->getCylinder(astigma, degreeCurrent)*/;
    if (!ret.success)
    {
        return ret;
    }

    ret = MLColorimeterMode::Instance()->GetMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), degree);
    return ret;
}

Result CmWidget::degreeRel(int degree)
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        return Result(false, "Measure camera is not connected.");
    }

    QString astigma;
    int degreeCurrent;
    Result ret /*= MLColorimeterMode::Instance()->GetCylinder(astigma, degreeCurrent)*/;
    if (!ret.success)
    {
        return ret;
    }

    ret = MLColorimeterMode::Instance()->GetMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(),
                                                                                 degreeCurrent + degree);
    return ret;
}

Result CmWidget::stepRel(int steps)
{
    //TODO: to be done
    return Result();

    QString astigma;
    int degree;
    Result ret /*= MLColorimeterMode::Instance()->getCylinder(astigma, degree)*/;
    if(!ret.success){
        return ret;
    }

    //steps switch degree
    ret = MLColorimeterMode::Instance()->GetMonocular()->ML_MoveRXFilterByNameSync(astigma.toStdString(), degree);
    return ret;
}

Result CmWidget::refreshData()
{
    if (!MLColorimeterMode::Instance()->IsConnect())
    {
        return Result(false, "Measure camera is not connected.");
    }

    QString astigma;
    int degree;
    Result ret/* = MLColorimeterMode::Instance()->GetCylinder(astigma, degree)*/;
    if (!ret.success)
    {
        return ret;
    }

    ui.label_astigma->setText(astigma);
    ui.label_degree->setText(QString::number(degree));

    //TODO: to be done
    //ui.label_steps->setText(QString::number(steps));
    return Result();
}
