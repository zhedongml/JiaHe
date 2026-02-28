#pragma once
#include "windows.h"
#include "Result.h"
#include <QString>
#include <QMap>
#include "opencv2/opencv.hpp"
//#include "IQMetrics/ML_Luminance.h"

using namespace cv;
//using namespace MLIQMetrics;

class LumStatistics
{
	struct LumData{
		QString time;
		QString lightColor;
		QMap<QString, float> ISLumMap;
		float imageLum;
	};

public:
	static LumStatistics& instance();
	~LumStatistics();

	Result saveISluminance(QString XYZFilter, QString lightColor);
	Result saveImagelumCsv(QString file, QString lightColor, cv::Mat image);

	Result saveISlumCsv(QString filePath, QString lightColor);

private:
	LumStatistics();

	Result saveCsv(QString filePath, LumData lumData);

	Result createFileIfNotExists(const QString& filePath, const QString &firstLine);

private:
	LumData m_lumData;
};

