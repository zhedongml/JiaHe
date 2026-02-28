#include "AlignWidget.h"
#include <QHeaderView>
#include <QtConcurrent>
#include "AlignUtils.h"
#include <QFuture>
#include <QMessageBox>
#include <QThread>
#include <QFileDialog>
#include "AnalyzeRecipe.h"
#include "Core/loggingwrapper.h"
#include "CameraModel.h"

AlignWidget::AlignWidget(QWidget* parent) :
	QWidget(parent)
{
	ui.setupUi(this);

	init();
}

AlignWidget::~AlignWidget()
{
}

void AlignWidget::init()
{
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->horizontalHeader()->setFixedHeight(35);
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{"
		"border-top:0px solid #E5E5E5;"
		"border-left:0px solid #E5E5E5;"
		"border-right:0px solid #E5E5E5;"
		"border-bottom: 0.5px solid #E5E5E5;"
		"background:rgb(192,192,192);"
		"padding:4px;"
		"color: black;"
		"}");
	ui.tableWidget->verticalHeader()->setVisible(false);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
	ui.groupBox->setMaximumHeight(150);
	ui.groupBox->setHidden(true);
	ui.widget->setHidden(true);
	ui.btn_confirmFiducial->setEnabled(false);
	ui.plainText_loadFile->setEnabled(true);

	connect(&watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
	// connect(Alignment::instance(), SIGNAL(messageBoxSignal(QString)), this, SLOT(messageBoxSlot(QString)));
	 //devicesConnect
}

void AlignWidget::updateState(State state)
{
	switch (state)
	{
	case STOP:
		ui.btn_start->setText(BTN_START_TEXT);
		clearTableFiducial();
		break;
	case RUNNING:
		ui.btn_start->setText(BTN_STOP_TEXT);
		break;
	default:
		break;
	}
}

Result AlignWidget::start()
{
	//if (!Alignment::instance()->isStop())
	//{
	//    return Result(false, "Recipe is running, please stop and start again.");
	//}
	emit deleteFiducialSignal();

	Result result = analyzeFile();
	if (!result.success)
	{
		return result;
	}

	int eyeType = CalibrationConfig::instance()->getDutConfig().eyeType;
	AlignBaseInfo alignInfo = AlignUtils::getAlignBaseInfo(eyeType);
	EyeReliefBaseInfo eyeReliefInfo = AlignUtils::getEyeReliefBaseInfo(eyeType);
	AlignConfig alignConfig = CalibrationConfig::instance()->getAlignConfig();

	//result = Alignment::instance()->judgeDataValidity(alignInfo, eyeReliefInfo, alignConfig);
	if (!result.success)
	{
		updateState(STOP);
		return result;
	}

	ui.plainText_loadFile->setEnabled(false);
	QFuture<Result> future = QtConcurrent::run(this, &AlignWidget::startThread, alignInfo, eyeReliefInfo, alignConfig.autoMode);
	watcher.setFuture(future);
	return Result();
}

Result AlignWidget::startThread(const AlignBaseInfo& alignBaseInfo, const EyeReliefBaseInfo& eyeReliefInfo,
	AlignConfig alignConfig)
{
	// Alignment::instance()->continueRun(true);

	std::vector<cv::Point3f> acsCoords;
	Result result;// = Alignment::instance()->alignEyeboxAcsXYZ(acsCoords, alignBaseInfo, eyeReliefInfo, alignConfig);
	if (!result.success)
	{
		LoggingWrapper::instance()->error(QString::fromStdString(result.errorMsg));
		return result;
	}

	for (cv::Point3f acsCoord : acsCoords)
	{
		// Alignment::instance()->i29AlignEyebox(acsCoord, alignBaseInfo.acsCoordLimit);
		QThread::sleep(1000);
	}
	return Result();
}

void AlignWidget::stop()
{
	//Alignment::instance()->continueRun(false);
}

void AlignWidget::confirmFiducial()
{
	cv::Point2f fiducialPixel;
	Result result = getTableFiducial(fiducialPixel);
	if (!result.success)
	{
		QMessageBox::critical(this, "Confirm fiducial error", QString::fromStdString(result.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	clearTableFiducial();

	//Alignment::instance()->continueRun(true, fiducialPixel);
	emit deleteFiducialSignal();
}

void AlignWidget::addTableFiducial(const cv::Point2f& fiducialPixel)
{
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);
	QTableWidgetItem* xItem = new QTableWidgetItem(QString::number(fiducialPixel.x));
	ui.tableWidget->setItem(row, 0, xItem);
	QTableWidgetItem* yItem = new QTableWidgetItem(QString::number(fiducialPixel.y));
	ui.tableWidget->setItem(row, 1, yItem);

	xItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	yItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	ui.btn_confirmFiducial->setEnabled(true);
}

void AlignWidget::updateTableFiducial(const cv::Point2f& fiducialPixel)
{
	if (ui.tableWidget->rowCount() == 0) {
		addTableFiducial(fiducialPixel);
	}
	else {
		QTableWidgetItem* xItem = ui.tableWidget->item(0, 0);
		xItem->setText(QString("%1").arg(fiducialPixel.x));
		QTableWidgetItem* yItem = ui.tableWidget->item(0, 1);
		yItem->setText(QString("%1").arg(fiducialPixel.y));
	}
}

Result AlignWidget::getTableFiducial(cv::Point2f& fiducialPixel)
{
	if (ui.tableWidget->rowCount() == 0)
	{
		return Result(false, "Manual calibrated fiducial is null.");
	}

	fiducialPixel.x = ui.tableWidget->item(0, 0)->text().trimmed().toDouble();
	fiducialPixel.y = ui.tableWidget->item(0, 1)->text().trimmed().toDouble();
	return Result();
}

bool AlignWidget::isAlignWidgetRun()
{
	return ui.btn_start->text() == BTN_STOP_TEXT;
}

void AlignWidget::clearTableFiducial()
{
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);

	ui.btn_confirmFiducial->setEnabled(false);
}

Result AlignWidget::analyzeFile()
{
	QString text = ui.plainText_loadFile->toPlainText().trimmed();
	if (text.isEmpty()) {
		return Result(false, "File is null, please load file.");
	}

	QList<QString> lines = text.split("\n");
	Result ret = AnalyzeRecipe().analyzeLines(lines, CalibrationConfig::instance()->getDutConfigReference(),
		CalibrationConfig::instance()->getAlignConfigReference());
	return ret;
}

void AlignWidget::binningOneToTwo()
{
	QStringList list;
	addSubFolderImages("D:/FFC1", list);

	for (QString file : list) {
		imageBinningSwitch(file, 2);
	}
}

void AlignWidget::imageBinningSwitch(QString file, int bin)
{
	cv::Mat src = cv::imread(file.toStdString(), -1);
	cv::Mat mat = binningSwitch(src, bin);
	cv::imwrite(file.toStdString(), mat);
}

void AlignWidget::addSubFolderImages(QString path, QStringList& string_list)
{
	QDir dir(path);
	if (!dir.exists())
	{
		return;
	}
	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
	dir.setSorting(QDir::DirsFirst);
	QFileInfoList list = dir.entryInfoList();
	int i = 0;
	bool is_dir;
	do
	{
		QFileInfo file_info = list.at(i);
		if (file_info.fileName() == "." | file_info.fileName() == "..")
		{
			i++;
			continue;
		}

		is_dir = file_info.isDir();
		if (is_dir)
		{
			addSubFolderImages(file_info.filePath(), string_list);
		}
		else
		{
			QString suffix = file_info.suffix();
			if (QString::compare(suffix, QString("tif"), Qt::CaseInsensitive) == 0)
			{
				QString absolute_file_path = file_info.absoluteFilePath();
				string_list.append(absolute_file_path);
			}
		}
		i++;
	} while (i < list.size());
}

cv::Mat AlignWidget::binningSwitch(cv::Mat& src, int bin)
{
	if (src.empty())
	{
		return cv::Mat();
	}
	if (bin < 2)
	{
		return src;
	}

	cv::Mat binning = cv::Mat::zeros(src.rows / bin, src.cols / bin, src.type());

	cv::Mat img_c;
	src.convertTo(img_c, CV_32SC1);
	int w = img_c.cols;
	int h = img_c.rows;

	for (size_t i = 0; i < binning.rows; i++)
	{
		for (size_t j = 0; j < binning.cols; j++)
		{
			unsigned short* data = binning.ptr<unsigned short>(i) + j;

			float sum = 0;
			for (size_t m = 0; m < bin; m++)
			{
				int* p = img_c.ptr<int>(i * bin + m) + j * bin;
				for (size_t n = 0; n < bin; n++)
				{
					sum += p[n];
				}
			}
			float result = sum / (bin * bin);
			data[0] = unsigned short(result);
		}
	}
	return binning;
}

void AlignWidget::on_btn_start_clicked()
{
	if (ui.btn_start->text() == BTN_START_TEXT)
	{
		Result result = start();
		if (!result.success)
		{
			QMessageBox::critical(this, "Align error", QString::fromStdString(result.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
			return;
		}
		updateState(RUNNING);
	}
	else {
		updateState(STOP);
		stop();
	}
}

void AlignWidget::on_btn_loadFile_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Load file"), ".", QStringLiteral("Files(*.recipe)"));
	if (fileName.isEmpty())
	{
		return;
	}
	ui.plainText_loadFile->setPlainText("");

	QList<QString> lineDatas = AnalyzeRecipe().selectCalibrateInfo(fileName, CalibrationConfig::instance()->getDutConfigReference(),
		CalibrationConfig::instance()->getAlignConfigReference());

	for (QString line : lineDatas)
	{
		ui.plainText_loadFile->appendPlainText(line);
	}
}

void AlignWidget::on_btn_confirmFiducial_clicked()
{
	confirmFiducial();
}

void AlignWidget::on_saveImageBtn_clicked()
{
	if (!CameraModel::GetInstance()->isConnected())
	{
		QMessageBox::critical(this, "Save image error", "MV camera is not connected.", QMessageBox::Ok,
			QMessageBox::NoButton);
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(
		this, QStringLiteral("Save image:"), "d:", QStringLiteral("(*.png *.jpg *.xpm  *.tif);;All files (*.*)"));
	if (!fileName.isEmpty())
	{
		cv::Mat img = CameraModel::GetInstance()->GetImage();
		if (!img.empty())
		{
			std::vector<int> compression_params;

			compression_params.push_back(cv::ImwriteFlags::IMWRITE_TIFF_COMPRESSION);
			compression_params.push_back(1);
			cv::imwrite(fileName.toStdString(), img, compression_params);
		}
	}
}

void AlignWidget::threadFinished()
{
	updateState(STOP);
	// Alignment::instance()->continueRun(false);
	ui.plainText_loadFile->setEnabled(true);

	Result result = watcher.future().result();
	if (!result.success && !result.errorCode)
	{
		QMessageBox::critical(this, "Alignment error", QString::fromStdString(result.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void AlignWidget::addFiducialSlot(QPointF scenePos)
{
	clearTableFiducial();
	addTableFiducial(cv::Point2f(scenePos.x(), scenePos.y()));
}

void AlignWidget::deleteFiducialSlot(QPointF scenePos)
{
	clearTableFiducial();
}

void AlignWidget::updateFiducialSlot(QPointF scenePos)
{
	updateTableFiducial(cv::Point2f(scenePos.x(), scenePos.y()));
}

void AlignWidget::messageBoxSlot(QString message)
{
	QMessageBox* msgBox = new QMessageBox(QMessageBox::Information, "Information", message, QMessageBox::Ok);
	msgBox->exec();

	delete msgBox;
	msgBox = nullptr;
}
