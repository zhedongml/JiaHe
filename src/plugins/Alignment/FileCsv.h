#pragma once
#include <QList>
#include "Result.h"
#include "opencv2/opencv.hpp"

struct DutCsvInfo
{
    cv::Point3f inputGrating;
    std::vector<cv::Point3f> fiducials;
    cv::Point3f eyeboxCenter;
    std::vector<cv::Point3f> eyeboxsCSYS;

    void clear()
    {
        fiducials.clear();
        eyeboxsCSYS.clear();
    }
};

class FileCsv
{
public:
    Result readWG(QString filePath, DutCsvInfo &info);
    Result writeWG(QString filePath, const DutCsvInfo &info);

    Result readEyeboxs(QString filePath, QList<cv::Point3f> &eyeboxs);
	Result writeEyeboxs(QString filePath, const QList<cv::Point3f> &eyeboxs);

private:
	Result readCsv(QString filePath, QList<QStringList> &data);
	Result writeCsv(QString filePath, QList<QStringList> &data);
};
