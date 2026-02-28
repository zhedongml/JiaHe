#include "crosscenterinfowidget.h"
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include "glimageview.cpp"
#include "harris.h"
#include "DashLineAutoRoi.cpp"
CrossCenterInfoWidget::CrossCenterInfoWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	crosscenterMap = new QMap<QString, cv::Point2f>;
	setAttribute(Qt::WA_DeleteOnClose);
}

CrossCenterInfoWidget::~CrossCenterInfoWidget()
{
	delete crosscenterMap;
}

void CrossCenterInfoWidget::setROIRegion(CrossCenterRegion region)
{
	crossCenterRegion = region;
}
void CrossCenterInfoWidget::findCrossCenter() {
	ui.filenamescbx->clear();
	cv::Rect rect(crossCenterRegion.startx, crossCenterRegion.starty, crossCenterRegion.width, crossCenterRegion.height);
	cv::Mat img = cv::imread(crossCenterRegion.baseFileName.toStdString(), -1);
	cv::Mat roiimg = img(rect);
	algorithm::MLCrossCenter my_cross;
	cv::Point2f center = my_cross.GetCrossCenter(roiimg);
	cvtColor(roiimg, roiimg, cv::COLOR_GRAY2RGB);
	cv::circle(roiimg, center, 1, cv::Scalar(0, 255, 0), -1);
	std::string coord = std::to_string(int(center.x + 0.5) + crossCenterRegion.startx) + ";" + std::to_string(int(center.y + 0.5) + crossCenterRegion.starty);
	putText(roiimg, coord, center, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0));
	putText(roiimg, crossCenterRegion.baseFileName.toStdString(), cv::Point(0, 10), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(255, 255, 255));
	addOneResultImg(roiimg, 0);
	ui.filenamescbx->addItem(crossCenterRegion.baseFileName);
}
void CrossCenterInfoWidget::findCrossCenters()
{
	crosscenterMap->clear();
	ui.filenamescbx->clear();
	QDir SourceDir;
	SourceDir.setFilter(QDir::Files | QDir::NoSymLinks);
	SourceDir.setPath(crossCenterRegion.filePath);
	QStringList sourceFilesList = SourceDir.entryList();
	cv::Rect rect(crossCenterRegion.startx, crossCenterRegion.starty, crossCenterRegion.width, crossCenterRegion.height);
	for (int i = 0, len = sourceFilesList.size(); i < len; i++) {

		QString fileName = sourceFilesList.at(i);
		QString fileStr = SourceDir.path() + QDir::separator() + fileName;
		cv::Mat img = cv::imread(fileStr.toStdString(), -1);
		cv::Mat roiimg = img(rect);
		algorithm::MLCrossCenter my_cross;
		cv::Point2f center = my_cross.GetCrossCenter(roiimg);
		cvtColor(roiimg, roiimg, cv::COLOR_GRAY2RGB);
		cv::circle(roiimg, center, 1, cv::Scalar(0, 255, 0), -1);
		std::string coord = std::to_string(int(center.x + 0.5) + crossCenterRegion.startx) + ";" + std::to_string(int(center.y + 0.5) + crossCenterRegion.starty);
		putText(roiimg, coord, center, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 0, 0));
		putText(roiimg, fileName.toStdString(), cv::Point(0, 10), cv::FONT_HERSHEY_TRIPLEX, 0.3, cv::Scalar(255, 255, 255));
		if (roiimg.depth() == CV_16U) {
			roiimg = convet16bitTo8bit(roiimg);
		}
		addOneResultImg(roiimg, i);
		ui.filenamescbx->addItem(fileName);
		crosscenterMap->insert(fileName, center);
		if (fileStr == crossCenterRegion.baseFileName) {
			basePointX = int(center.x + 0.5);
			basePointY = int(center.y + 0.5);
		}
	}
	updateResultLable();
	disconnect(ui.filenamescbx, SIGNAL(currentIndexChanged(QString)), 0, 0);
	connect(ui.filenamescbx, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateBasePoint(QString)));
}
void CrossCenterInfoWidget::updateBasePoint(QString text) {
	cv::Point2f pt = crosscenterMap->value(text);
	basePointX = int(pt.x + 0.5);
	basePointY = int(pt.y + 0.5);
	updateResultLable();
}
void CrossCenterInfoWidget::addOneResultImg(cv::Mat img, int idx)
{
	QGridLayout* lyt = (QGridLayout*)(ui.displayimgwidget->layout());
	GLImageView* view = new GLImageView(this);
	view->showImage(img);
	lyt->addWidget(view, idx / 3, idx % 3);
}

void CrossCenterInfoWidget::updateResultLable()
{
	QGridLayout* lyt = (QGridLayout*)(ui.textinfowidget->layout());
	QList<QLabel*> lableLst = ui.textinfowidget->findChildren<QLabel*>();
	if (lableLst.isEmpty()) {
		for (int i = 0; i < crosscenterMap->size(); i++) {
			QLabel* label = new QLabel(this);
			lyt->addWidget(label, i + 1, 0);
		}
	}
	lableLst = ui.textinfowidget->findChildren<QLabel*>();


	QMap<QString, cv::Point2f>::iterator it = crosscenterMap->begin();
	int i = 0;
	while (it != crosscenterMap->end()) {
		QString itemName = it.key();
		cv::Point2f pt = it.value();
		int xdiff = int(pt.x + 0.5) - basePointX;
		QString xdiffstr = itemName + ":X:" + QString::number(xdiff) + ";";
		int ydiff = int(pt.y + 0.5) - basePointY;
		QString ydiffstr = "Y:" + QString::number(ydiff);
		lableLst.at(i)->setText(xdiffstr + ydiffstr);
		i++;
		it++;
	}
}
cv::Mat CrossCenterInfoWidget::convet16bitTo8bit(cv::Mat& rawimg) {
	cv::Mat newImg;
	if (rawimg.depth() == CV_16U)
	{
		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		minMaxIdx(rawimg, minp, maxp);

		if (maxv > 4095.1)
		{
			cv::Mat tmp = rawimg / 65535 * 255;

			tmp.convertTo(newImg, CV_8UC1);
		}
		else
		{
			cv::Mat tmp = rawimg / maxv * 255;

			tmp.convertTo(newImg, CV_8UC1);
		}

		return newImg;
	}
	else {
		return rawimg;
	}

}
void CrossCenterInfoWidget::autoChromaticAberration()
{
	double scaled = 0.5;
	int roiwidth = 200;
	int roiheight = 200;
	crosscenterMap->clear();
	ui.filenamescbx->clear();
	QDir SourceDir;
	SourceDir.setFilter(QDir::Files | QDir::NoSymLinks);
	SourceDir.setPath(crossCenterRegion.filePath);
	QStringList sourceFilesList = SourceDir.entryList();

	for (int i = 0, len = sourceFilesList.size(); i < len; i++) {
		QString fileName = sourceFilesList.at(i);
		if (fileName.contains("_G") && i != 0) {
			sourceFilesList.swapItemsAt(0, i);
			break;
		}
	}
	cv::Point2d nearPt;
	DashLineAutoRoi dashlineAuto;
	for (int i = 0, len = sourceFilesList.size(); i < len; i++) {

		QString fileName = sourceFilesList.at(i);
		QString fileStr = SourceDir.path() + QDir::separator() + fileName;
		cv::Mat cvImg = cv::imread(fileStr.toStdString(), -1);
		cv::Rect rectfull(0, 0, cvImg.cols, cvImg.rows);
		cv::Point2d center;
		if (i == 0) {
			center = dashlineAuto.getCornnerPoint(cvImg);
			nearPt.x = center.x;
			nearPt.y = center.y;
		}
		else {
			center = dashlineAuto.getCornnerPoint(cvImg, nearPt);
		}

		cv::Rect rect(center.x - roiwidth / 2, center.y - roiheight / 2, roiwidth, roiheight);
		rect = rect & rectfull;
		cv::Mat roiimg = cvImg(rect);
		algorithm::MLCrossCenter my_cross;
		cv::Point2f crosscenter = my_cross.GetCrossCenter(roiimg);
		center.x = int(crosscenter.x + 0.5) + rect.x;
		center.y = int(crosscenter.y + 0.5) + rect.y;

		cv::Mat resultImg;
		cvtColor(cvImg, resultImg, cv::COLOR_GRAY2RGB);
		cv::circle(resultImg, center, 5, cv::Scalar(255, 255, 0), -1);
		std::string coord = std::to_string(int(center.x + 0.5) + crossCenterRegion.startx) + ";" + std::to_string(int(center.y + 0.5) + crossCenterRegion.starty);
		putText(resultImg, coord, center, cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255));
		putText(resultImg, fileName.toStdString(), cv::Point(0, 10), cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(255, 255, 255));
		if (cvImg.depth() == CV_16U) {
			resultImg = convet16bitTo8bit(resultImg);
		}
		addOneResultImg(resultImg, i);
		ui.filenamescbx->addItem(fileName);
		crosscenterMap->insert(fileName, center);
		if (fileStr == crossCenterRegion.baseFileName) {
			basePointX = int(center.x + 0.5);
			basePointY = int(center.y + 0.5);
		}
	}
	updateResultLable();
	disconnect(ui.filenamescbx, SIGNAL(currentIndexChanged(QString)), 0, 0);
	connect(ui.filenamescbx, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateBasePoint(QString)));
}
