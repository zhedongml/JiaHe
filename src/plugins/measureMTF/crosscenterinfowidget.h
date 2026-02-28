#pragma once

#include <QWidget>
#include "ui_crosscenterinfowidget.h"

//#include "ml_cross_center.h"
#include "../MLAlgorithm/ml_cross_center.h"
#include <QMap>
struct CrossCenterRegion {
	int startx = 0;
	int starty = 0;
	int width = 0;
	int height = 0;
	QString filePath = "";
	QString baseFileName = "";
};
class CrossCenterInfoWidget : public QWidget
{
	//Q_OBJECT

public:
	CrossCenterInfoWidget(QWidget* parent = Q_NULLPTR);
	~CrossCenterInfoWidget();
	void setROIRegion(CrossCenterRegion region);
	void findCrossCenters();
	void findCrossCenter();
	void addOneResultImg(cv::Mat img, int idx);
	void updateResultLable();
	void autoChromaticAberration();

private:
	Ui::CrossCenterInfoWidget ui;
	CrossCenterRegion crossCenterRegion;
	int basePointX = 0;
	int basePointY = 0;
	QMap<QString, cv::Point2f>* crosscenterMap;

	cv::Mat convet16bitTo8bit(cv::Mat& rawimg);
private slots:
	void updateBasePoint(QString text);
};

