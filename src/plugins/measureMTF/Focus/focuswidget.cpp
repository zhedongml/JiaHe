#include "focuswidget.h"
#include "pluginsystem/Services.h"
#include <QMessageBox>
#include <QRegExpValidator>
//#include "ProgressWidget.h"
#include "AutoFocusModel.h"
#include <QtConcurrent>
#include "Core/loggingwrapper.h"
#include "Core/icore.h"
#include "PrjCommon/PrjCommon.h"
#include "MLDiopterScan.h"

#include <QtConcurrent>
#include "MeasureWidget.h"
#include "MLMonoBusinessManage.h"
#include "Core/imode.h"
#include "Core/modemanager.h"
#include "RecipeXMLEngine/RecipeXMLEngine.h"

using namespace ML_AF;

FocusWidget::FocusWidget(QString toolBoxName, QWidget* parent)
	: IToolBox(toolBoxName, parent)
{
	ui.setupUi(this);

	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->addButton(ui.rb_1DRelative, 0);
	m_buttonGroup->addButton(ui.rb_1DGlobal, 1);
	m_buttonGroup->addButton(ui.VID, 2);
	connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClicked(int)));
	m_currentButtonIndex = 0;
	ui.rb_1DRelative->setChecked(true);
	ui.RelativeText->setEnabled(true);
	ui.GlobalText->setEnabled(false);
	ui.VIDText->setEnabled(false);

	QRegExp exp("[0-9\\.-]+$");
	QValidator* Validator = new QRegExpValidator(exp);
	ui.RelativeText->setValidator(Validator);
	ui.GlobalText->setValidator(Validator);
	ui.VIDText->setValidator(Validator);

	connect(ui.stop_autofocus_btn, &QPushButton::clicked, this, &FocusWidget::onStopButtonClicked);

	//connect(AutoFocusModel::instance(), SIGNAL(updateAutoFocusStep(int)), this, SLOT(updateCurrentAutoFocusStep(int)));
	//connect(AutoFocusModel::instance(), SIGNAL(StopAutoFocusStep()), this, SLOT(updateStopAutoFocusStep()));

	connect(MLColorimeterMode::Instance(), SIGNAL(connectStatus(bool)), this, SLOT(connectStatus(bool)));
	connect(&m_watcher, &QFutureWatcher<Result>::finished, this, &FocusWidget::ThreadFinshed);

	QWidget* measureWidget = nullptr;
	connect(this, SIGNAL(liveImageSignal(bool)), (MeasureWidget*)measureWidget,
		SLOT(stopGrabSlot(bool)));

	Core::IMode* mode = Core::ModeManager::instance()->findMode("Recipe.RecipeXMLMode");
	connect(static_cast<RecipeXMLEngine::Internal::RecipeXMLMode*>(mode)->GetRecipe2Plugin(), &Recipe2::updateTreeSystemIsRunDone,
		this, [this](bool value) {
			m_treeSystemRunning = !value;
			if (!value)
			{
				if (m_processCallback)
				{
					//disconnect(
					//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::coarseScanProgress, this,
					//	&FocusWidget::onRoughFocusProgress);
					//disconnect(
					//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::fineScanProgress, this,
					//	&FocusWidget::onFineFocusProgress);
					disconnect(
						AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::coarseScanProgress, this,
						&FocusWidget::onRoughFocusProgress);
					disconnect(
						AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::fineScanProgress, this,
						&FocusWidget::onFineFocusProgress);
					m_processCallback = false;
				}
			}
			else
			{
				if (!m_processCallback)
				{
					//connect(
					//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::coarseScanProgress, this,
					//	&FocusWidget::onRoughFocusProgress,
					//	Qt::QueuedConnection);
					//connect(
					//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::fineScanProgress, this,
					//	&FocusWidget::onFineFocusProgress,
					//	Qt::QueuedConnection);
					connect(
						AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::coarseScanProgress, this,
						&FocusWidget::onRoughFocusProgress,
						Qt::QueuedConnection);
					connect(
						AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::fineScanProgress, this,
						&FocusWidget::onFineFocusProgress,
						Qt::QueuedConnection);
					m_processCallback = true;
				}
			}
		});

	//QString text = MLColorimeterMode::Instance()->getSyncBestFocus() ? "Stop Sync Best Focus" : "Start Sync Best Focus";
	//ui.btn_syncBestFocus->setText(text);
	ui.btn_syncBestFocus->hide();
	ui.refreshPos_btn->hide();
	//ui.groupBox_2->hide();
}

FocusWidget::~FocusWidget()
{
	m_motion = MLColorimeterMode::Instance()->GetCameraMotion();
	if (m_motion != nullptr)
	{
		m_motion->Unsubscribe(ML::Motion::MLMotionEvent::kPositionChanged, this);
	}
	delete AutoFocusModel::instance();
}

void FocusWidget::NotifyMotionPosition(double position)
{
	double curpos = position;
	ui.curcoord1d_lab->setText(QString::number(curpos));
	cv::Point3f value = MLColorimeterMode::Instance()->TransFocusMotionToSphVid(curpos);
	if (value.x == 0)
	{
		ui.curvidlab->setText("inf");
		ui.sphere_lab->setText("0");
	}
	else
	{
		ui.sphere_lab->setText(QString::number(value.x, 'f', 3));
		ui.curvidlab->setText(QString::number(value.y / 1000, 'f', 3));
	}
}

void FocusWidget::on_refreshPos_btn_clicked()
{
	Result ret = motionConnect();
	if (!ret.success)
	{
		QMessageBox::information(this, "Get position error",
			"Get position failed, " + QString::fromStdString(ret.errorMsg));
		return;
	}
	double position = MLColorimeterMode::Instance()->GetFocusMotionPos();
	ui.curcoord1d_lab->setText(QString::number(position));
	cv::Point3f value = MLColorimeterMode::Instance()->TransFocusMotionToSphVid(position);
	if (value.x == 0)
	{
		ui.curvidlab->setText("inf");
		ui.sphere_lab->setText("0D");
	}
	else
	{
		ui.sphere_lab->setText(QString::number(value.x / 1000, 'f', 3) + "D");
		ui.curvidlab->setText(QString::number(value.y / 1000, 'f', 3));
	}
}

void FocusWidget::on_disconnect1dmotion_clicked()
{
	if (m_motion != nullptr && m_motion->IsConnected())
	{
		m_motion->Disconnect();
	}
}

void FocusWidget::onButtonClicked(int index)
{
	m_currentButtonIndex = index;
	if (index == 0)
	{
		ui.rb_1DRelative->setChecked(true);
		ui.RelativeText->setEnabled(true);
		ui.GlobalText->setEnabled(false);
		ui.VIDText->setEnabled(false);
		ui.Addition->setEnabled(true);
		ui.subtraction->setEnabled(true);
	}
	else if (index == 1)
	{
		ui.rb_1DGlobal->setChecked(true);
		ui.RelativeText->setEnabled(false);
		ui.GlobalText->setEnabled(true);
		ui.VIDText->setEnabled(false);
		ui.Addition->setEnabled(false);
		ui.subtraction->setEnabled(false);
	}
	else
	{
		ui.VID->setChecked(true);
		ui.RelativeText->setEnabled(false);
		ui.GlobalText->setEnabled(false);
		ui.VIDText->setEnabled(true);
		ui.Addition->setEnabled(false);
		ui.subtraction->setEnabled(false);
	}
}

void FocusWidget::on_move_clicked()
{
	Result ret = motionConnect();
	if (!ret.success)
	{
		QMessageBox::information(this, "Move focus error",
			"Move focus failed, " + QString::fromStdString(ret.errorMsg));
		return;
	}
	ML::MLColorimeter::MotionConfig config = MLColorimeterMode::Instance()->GetCameraMotionConfig();
	if (m_currentButtonIndex == 0)
	{
		QString text = ui.RelativeText->text();
		double motionPosition = MLColorimeterMode::Instance()->GetFocusMotionPos();
		if ((motionPosition + text.toDouble()) < config.SoftwareLimitMax ||
			(motionPosition + text.toDouble()) > config.SoftwareLimitMin)
		{
			QMessageBox::information(this, "Tips!", "Out of motor travel range(" + QString::number(config.SoftwareLimitMin) + "-" + QString::number(config.SoftwareLimitMax) + ")!", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}

		ret = MLColorimeterMode::Instance()->SetFocusMotionPosAsync(text.toDouble());
		if (!ret.success) {
			QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
	}
	else if (m_currentButtonIndex == 1)
	{
		QString text = ui.GlobalText->text();
		if (text.toDouble() < config.SoftwareLimitMin || text.toDouble() > config.SoftwareLimitMax)
		{
			QMessageBox::information(this, "Tips!", "Out of motor travel range(" + QString::number(config.SoftwareLimitMin) + "-" + QString::number(config.SoftwareLimitMax) + ")!", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		ret = MLColorimeterMode::Instance()->SetFocusMotionPosAsync(text.toDouble());
		if (!ret.success)
		{
			QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
	}
	else
	{
		QString vid = ui.VIDText->text();
		//vid = "-" + vid;
		cv::Point3f value = MLColorimeterMode::Instance()->TransVidToFocusMotionSph(vid.toDouble());

		if (value.z < config.SoftwareLimitMin || value.z > config.SoftwareLimitMax)
		{
			QMessageBox::information(this, "Tips!", "Out of motor travel range(" + QString::number(config.SoftwareLimitMin) + "-" + QString::number(config.SoftwareLimitMax) + ")!", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		ret = MLColorimeterMode::Instance()->SetFocusMotionPosAsync(value.z);
		if (!ret.success)
		{
			QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
	}
}

void FocusWidget::on_stop_clicked()
{
	Result ret = motionConnect();
	if (!ret.success) {
		return;
	}
	m_motion->StopMove();
	onStopButtonClicked();
}

void FocusWidget::on_autoFocus_clicked()
{
	Result ret = motionConnect();
	if (!ret.success)
	{
		QMessageBox::information(this, "Auto focus error", "Auto focus failed, " + QString::fromStdString(ret.errorMsg));
		return;
	}
	emit liveImageSignal(false);
	ui.rough_progressBar->setValue(0);
	ui.fine_progressBar->setValue(0);
	QFuture<Result> future = QtConcurrent::run(this,&FocusWidget::ThroughFocusProcess);
	m_watcher.setFuture(future);
}

Result FocusWidget::ThroughFocusProcess()
{
	DiopterScanConfig config;
	std::string savepath = ui.savepath->text().toStdString();
	config.StoreResultImg = ui.saveImages->isChecked();
	config.IsUseFineAdjust = ui.fine_checkBox->isChecked();
	std::string prefix = ui.prefix->text().toStdString();
	return AutoFocusModel::instance()->DiopterScanProcess(config, savepath, prefix);
}

void FocusWidget::on_setpath_clicked() {
	ui.savepath->clear();
	QString savePath = QFileDialog::getExistingDirectory(nullptr, "choose folder", "",
		QFileDialog::ShowDirsOnly);
	if (savePath == "") {
		QMessageBox::warning(this, tr("Blank input warning"), "Please Set Save Path!");
		return;
	}
	else {
		ui.savepath->setText(savePath);
	}
}

void FocusWidget::on_Addition_clicked()
{
	if (m_motion != nullptr && m_motion->IsConnected())
	{
		QString text = ui.RelativeText->text();
		ML::MLColorimeter::MotionConfig config = MLColorimeterMode::Instance()->GetCameraMotionConfig();
		if ((MLColorimeterMode::Instance()->GetFocusMotionPos() + text.toDouble()) < config.SoftwareLimitMin || (MLColorimeterMode::Instance()->GetFocusMotionPos() + text.toDouble()) > config.SoftwareLimitMax)
		{
			QMessageBox::information(this, "Tips!", "Out of motor travel range(" + QString::number(config.SoftwareLimitMin) + "-" + QString::number(config.SoftwareLimitMax) + ")!", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		Result ret = MLColorimeterMode::Instance()->SetFocusMotionPosAsync(text.toDouble() + MLColorimeterMode::Instance()->GetFocusMotionPos());
		if (!ret.success)
		{
			QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
	}
}

void FocusWidget::on_subtraction_clicked()
{
	if (m_motion != nullptr && m_motion->IsConnected())
	{
		QString text = ui.RelativeText->text();
		ML::MLColorimeter::MotionConfig config = MLColorimeterMode::Instance()->GetCameraMotionConfig();
		if ((MLColorimeterMode::Instance()->GetFocusMotionPos() - text.toDouble()) < config.SoftwareLimitMin || (MLColorimeterMode::Instance()->GetFocusMotionPos() - text.toDouble()) > config.SoftwareLimitMax)
		{
			QMessageBox::information(this, "Tips!", "Out of motor travel range(" + QString::number(config.SoftwareLimitMin) + "-" + QString::number(config.SoftwareLimitMax) + ")!", QMessageBox::Yes, QMessageBox::Yes);
			return;
		}
		Result ret = MLColorimeterMode::Instance()->SetFocusMotionPosAsync(-text.toDouble() + MLColorimeterMode::Instance()->GetFocusMotionPos());
		if (!ret.success) {
			QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
				QMessageBox::NoButton);
			return;
		}
	}
}

void FocusWidget::connectStatus(bool connected)
{
	if (!connected)
	{
		m_motion = MLColorimeterMode::Instance()->GetCameraMotion();
		if (m_motion != nullptr)
		{
			m_motion->Unsubscribe(ML::Motion::MLMotionEvent::kPositionChanged, this);
		}

		ui.curvidlab->setText("");
		ui.curcoord1d_lab->setText("");
	}
	else
	{
		m_motion = MLColorimeterMode::Instance()->GetCameraMotion();
		if (m_motion != nullptr)
		{
			m_motion->Subscribe(ML::Motion::MLMotionEvent::kPositionChanged, this);
			if (!m_treeSystemRunning)
			{
				//connect(
				//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::coarseScanProgress, this,
				//	&FocusWidget::onRoughFocusProgress,
				//	Qt::QueuedConnection);
				//connect(
				//	MLColorimeterMode::Instance()->GetMonocular(), &MLMonoBusinessManage::fineScanProgress, this,
				//	&FocusWidget::onFineFocusProgress,
				//	Qt::QueuedConnection);
				connect(
					AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::coarseScanProgress, this,
					&FocusWidget::onRoughFocusProgress,
					Qt::QueuedConnection);
				connect(
					AutoFocusModel::instance()->GetDiopterScanManage(), &ML_AF::MLDiopterScan::fineScanProgress, this,
					&FocusWidget::onFineFocusProgress,
					Qt::QueuedConnection);
				m_processCallback = true;
			}
		}
	}
}

Result FocusWidget::motionConnect()
{
	if (!MLColorimeterMode::Instance()->IsConnect()) {
		return Result(false, "Colorimeter is not connected.");
	}
	return Result();
}

void FocusWidget::ThreadFinshed() {
	ui.autoFocus->setEnabled(true);
	Result ret = m_watcher.future().result();
	emit liveImageSignal(true);
	if (!ret.success) {
		QMessageBox::critical(this, tr(""), QString::fromStdString(ret.errorMsg));
		return;
	}

	std::vector<cv::Point3f> throughfocusRes;
	QStringList rowLabels;
	throughfocusRes = AutoFocusModel::instance()->GetDiopterScanResult();
	for (int i = 0; i < throughfocusRes.size(); i++)
	{
		rowLabels.append(QString("R%1").arg(i));
	}
	//cv::Point3f result_h = MLColorimeterMode::Instance()->GetMonocular()->ML_GetDiopterScanHResult();
	//if (result_h.x != 0.0)
	//{
	//	throughfocusRes.push_back(result_h);
	//	rowLabels.append("H");
	//}
	//cv::Point3f result_v = MLColorimeterMode::Instance()->GetMonocular()->ML_GetDiopterScanVResult();
	//if (result_v.x != 0.0)
	//{
	//	throughfocusRes.push_back(result_v);
	//	rowLabels.append("V");
	//}
	QStringList qlist{ "Best MTF", "Best VID", "Best Diopter" };
	ui.tableWidget->setHorizontalHeaderLabels(qlist);
	ui.tableWidget->setColumnCount(3);
	ui.tableWidget->setRowCount(throughfocusRes.size());
	ui.tableWidget->setVerticalHeaderLabels(rowLabels);
	for (size_t i = 0; i < throughfocusRes.size(); i++) {
		QTableWidgetItem* mtfItem =
			new QTableWidgetItem(QString::number(throughfocusRes[i].z));
		QTableWidgetItem* vidItem =
			new QTableWidgetItem(QString::number(throughfocusRes[i].y));
		QTableWidgetItem* motionItem =
			new QTableWidgetItem(QString::number(throughfocusRes[i].x));
		ui.tableWidget->setItem(i, 0, mtfItem);
		ui.tableWidget->setItem(i, 1, vidItem);
		ui.tableWidget->setItem(i, 2, motionItem);
	}
}

void FocusWidget::onStopButtonClicked()
{
	MLColorimeterMode::Instance()->GetMonocular()->StopThroughFocus();
}

void FocusWidget::onRoughFocusProgress(double progress) {
	ui.rough_progressBar->setValue(progress * 100);
}

void FocusWidget::onFineFocusProgress(double progress) {
	ui.fine_progressBar->setValue(progress * 100);
}