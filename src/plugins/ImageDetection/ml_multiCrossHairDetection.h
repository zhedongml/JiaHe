#pragma once
#include<numeric>
#include <string>
//#include <MLImageDetection>
#include <algorithm>

#include <functional>
#include "ml_image_public.h"
using namespace std;
namespace MLImageDetection
{
enum LOCType
{
    LEFT,
    RIGHT
};
struct MultiCrossHairRe
{
    map<string, cv::Point2f> centerMap;
    map<string, cv::Rect> rectMap;
    vector<cv::Rect> rectVec;
    cv::Mat img_draw;
    bool flag = "true";
    string errMsg = "";
};
struct CenterCrossHairRe
{
    cv::Point2f center;
    cv::Rect rectCenter;
    vector<cv::Point2f> cenVec;
    vector<cv::Rect> rectVec;
    cv::Mat img_draw;
    bool flag = true;
    string errMsg = "";
};

class ALGORITHM_API MultiCrossHairDetection:public MLimagePublic
{
  public:
    MultiCrossHairDetection();
    ~MultiCrossHairDetection();
  public:
    void setBinNum(int binNum);
    MultiCrossHairRe getMuliCrossHairCenterByTemplate(cv::Mat img, int binNum);
    MultiCrossHairRe getMuliCrossHairCenter(cv::Mat img,int binNum,bool flag); 
    MultiCrossHairRe getMultiCrossHairCenterBySlope(cv::Mat img, vector<cv::Point2f> &re);
    //MultiCrossHairRe getMuliCrossHairCenterByDistance(cv::Mat img, int binNum = 1);
   
    //eye=0 left eye=1 right
    MultiCrossHairRe getMuliCrossHairCenterByDistance(cv::Mat img,int eyeLoc, int binNum = 1);

    CenterCrossHairRe getCenterCrossHairCenter(cv::Mat img, int binNum = 1);
    CenterCrossHairRe getCenterCrossHairCenterByContour(cv::Mat img,int binNum=1);
    cv::Mat getImgdraw();

    vector<cv::Point2f> findCrossHairCenterContour(cv::Mat img,vector<cv::Rect>&rectVec,int binNum);
    vector<cv::Point2f> findCrossHairCenterTemplate(cv::Mat img, cv::Mat templ);
    cv::Point2f getPointsMean(vector<cv::Point2f> pts);
    int findClosestPoint(vector<cv::Point2f> pts, cv::Point2f p0);
    cv::Point2f getExactCrossCenter(cv::Mat img8, cv::Rect rect);

  private:
    cv::Point2f getPointByDeg(cv::Point2f center, double theta, double xdeg, double ydeg);
    void updateCenterPoints(cv::Point2f &pt1, cv::Point2f &d1, double thetaH, cv::Point2f center, int eyeLoc);
    vector<cv::Point2f> sortPoints(vector<cv::Point2f> pts);
private:
    cv::Mat m_img_draw;
    double m_Rdeg = 1098;
    double m_cenx = 76;
    vector<double>xdegL = {-153,945,-1252,-153,-153,1456,-1763,-1763,1456,0,696,-696,696,-696};
    vector<double> ydegL = {0, 0, 0, 1098, -1098, 1189, 1189, -1189, -1189, 0, 346, 346, -346, -346};
    vector<double> xdegR = {153, -945, 1252, 153, 153, -1456, 1763, 1763, -1456, 0, 696, -696, -696, 696};
    vector<double> ydegR = {0, 0, 0, 1098, -1098, 1189, 1189, -1189, -1189, 0, 346, 346, -346, -346};
    vector<string> strVec = {"1"};
    int m_binNum = 1;
    int m_len = 1700;
    int resizeNum = 1;
};
} // namespace MLImageDetection
