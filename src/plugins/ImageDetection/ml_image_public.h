#pragma once

#ifndef MLALGORITHM_MLIMAGEPUBLIC_H_
#define MLALGORITHM_MLIMAGEPUBLIC_H_
#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <direct.h>
#include<io.h>
#include<armadillo>
#include<vector>
//using namespace cv;
using namespace std;
#define DEBUG 0

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

namespace MLImageDetection
{
struct ALGResult
{
    bool success;
    std::string errorMsg;

  public:
    ALGResult()
    {
        success = true;
        errorMsg = "";
    }
    ALGResult(bool success, std::string errorMsg, int errorCode = 0)
    {
        this->success = success;
        this->errorMsg = errorMsg;
    }
    ~ALGResult()
    {
    }
};
enum MirrorALG
{
    LEFT_RIGHT,
    UP_DOWN,
    NO_OP
};
static struct ROIParaNew
{
    float step = 1;
    float cenX = 0;
    float cenY = 0;
    float width = 0;
    float height = 0;
    int rotationAngle = 0;
    MirrorALG mirror = NO_OP;
};
class ALGORITHM_API MLimagePublic
    {
    public:
        void NaiveRemoveNoise(cv::Mat& srcImage, double  pArea);
        void ContoursRemoveNoise(cv::Mat& srcImage, double pArea);
        void Clear_MicroConnected_Areas(cv::Mat src, cv::Mat& dst, double min_area);
        bool GetAllMinLoc(cv::Mat image, cv::Mat templ, double sim, cv::Scalar mask, vector<cv::Point> *all_min_loc,
                          int num = 1);
        void readROItxtFile(string filepath, vector<cv::Rect>& roiArea);
        void readPointstxtFile(string filepath, vector<cv::Point2f>& points);
        void writePointsFile(string fileName, vector<cv::Point2f>points);
        void writeROIFile(string fileName, vector<cv::Rect>roiRect);
        cv::Mat readCSVToMat(const string &filename);
        void writeMatTOCSV(std::string filename, cv::Mat m);
        vector<int> findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence);
        int  findEdgePt(cv::Mat data);


        cv::Mat convertToUint8(const cv::Mat img);
        cv::Mat convert12bitTo8bit(cv::Mat img);
        cv::Mat convert2ColorImage(cv::Mat img);
        cv::Mat convertTo3Channels(const cv::Mat binImg);
        cv::Mat convertToGrayImage(const cv::Mat img);
        cv::Mat getRectROIImg(cv::Mat img, cv::Rect rect);

        cv::Mat getCircleMask(cv::Size size, int radius);

        cv::RotatedRect getMaxAreaContourRect(cv::Mat imgth, cv::Rect &rect);
        cv::RotatedRect getClosestContourRect(cv::Mat imgth,cv::Point2f c0,cv::Rect &rect);

        cv::Mat GetROIMat(cv::Mat img, cv::Rect rec);
        double Getdistance(cv::Point p1, cv::Point p2);
        double corrlateMat90(cv::Mat A);
        double corrlateMat180(cv::Mat A);
        double getMagnitude(cv::Mat img);
        vector<cv::Point> drawline(double k, double b);
        vector<cv::Point> drawline(vector<double> line);
        double median(vector<double>valVec);
        double median(double a1, double a2, double a3);
        vector<double> LineInf(cv::Point2f p1, cv::Point2f p2);
        cv::Point2f getintersection(double k1, double b1, double k2, double b2);
        void writeCSV(string filename, cv::Mat m);
        cv::Point2f get2LineIntersectionPoint(cv::Point2f lineOnePt1, cv::Point2f lineOnePt2,
            cv::Point2f lineTwoPt1, cv::Point2f lineTwoPt2);
        bool sort_user(const vector<double> x, vector<double>& result, vector<int>& index);
        cv::Mat linspace(double begin, double finish, int number);
        double percentile(cv::Mat img, double p);
        double percentile(const arma::vec &data, double p);
        double getPercentile(cv::Mat seqMat, cv::Mat mask, double p);
        arma::mat openCVMatToArmaMat(cv::Mat img);
        cv::Mat armaMatToOpenCVMat(arma::mat mat);
        cv::Mat armaVecToOpenCVMat(arma::vec v);
        arma::vec openCVMatToArmaVec(cv::Mat img);
        double deg2rad(double deg);


        double calculateMatMedian(cv::Mat roi);

        cv::Mat getRotationImg(cv::Mat img, double angle);

        // need to trasnfer to math
        vector<double> line_fit(cv::Mat abs_grad, int flag);
        vector<double> line_fit_orthogonal(cv::Mat abs_grad);
        cv::Point2f calLinesCross(float a1, float b1, float c1, float a2, float b2, float c2);
        cv::Point2f calLinesIntersection(cv::Vec4f line1, cv::Vec4f line2);
        vector<double>unique(vector<double>xVec,int thresh);
        int findIndex(vector<cv::Point2f>pts, cv::Point2f pt0,double thresh);
        int findNeighborNum(vector<double>xVec, double x0, double thresh = 25);
        int findNeighborNum(vector<double> xVec, double x0, vector<int>&inVec, double thresh = 25);

        vector<cv::Point2f> pointsClusters(vector<cv::Point2f> pts, double thresh = 25);
        vector<double>dataClusters(vector<double>data,vector<int>&vecList,double thresh=25);
        vector<double> dataIndexMap(vector<double> data, vector<cv::Point> P, cv::Mat &indexMap, double thresh = 50);
        cv::Mat generatePointsIndexMap1(vector<cv::Point2f>pts,bool extendFlag=false,double thresh=50);
       
        cv::Mat generatePointsIndexMap(vector<cv::Point2f> pts, bool extendFlag = false, double thresh = 50);

        std::vector<cv::Point> convecHull(std::vector<cv::Point> &points); 
        double getDistancePL(cv::Point a, cv::Point b, cv::Point c);
        vector<int> sortIndex(vector<double> xVec);
        cv::Point2f getPointsCenter(vector<cv::Point2f>pts);
        int findNeartVal(vector<double> xVec, double x0);
        void circleLeastFit(const std::vector<cv::Point> points, cv::Point2f &center, double &radius);
        void circleLeastFit(const std::vector<cv::Point2f> points, cv::Point2f& center, double& radius);

        string get_current_dir();
        string createDir();
        string get_exe_dir();
        string currentTimetoStr();

        cv::Point2f getOpticalCenterAfterRotation(cv::Point2f center, cv::Size s, int rota);
        cv::Point2f getOpticalCenterAfterMirror(cv::Point2f center, cv::Size s, MirrorALG m);
        cv::Point2f updateOpticalCenter(cv::Point2f base, cv::Size s, int rotationAngle, MirrorALG mirror);

        bool Is_File_Exist(const std::string &file_path);

         bool containRect(cv::Rect a, cv::Rect b); // a include b
        cv::Point2f getRectCenter(cv::Rect rect);
        cv::Rect updateRectByRatio(cv::Rect, double ratio);
        void updateRectByRatio1(cv::Rect&rect, double ratio);

        cv::Rect rotateRect(cv::RotatedRect rectR, cv::Point2f center, double angle);
        vector<cv::Point2f> rotatePoints(vector<cv::Point2f> pts, cv::Point2f center, double angle);
       
        // just for austin
        void updateRotateImg(cv::Mat &img,double angle);
        cv::Rect updateRotateRect(cv::RotatedRect rectR, cv::Point2f center);

        
        // draw on image
        string numToString(double value);
        void drawPointOnImage(cv::Mat &imgdraw, cv::Point2f pt, int r = 5);
        void drawPointsOnImage(cv::Mat &imgdraw, std::vector<cv::Point2f> pts, int r = 5,cv::Scalar color=cv::Scalar(0,0,255));
        void drawRectOnImage(cv::Mat &imgdraw, cv::Rect rect, int width = 5);
        void drawRectsOnImage(cv::Mat &imgdraw, std::vector<cv::Rect> rectVec, int width = 2);
        void drawCircleOnImage(cv::Mat &imgdraw, cv::Point2f pt, int r);
        void putTextOnImage(cv::Mat &imgdraw, std::string str, cv::Point2f pt, int width = 5);
        void drawLineOnImage(cv::Mat &imgdraw, cv::Point2f pt1, cv::Point2f pt2, int width = 2);


    };

}
#endif // !MLALGORITHM_MLIMAGEPUBLIC_H_
