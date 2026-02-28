#pragma once
#include<numeric>
#include <string>
#include <algorithm>
#include <functional>
#include "ml_image_public.h"
using namespace std;
namespace algorithm
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
    ALGResult flag;   
};
struct CenterCrossHairRe
{
    cv::Point2f center;
    cv::Rect rectCenter;
    vector<cv::Point2f> cenVec;
    vector<cv::Rect> rectVec;
    cv::Mat img_draw;
    ALGResult flag;
};

class ALGORITHM_API MultiCrossHairDetection:public MLimagePublic
{
  public:
    MultiCrossHairDetection();
    ~MultiCrossHairDetection();
  public:
    MultiCrossHairRe getMuliCrossHairCenter(cv::Mat img,int binNum=1); 
    MultiCrossHairRe getMultiCrossHairCenterBySlope(cv::Mat img, vector<cv::Point2f> &re);
    //MultiCrossHairRe getMuliCrossHairCenterByDistance(cv::Mat img, int binNum = 1);
    
    //eye=0 left eye=1 right
    MultiCrossHairRe getMuliCrossHairCenterByDistance(cv::Mat img,int eyeLoc, int binNum = 1);

    CenterCrossHairRe getCenterCrossHairCenter(cv::Mat img, int binNum = 1);
    CenterCrossHairRe getCenterCrossHairCenterByContour(cv::Mat img,int binNum=1);
    cv::Mat getImgdraw();

    vector<cv::Point2f> findCrossHairCenterContour(cv::Mat img,vector<cv::Rect>&rectVec);
    vector<cv::Point2f> findCrossHairCenterTemplate(cv::Mat img, cv::Mat templ);
    cv::Point2f getPointsMean(vector<cv::Point2f> pts);
    int findClosestPoint(vector<cv::Point2f> pts, cv::Point2f p0);
    int m_binning;

  private:
    cv::Point2f getPointByDeg(cv::Point2f center, double theta, double xdeg, double ydeg);
    void updateCenterPoints(cv::Point2f &pt1, cv::Point2f &d1, double thetaH, cv::Point2f center, int eyeLoc);
  private:
    cv::Mat m_img_draw;
    double m_cenx = 76;
   // vector<double>xdegL = {-153,945,-1252,-153,-153,1456,-1763,-1763,1456,0,696,-696,696,-696};
    //vector<double> ydegL = {0, 0, 0, 1098, -1098, 1189, 1189, -1189, -1189, 0, 346, 346, -346, -346};
    vector<double> xdegL = {153, - 946, 1252, 153, 153, - 1457, 1764, 1764 ,- 1457, 0, - 697, 697, - 697, 697};
    vector<double> ydegL = {0 ,0 ,0, - 1099, 1099, - 1190 ,- 1190, 1190, 1190, 0 ,- 347 ,- 347, 347, 347};
   
    //vector<double> xdegR = {153, -945, 1252, 153, 153, -1456, 1763, 1763, -1456, 0, 696, -696, -696, 696};
    //vector<double> xdegR = {-153, 945, -1252, -153, -153, 1456, -1763, -1763, 1456, 0, 696, -696, -696, 696};
    //vector<double> ydegR = {0, 0, 0, 1098, -1098, 1189, 1189, -1189, -1189, 0, 346, 346, -346, -346};
    vector<double> xdegR = {-153, 946, - 1252, - 153 ,- 153, 1457, - 1764, - 1764, 1457, 0 ,- 697, 697 ,697, - 697};
    vector<double> ydegR = {0, 0, 0, - 1099 ,1099, - 1190, - 1190, 1190, 1190,0, - 347 ,- 347, 347, 347};

    vector<string> strVec = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "D1", "D2", "D3", "D4", "D5"};
    //vector<string> strVec = {"1"};

    int m_binNum = 1;

};
} // namespace algorithm
