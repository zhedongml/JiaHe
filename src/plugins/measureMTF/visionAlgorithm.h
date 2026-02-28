#pragma once

#ifndef ML_VISIONALGORITHM_H
#define ML_VISIONALGORITHM_H

#include <fstream>
#include <iostream>
#include <string>

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <Qapplication>

#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

struct s_TemplData
{
    std::vector<cv::Mat> vecPyramid;
    std::vector<cv::Scalar> vecTemplMean;
    std::vector<double> vecTemplNorm;
    std::vector<double> vecInvArea;
    std::vector<int> vecResultEqual1;
    int bIsPatternLearned;
    int iBorderColor;
    void clear()
    {
        std::vector<cv::Mat>().swap(vecPyramid);
        std::vector<double>().swap(vecTemplNorm);
        std::vector<double>().swap(vecInvArea);
        std::vector<cv::Scalar>().swap(vecTemplMean);
       // cv::dnn::dnn4_v20210301::MatShape::vector<int>().swap(vecResultEqual1);
    }
    void resize(int searchAngle)
    {
        vecTemplMean.resize(searchAngle);
        vecTemplNorm.resize(searchAngle, 0);
        vecInvArea.resize(searchAngle, 1);
        vecResultEqual1.resize(searchAngle, 0);
    }
    s_TemplData()
    {
        bIsPatternLearned = 0;
    }
};
struct s_MatchParameter
{
    cv::Point2d point;
    double dMatchScore;
    double dMatchAngle;
    // Mat matRotatedSourceImage;
    cv::Rect rectRoi;
    double dAngleStart;
    double dAngleEnd;
    cv::RotatedRect rectR;
    cv::Rect rectBounding;
    int bDelete;

    double vecResult[3][3]; // for subpixel
    int iMaxScoreIndex;     // for subpixel
    int bPosOnBorder;
    cv::Point2d ptSubPixel;
    double dNewAngle;

    s_MatchParameter(cv::Point2f ptMinMax, double dScore, double dAngle) //, Mat matRotatedSourceImage = Mat ())
    {
        point = ptMinMax;
        dMatchScore = dScore;
        dMatchAngle = dAngle;

        bDelete = 0;
        dNewAngle = 0.0;

        bPosOnBorder = 0;
    }
    s_MatchParameter()
    {
        double dMatchScore = 0;
        double dMatchAngle = 0;
    }
    ~s_MatchParameter()
    {
    }
};
struct s_BlockMax
{
    struct Block
    {
        cv::Rect rect;
        double dMax;
        cv::Point ptMaxLoc;
        Block()
        {
        }
        Block(cv::Rect rect_, double dMax_, cv::Point ptMaxLoc_)
        {
            rect = rect_;
            dMax = dMax_;
            ptMaxLoc = ptMaxLoc_;
        }
    };
    s_BlockMax()
    {
    }
    std::vector<Block> vecBlock;
    cv::Mat matSourceImage;
    s_BlockMax(cv::Mat matSourceImage_, cv::Size sizeTemplate)
    {
        matSourceImage = matSourceImage_;
        // Split mat SourceImage into several blocks and calculate the maximum value separately
        int iBlockW = sizeTemplate.width * 2;
        int iBlockH = sizeTemplate.height * 2;

        int iCol = matSourceImage.cols / iBlockW;
        int bHResidue = matSourceImage.cols % iBlockW != 0;

        int iRow = matSourceImage.rows / iBlockH;
        int bVResidue = matSourceImage.rows % iBlockH != 0;

        if (iCol == 0 || iRow == 0)
        {
            vecBlock.clear();
            return;
        }

        vecBlock.resize(iCol * iRow);
        int iCount = 0;
        for (int y = 0; y < iRow; y++)
        {
            for (int x = 0; x < iCol; x++)
            {
                cv::Rect rectBlock(x * iBlockW, y * iBlockH, iBlockW, iBlockH);
                vecBlock[iCount].rect = rectBlock;
                cv::minMaxLoc(matSourceImage(rectBlock), 0, &vecBlock[iCount].dMax, 0, &vecBlock[iCount].ptMaxLoc);
                vecBlock[iCount].ptMaxLoc += rectBlock.tl();
                iCount++;
            }
        }
        if (bHResidue && bVResidue)
        {
            cv::Rect rectRight(iCol * iBlockW, 0, matSourceImage.cols - iCol * iBlockW, matSourceImage.rows);
            Block blockRight;
            blockRight.rect = rectRight;
            cv::minMaxLoc(matSourceImage(rectRight), 0, &blockRight.dMax, 0, &blockRight.ptMaxLoc);
            blockRight.ptMaxLoc += rectRight.tl();
            vecBlock.push_back(blockRight);

            cv::Rect rectBottom(0, iRow * iBlockH, iCol * iBlockW, matSourceImage.rows - iRow * iBlockH);
            Block blockBottom;
            blockBottom.rect = rectBottom;
            cv::minMaxLoc(matSourceImage(rectBottom), 0, &blockBottom.dMax, 0, &blockBottom.ptMaxLoc);
            blockBottom.ptMaxLoc += rectBottom.tl();
            vecBlock.push_back(blockBottom);
        }
        else if (bHResidue)
        {
            cv::Rect rectRight(iCol * iBlockW, 0, matSourceImage.cols - iCol * iBlockW, matSourceImage.rows);
            Block blockRight;
            blockRight.rect = rectRight;
            cv::minMaxLoc(matSourceImage(rectRight), 0, &blockRight.dMax, 0, &blockRight.ptMaxLoc);
            blockRight.ptMaxLoc += rectRight.tl();
            vecBlock.push_back(blockRight);
        }
        else
        {
            cv::Rect rectBottom(0, iRow * iBlockH, matSourceImage.cols, matSourceImage.rows - iRow * iBlockH);
            Block blockBottom;
            blockBottom.rect = rectBottom;
            cv::minMaxLoc(matSourceImage(rectBottom), 0, &blockBottom.dMax, 0, &blockBottom.ptMaxLoc);
            blockBottom.ptMaxLoc += rectBottom.tl();
            vecBlock.push_back(blockBottom);
        }
    }
    void UpdateMax(cv::Rect rectIgnore)
    {
        if (vecBlock.size() == 0)
            return;
        // Find all blocks that intersect with rect Ignore
        int searchAngle = vecBlock.size();
        for (int i = 0; i < searchAngle; i++)
        {
            cv::Rect rectIntersec = rectIgnore & vecBlock[i].rect;
            // No intersection
            if (rectIntersec.width == 0 && rectIntersec.height == 0)
                continue;
            // There are intersections, update extreme and extreme positions
            cv::minMaxLoc(matSourceImage(vecBlock[i].rect), 0, &vecBlock[i].dMax, 0, &vecBlock[i].ptMaxLoc);
            vecBlock[i].ptMaxLoc += vecBlock[i].rect.tl();
        }
    }
    void GetMaxValueLoc(double &dMax, cv::Point &ptMaxLoc)
    {
        int searchAngle = vecBlock.size();
        if (searchAngle == 0)
        {
            cv::minMaxLoc(matSourceImage, 0, &dMax, 0, &ptMaxLoc);
            return;
        }
        // Find the maximum value from block
        int iIndex = 0;
        dMax = vecBlock[0].dMax;
        for (int i = 1; i < searchAngle; i++)
        {
            if (vecBlock[i].dMax >= dMax)
            {
                iIndex = i;
                dMax = vecBlock[i].dMax;
            }
        }
        ptMaxLoc = vecBlock[iIndex].ptMaxLoc;
    }
};

struct s_SingleTargetMatch
{
    cv::Point2d ptLT, ptRT, ptRB, ptLB, sourceImageCenterPoint;
    double dMatchedAngle;
    double dMatchScore;
};

enum ReturnMatchedSignal
{
    MTF_WIDGET,
    VISION_PLUGIN,
    NONE
};

class visionAlgorithm : public QThread
{
    Q_OBJECT

  protected:
    static visionAlgorithm *instancePtr;

  public:
    visionAlgorithm(QObject *parent = nullptr);
    ~visionAlgorithm();
    visionAlgorithm(const visionAlgorithm &obj) = delete;
    double MatchScore;
    int MatchTarget;
    double MatchAngle;
    double ScaleFactor;
    ReturnMatchedSignal Mode = NONE;
    cv::Mat SourceImage;
    cv::Mat DestinationImage;
    std::vector<s_SingleTargetMatch> ResultData;
    void Match();
    static visionAlgorithm *GetInstance()
    {
        if (instancePtr == nullptr)
        {
            instancePtr = new visionAlgorithm();
        }
        return instancePtr;
    }

  signals:
    void drawResult();
    void drawResultMTF();

  private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    const double m_scaleStep = 0.1;
    double m_dMaxOverlap = 0;
    int m_iMinReduceArea = 256;
    s_TemplData m_TemplateData;
    cv::Mat m_destinationImageClone;
    int getTopLayer(cv::Mat *matTempl, int iMinDestinationImageLength);
    cv::Size getBestRotationSize(cv::Size sizeSourceImage, cv::Size sizeDestinationImage,
                                 double destinationRotateAngle);
    cv::Point2f ptRotatePt2f(cv::Point2f ptInput, cv::Point2f ptOrg, double dAngle);
    void matchTemplate(cv::Mat &matSourceImage, s_TemplData *pointerTemplateData, cv::Mat &matResult, int iLayer);
    void coefficientDenominator(cv::Mat &matSourceImage, s_TemplData *pointerTemplateData, cv::Mat &matResult,
                                int iLayer);
    cv::Point getNextMaxLoc(cv::Mat &matResult, cv::Point ptMaxLoc, cv::Size sizeTemplate, double &dMaxValue,
                            double dMaxOverlap);
    cv::Point getNextMaxLoc(cv::Mat &matResult, cv::Point ptMaxLoc, cv::Size sizeTemplate, double &dMaxValue,
                            double dMaxOverlap, s_BlockMax &blockMax);
    void getRotatedROI(cv::Mat &matSourceImage, cv::Size size, cv::Point2f ptLT, double dAngle, cv::Mat &matROI);
    void filterWithScore(std::vector<s_MatchParameter> *vec, double dScore);
    void filterWithRotatedRect(std::vector<s_MatchParameter> *vec, int iMethod = 5, double dMaxOverLap = 0);
    void sortPtWithCenter(std::vector<cv::Point2f> &vecSort);
    void learnPattern();
    cv::Mat transformAndNormalize(cv::Mat mat);
};
#endif