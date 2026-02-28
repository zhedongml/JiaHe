#pragma once
#include <QString>
//#include "MLAlgorithm/ml_chessboardDetect.h"
//#include"MLCherkerboardDetect.h"
#include"ImageDetection/MLCherkerboardDetect.h"
#include "Result.h"

using namespace std;
using namespace cv;
using namespace MLImageDetection;

class SlbManage
{
public:
    enum RETICLE_TYPE
    {
        CLEAR = 0,
        CHECKER_SMALL,
        CHECKER_BIG,
    };

    Result saveCR(cv::Mat imgP, cv::Mat imgN, QString color, RETICLE_TYPE reticleType);
    Result saveCR(cv::Mat crMat, QString color, RETICLE_TYPE reticleType);


    Result queryCR(cv::Mat &crData, QString color, RETICLE_TYPE reticleType);

private:
    Result saveMat(cv::Mat crData, QString color, RETICLE_TYPE reticleType);

    QByteArray mat2ByteArray(const cv::Mat &image);
    cv::Mat byteArray2Mat(const QByteArray &byteArray);

    // TODO: to be delete
    void test();

private:
    const int MAX_COUNT = 50;
};
