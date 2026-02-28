#include "ReticleWidget.h"
#include "ReticleMode.h"
#include <QtConcurrent>
#include "PrjCommon/WaitWidget.h"
#include <QMessageBox>

ReticleWidget::ReticleWidget(QString toolboxName, QWidget *parent)
	: IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	init();
}

ReticleWidget::~ReticleWidget()
{
    delete ReticleMode::instance();
}

void ReticleWidget::on_btn_connect_clicked()
{
    setConnectEnabled(false);
    WaitWidget::instance()->startAnimation();
    QFuture<Result> future = QtConcurrent::run(ReticleMode::instance(), &ReticleMode::connect);
    watcherConnect.setFuture(future);
}

void ReticleWidget::on_btn_disconnect_clicked()
{
    ReticleMode::instance()->disconnect();
    connectStatus(false);
}

void ReticleWidget::on_comboBox_change_activated(const QString &text)
{
    setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(ReticleMode::instance(), &ReticleMode::changeReticle, text);
    watcher.setFuture(future);
}

void ReticleWidget::on_btn_absCoord_clicked()
{
    //TODO: add coordinate limit juage
    setEnabled(false);
    double coord = ui.spinBox_absCoord->text().toDouble();
    QFuture<Result> future = QtConcurrent::run(ReticleMode::instance(), &ReticleMode::absMove, coord);
    watcher.setFuture(future);
}

void ReticleWidget::on_btn_relCoord_clicked()
{
    // TODO: add coordinate limit juage
    setEnabled(false);
    double coord = ui.spinBox_relCoord->text().toDouble();
    QFuture<Result> future = QtConcurrent::run(ReticleMode::instance(), &ReticleMode::relativeMove, coord);
    watcher.setFuture(future);
}

void ReticleWidget::threadFinished()
{
    setEnabled(true);

    QFuture<Result> future = watcher.future();
    Result ret = future.result();
    if (!ret.success)
    {
        QMessageBox::critical(this, "Change reticle error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
    }
}

void ReticleWidget::threadFinishedConnect()
{
    WaitWidget::instance()->stopAnimation();

    setConnectEnabled(true);

    Result ret = watcherConnect.result();
    connectStatus(ret.success);
    if (!ret.success)
    {
        qCritical() << QString::fromStdString(ret.errorMsg);
    }
}

void ReticleWidget::updateCoordinate(float x)
{
    ui.label_coord->setText(QString("%1").arg(x));
}

void ReticleWidget::updateMotionStatus(MLMotionState status)
{
    if (status == MLMotionState::kMLStatusDisConnected || 
        status == MLMotionState::kMLStatusUnknow)
    {
        connectStatus(false);
    }else{
        connectStatus(true);
    }
}

void ReticleWidget::init()
{
    QObject::connect(ReticleMode::instance(), SIGNAL(updateMotion1DPositionSignal(float)), this, SLOT(updateCoordinate(float)));
    QObject::connect(ReticleMode::instance(), SIGNAL(updateMotion1DStatusSignal(MLMotionState)), this, SLOT(updateMotionStatus(MLMotionState)));

    connect(&watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(&watcherConnect, SIGNAL(finished()), this, SLOT(threadFinishedConnect()));

    QList<QString> list = ReticleMode::instance()->getReticles();
    ui.comboBox_change->clear();
    ui.comboBox_change->addItems(list);
}

void ReticleWidget::connectStatus(bool status)
{
    if (status)
    {
        ui.label_status->setText("Connected.");
    }
    else
    {
        ui.label_status->setText("Not connected.");
    }
}

void ReticleWidget::setEnabled(bool enabled)
{
    ui.comboBox_change->setEnabled(enabled);
    ui.btn_absCoord->setEnabled(enabled);
    ui.btn_relCoord->setEnabled(enabled);
}

void ReticleWidget::setConnectEnabled(bool enabled)
{
    ui.btn_connect->setEnabled(enabled);
    ui.btn_disconnect->setEnabled(enabled);
}
