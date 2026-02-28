#include "ReticleWidget2D.h"
#include "ReticleMode2D.h"
#include <QtConcurrent>
#include "PrjCommon/WaitWidget.h"
#include <QMessageBox>
#include "Core/icore.h"
#include "piMotionActor/calibrationconfig.h"
#include "PrjCommon/PrjCommon.h"

ReticleWidget2D::ReticleWidget2D(QString toolboxName, QWidget *parent)
	: IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	init();
}

ReticleWidget2D::~ReticleWidget2D()
{
    delete ReticleMode2D::instance();
}

void ReticleWidget2D::on_btn_connect_clicked()
{
    setConnectEnabled(false);
    WaitWidget::instance()->startAnimation();
    QFuture<Result> future = QtConcurrent::run(this, &ReticleWidget2D::deviceConnect);
    watcherConnect.setFuture(future);
}

void ReticleWidget2D::on_btn_disconnect_clicked()
{
    ReticleMode2D::instance()->disconnect();
    connectStatus(false);
}

void ReticleWidget2D::on_comboBox_change_activated(const QString &text)
{
    setEnabled(false);
    int type = ui.comboBox_type->currentIndex();
    QFuture<Result> future = QtConcurrent::run(ReticleMode2D::instance(), &ReticleMode2D::changeReticle, text, type);
    watcher.setFuture(future);
}

void ReticleWidget2D::on_comboBox_eyeType_activated(int index)
{
    setEnabled(false);
    int type = index;
    QString reticle = ui.comboBox_change->currentText();
    QFuture<Result> future = QtConcurrent::run(ReticleMode2D::instance(), &ReticleMode2D::changeReticle, reticle, type);
    watcher.setFuture(future);
}

void ReticleWidget2D::on_btn_absCoord_clicked()
{
    //TODO: add coordinate limit juage
    setEnabled(false);
    float x = ui.spinBox_absCoordX->text().toDouble();
    float y = ui.spinBox_absCoordY->text().toDouble();
    QFuture<Result> future = QtConcurrent::run(ReticleMode2D::instance(), &ReticleMode2D::absMove, cv::Point2f(x,y));
    watcher.setFuture(future);
}

void ReticleWidget2D::on_btn_relCoord_clicked()
{
    // TODO: add coordinate limit juage
    setEnabled(false);
    float x = ui.spinBox_relCoordX->text().toDouble();
    float y = ui.spinBox_relCoordY->text().toDouble();
    QFuture<Result> future = QtConcurrent::run(ReticleMode2D::instance(), &ReticleMode2D::relativeMove, cv::Point2f(x,y));
    watcher.setFuture(future);
}

void ReticleWidget2D::on_btn_home_clicked()
{
    setEnabled(false);
    QFuture<Result> future = QtConcurrent::run(ReticleMode2D::instance(), &ReticleMode2D::homingForced);
    watcher.setFuture(future);
}

void ReticleWidget2D::on_btn_stop_clicked()
{
    ReticleMode2D::instance()->moveStop();
}

void ReticleWidget2D::threadFinished()
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

void ReticleWidget2D::threadFinishedConnect()
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

void ReticleWidget2D::updateCoordinate(float x, float y)
{
    ui.label_coord->setText(QString("%1, %2").arg(QString::number(x, 'f', 4)).arg(QString::number(y, 'f', 4)));
}

void ReticleWidget2D::updateMotionStatus(MLMotionState status)
{
    static bool preConnected = false;
    bool current = !(status == MLMotionState::kMLStatusDisConnected || status == MLMotionState::kMLStatusUnknow);

    if (preConnected != current)
    {
        connectStatus(current);
    }
    preConnected = current;
}

void ReticleWidget2D::updateDutConfigSlot()
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    ui.comboBox_type->setCurrentIndex(dutConfig.eyeType);
    if (ReticleMode2D::instance()->isConnected()){
        ReticleMode2D::instance()->moveUpDown(dutConfig.eyeType);
    }
}

void ReticleWidget2D::init()
{
    qRegisterMetaType<MLStatus>("MLMotionState");

    QObject::connect(ReticleMode2D::instance(), SIGNAL(updateMotion2DPositionSignal(float, float)), this,
                     SLOT(updateCoordinate(float, float)));
    QObject::connect(ReticleMode2D::instance(), SIGNAL(updateMotion2DStatusSignal(MLMotionState)), this,
                     SLOT(updateMotionStatus(MLMotionState)));

    connect(&watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(&watcherConnect, SIGNAL(finished()), this, SLOT(threadFinishedConnect()));
    
    if (ReticleMode2D::instance() ->isDutSync())
    {
        connect(Core::PrjCommon::instance(), SIGNAL(updateDutConfigSignal()), this, SLOT(updateDutConfigSlot()));
    }

    QList<QString> list = ReticleMode2D::instance()->getReticles();
    ui.comboBox_change->clear();
    ui.comboBox_change->addItems(list);

    ui.label_coord->setText("");
}

void ReticleWidget2D::connectStatus(bool status)
{
    if (status)
    {
        ui.label_status->setText("Connected.");
        //ui.comboBox_dutType->setCurrentIndex(CalibrationConfig::instance()->getDutConfig().eyeType);
    }
    else
    {
        ui.label_status->setText("Not connected.");
    }
}

void ReticleWidget2D::setEnabled(bool enabled)
{
    ui.comboBox_change->setEnabled(enabled);
    ui.btn_absCoord->setEnabled(enabled);
    ui.btn_relCoord->setEnabled(enabled);
    ui.btn_home->setEnabled(enabled);
}

void ReticleWidget2D::setConnectEnabled(bool enabled)
{
    ui.btn_connect->setEnabled(enabled);
    ui.btn_disconnect->setEnabled(enabled);
}

Result ReticleWidget2D::deviceConnect()
{
    Result ret = ReticleMode2D::instance()->connect();
    if (!ret.success)
    {
        return ret;
    }

    return ret;
    //return ReticleMode2D::instance()->homingForced();
}
