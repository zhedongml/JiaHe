#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#include "ml_image_public.h"
#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif
using namespace std;
namespace MLImageDetection
{
    struct  GridRe
    {
        cv::Point2f center;
        vector<cv::Point2f>corners;
        cv::Mat indexMap;
        cv::Mat xLocMat;
        cv::Mat yLocMat;
        cv::Size boardSize;
        cv::Mat imgdraw;
        bool flag = true;
        string errMsg = "";
    };

class ALGORITHM_API MLGridDetect : public MLimagePublic
{
  public:
    MLGridDetect();
    ~MLGridDetect();
  public:
    void setAccurateDetectionFlag(bool flag);
    GridRe getGridCenter(const cv::Mat img);
    GridRe getGridCenterPreLoc(const cv::Mat img,cv::Point2f cen);
    GridRe getGridCorners(cv::Mat img);
    GridRe getGridHoughLine(cv::Mat img);
    GridRe getGridHoughLine1(cv::Mat img);
    GridRe getGridContour(cv::Mat img);
    GridRe getGridTemplateGaussian(cv::Mat img);
    GridRe getGridHist(cv::Mat img);
    GridRe getGridPreLoc(cv::Mat img,cv::Mat xlocmat,cv::Mat ylocmat);
    cv::Point2f getGridCenter(cv::Mat xmat, cv::Mat ymat);

    void SetbinNum(int bin);
    void SetGridPointsClusters(double thresh);
    void SetGridxyClassification(double thresh);
    void SetGridWidth(double w);
    void SetChessboardUpdateFlag(bool flag);
    cv::Mat rotateGridImg(cv::Mat img);
    void writeGridInfoToCSV(GridRe gridRe);
    void readGridInfoFromCSV(GridRe& gridRe);
private:
    cv::Point2f getAccurateCenter(cv::Point2f c0, cv::Mat img);
    cv::Point2f getCenLoc(cv::Mat xloc, cv::Mat yloc);
  private:
    int m_binNum=1;
    int m_pointsClusters = 150;
    int m_xyClassification = 240;
    int m_gridWidth = 290;
    int m_len = 520;
    bool m_update = false;
    bool accurateFlag = false; 
};

} // namespace MLImageDetection
