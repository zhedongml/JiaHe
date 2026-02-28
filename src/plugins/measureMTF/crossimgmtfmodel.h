#pragma once

#include <QObject>
#include <QVector>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "mtfmodelconfig.h"
#include <QMap>
struct CrossItemROI
{
    int id = 0;
    int fromx = 0;
    int fromy = 0;
    int width = 0;
    int height = 0;
    QString roiDir = "V";
};
struct ROIMTFValues
{
    int result = 0;
    QString roiDir = "V";
    int roiId = 0;
    int binLen = 0;
    double *m_mtfVals = nullptr;
    double *m_freqVals = nullptr;
    double *m_lsfreqVals = nullptr;
    double *m_lsfVals = nullptr;
    ~ROIMTFValues()
    {
        if (m_mtfVals != nullptr)
        {
            delete[] m_mtfVals;
        }
        if (m_freqVals != nullptr)
        {
            delete[] m_freqVals;
        }

        if (m_lsfreqVals != nullptr)
        {
            delete m_lsfreqVals;
        }
        if (m_lsfVals != nullptr)
        {
            delete[] m_lsfVals;
        }
    }
    QMap<QString, double> specMtfVal; // key QString::number(freq,'f',3)
};
struct PassDetail
{
    QString freq = "0";
    double mtfVal = 0;
    double upLimit = 0;
    double lowLimit = 0;
    QString dir = "V";
};
struct PassResult
{
    int aoiId = 0;
    bool isPass = false;
    QVector<PassDetail> passDetails;
};
class CrossImgMTFModel : public QObject
{
    Q_OBJECT

  public:
    CrossImgMTFModel(QObject *parent);
    CrossImgMTFModel();
    ~CrossImgMTFModel();
    QVector<CrossItemROI> m_roiVec;
    bool saveMTFData(QString path, QString fileName, int index);
    bool saveData = true;
    QVector<ROIMTFValues *> m_mtfValsVec;
    void calculateMTF(bool isAuto = true, QList<int> list = QList<int>());
    void setImage(cv::Mat img);
    cv::Mat getImage();
    void setAOIId(int val);
    int getAOIId();
    PassResult *judgeResult();

    QVector<CrossItemROI> getCrossROI(bool isAuto = true, int num = 4);
    QVector<CrossItemROI> getRectangleROI(int num = 4);

    QVector<cv::Rect> getCrossROIFour(cv::Mat rectImg);
    bool mtfROICalculateFlag(cv::Mat img, int i);
    cv::Point2f getLineAndRectIntersectionPoint(cv::Point2f p1, cv::Point2f P0, cv::Point2f P3, cv::Rect rect);
    std::vector<cv::Point> RectangleROI(cv::Mat img);


  private:
    cv::Mat crossImg;
    ROIPosConfig roiPosCfg;
    double focuslength = 0;
    double vid = DBL_MAX;
    QVector<CrossItemROI> getCrossROI(QList<int> list, bool isAuto = true);
    cv::Mat cvMatTo8Bit(const cv::Mat &intImg);
    void addOneROI(cv::Point2f roiCenter, int id, QString dir = "V");
    void saveImageData(QString path, QString fileName, int index);
    void saveCSVData(QString path, QString fileName, int index);
   // bool mtfROICalculateFlag(cv::Mat img,int i);
    void clearResult();
    int aoiId = 0;

    bool judgePassOrFail(const QVector<PassDetail> &mtfDataInfo);
};
