#include "FindDistance.h"
#include <fstream>
#include <QWidget>
#include <MAalignment/CameraModel.h>
#include <QHash>
#include <string>
#include "pluginsystem/Services.h"
#include <extlib/logplus/LogPlus.h>
#include <piMotionActor/Motion3DModel.h>
#include "AlignUtils.h"
#include <integratingSphere\integratingspheretcpmodel.h>
#include <PLCControl\PLCController.h>
#include "piMotionActor/MoveLimit.h"
#include "Core/loggingwrapper.h"
#include <QDateTime>

using namespace std;

FindDistance::FindDistance()
{
}

FindDistance::~FindDistance()
{
}

Result FindDistance::getBestDistance(float &dutZ, const DutDistanceInfo &dutDistanceInfo)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    Result ret = AlignUtils::controlLight(true, false);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    ret = initMove(dutDistanceInfo.inView, dutDistanceInfo.distanceLimit);
    if (!ret.success){
        return ret;
    }

    ret = MoveLimit::instance()->setMotion3DMoveAlignSpeed(true);
    if (!ret.success)
    {
        return ret;
    }

    ret = alignBestDistance(dutZ, dutDistanceInfo);
    if (!ret.success)
    {
        MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);
        return ret;
    }

    ret = MoveLimit::instance()->setMotion3DMoveAlignSpeed(false);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    //LoggingWrapper::instance()->error(QString("----input Z=%1, time=%2").arg(dutZ).arg(takeTime));
    return ret;
}

void FindDistance::continueRun(bool isContinue)
{
    m_waitPause.continueRun(isContinue);
}

void FindDistance::notifyPause(bool isPause)
{
    m_waitPause.notifyPause(isPause);
}

Result FindDistance::initMove(cv::Point2f inView, const DistanceLimit &config)
{
    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    cv::Point3f coord;
    coord.x = inView.x;
    coord.y = inView.y;
    coord.z = config.high;
    return AlignUtils::acsAbsoluteMove(coord);
}

Result FindDistance::waitPause(bool isPause)
{
    return m_waitPause.waitPause(isPause);
}

Result FindDistance::calculateByStep(float posMin, float posMax, float step, float &bestPos)
{
    Result ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs("Z", posMin * UNIT_CONVERSION);
    if (!ret.success)
    {
        return ret;
    }

    int decreaseNumber = 0;
    m_imageQuality.clear();
    m_distance.clear();

    //int tmp = 0;
    float qualtiyMax = 0; 
    for (size_t i = 0; i <= (posMax - posMin); i += step)
    {
        // TODO: is or not need sleep
        Sleep(1000);
        cv::Mat img = CameraModel::GetInstance()->GetImage();

        //string name = "D:\\AA\\" + to_string(tmp++) + ".tif";    
        //cv::imwrite(name,img);

        double iqValue;
        try
        {
            iqValue = tenengard(img);
        }
        catch (exception e)
        {
            string msg = e.what();
            return Result(false, "Input AA align Z error, get img qualtiy exception, " + msg);
        }

        m_imageQuality.push_back(iqValue);
        m_distance.push_back(Motion3DModel::getInstance(withDUT)->getPosition().z / UNIT_CONVERSION);

        //LoggingWrapper::instance()->info(QString("----iqValue=%1, tmp=%2").arg(iqValue).arg(tmp));
        if (abs(iqValue) <= 0.000001)
        {
            break;
        }

        if (abs(qualtiyMax) < 0.001)
        {
            qualtiyMax = iqValue;
        }
        else
        {
            if (qualtiyMax > iqValue || abs(qualtiyMax - iqValue) < 0.01)
            {
                decreaseNumber++;
            }
            else
            {
                qualtiyMax = iqValue;
                decreaseNumber = 0;
            }
        }

        //if (m_imageQuality.size() >= 2 && (iqValue < m_imageQuality.at(m_imageQuality.size() - 2) 
        //    || abs(iqValue - m_imageQuality.at(m_imageQuality.size() - 2)) < 0.01))
        //{
        //    decreaseNumber++;
        //}
        //else
        //{
        //    decreaseNumber = 0;
        //}

        if (decreaseNumber > 3)
        {
            break;
        }

        if (Motion3DModel::getInstance(withDUT)->getPosition().z / UNIT_CONVERSION > posMax)
        {
            break;
        }
        else
        {
            ret = waitPause(m_waitPause.recipePause());
            if (!ret.success)
            {
                return ret;
            }

            ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveRel("Z", step * UNIT_CONVERSION);
            if (!ret.success)
            {
                return ret;
            }
        }

        ret = waitPause(m_waitPause.recipePause());
        if (!ret.success)
        {
            return ret;
        }
    }

    vector<float>::iterator biggestIQValue = max_element(begin(m_imageQuality), end(m_imageQuality));
    int index = distance(begin(m_imageQuality), biggestIQValue);
    float absolute_pos = m_distance[index]; 

    //for (int i = 1; i < 3; i++)
    //{
    //    if (m_distance.size() > index + i)
    //    {
    //        if (abs(m_imageQuality[index + i] - m_imageQuality[index + i - 1]) < 0.01)
    //        {
    //            absolute_pos = m_distance[index + i];
    //        }
    //    }
    //}

    bestPos = absolute_pos;
    return Result();
}

Result FindDistance::alignBestDistance(float &dutZ, const DutDistanceInfo &dutDistanceInfo)
{
    double iqValue;
    float posMax = dutDistanceInfo.distanceLimit.high;
    float posMin = dutDistanceInfo.distanceLimit.low;
    float step = dutDistanceInfo.stepZ;

    Result ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Start find clearest image!");
    ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs("Z", (posMin + posMax) * UNIT_CONVERSION / 2.0);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    CameraModel::GetInstance()->SetMLExposureAuto();
    _sleep(100);

    float bestPos = 0;
    float roughStep = step;
    ret = calculateByStep(posMin, posMax, roughStep, bestPos);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    ret = Motion3DModel::getInstance(withDUT)->Motion3DMoveAbs("Z", bestPos * UNIT_CONVERSION);
    if (!ret.success)
    {
        return ret;
    }

    ret = waitPause(m_waitPause.recipePause());
    if (!ret.success)
    {
        return ret;
    }

    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Motion:get clearest img distance end!");
    dutZ = bestPos;
    return Result();
}

double FindDistance::tenengard(cv::Mat& image)
{
    // assert(image.empty());
    cv::Mat gray_img, sobel_x, sobel_y, G;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, cv::COLOR_BGR2GRAY);
    }
    else
        gray_img = image.clone();

    //分别计算x/y方向梯度
    cv::Sobel(gray_img, sobel_x, CV_32FC1, 1, 0);
    cv::Sobel(gray_img, sobel_y, CV_32FC1, 0, 1);
    cv::multiply(sobel_x, sobel_x, sobel_x);
    cv::multiply(sobel_y, sobel_y, sobel_y);
    cv::Mat sqrt_mat = sobel_x + sobel_y;
    cv::sqrt(sqrt_mat, G);
    return cv::mean(G)[0];
}


