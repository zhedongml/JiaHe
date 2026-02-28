#include "visionAlgorithm.h"

#define VISION_TOLERANCE 0.0000001
#define DEGREETORADIAN (CV_PI / 180.0)
#define RADIANTODEGREE (180.0 / CV_PI)
#define MATCH_CANDIDATE_NUM 5

const cv::Scalar colorGreen(0, 255, 0);

visionAlgorithm *visionAlgorithm ::instancePtr = nullptr;

visionAlgorithm::visionAlgorithm(QObject *parent) : QThread(parent)
{
}

visionAlgorithm::~visionAlgorithm()
{
    m_mutex.lock();
    m_condition.wakeOne();
    m_mutex.unlock();
    wait();
}

bool compareScoreBig2Small(const s_MatchParameter &lhs, const s_MatchParameter &rhs)
{
    return lhs.dMatchScore > rhs.dMatchScore;
}

cv::Mat visionAlgorithm::transformAndNormalize(cv::Mat mat)
{
    if (mat.depth() != CV_8U)
    {
        double maxVal = 0;
        cv::minMaxLoc(mat, 0, &maxVal, NULL, NULL);
        if (maxVal > 256)
        {
            cv::normalize(mat, mat, 0, 255, cv::NORM_MINMAX);
        }
        mat.convertTo(mat, CV_8U);
    }
    if (mat.channels() != 1)
    {
        cv::cvtColor(mat, mat, CV_BGR2GRAY);
    }
    return mat;
}

void visionAlgorithm::Match()
{
    if (DestinationImage.empty() || SourceImage.empty())
    {
        return;
    }

    DestinationImage = transformAndNormalize(DestinationImage);
    SourceImage = transformAndNormalize(SourceImage);

    m_destinationImageClone = DestinationImage.clone();
    // 1st matching scaling search
    for (double downScale = 1; downScale >= (1 - ScaleFactor); downScale -= m_scaleStep)
    {
        cv::resize(m_destinationImageClone, m_destinationImageClone, cv::Size(), downScale, downScale);
        if (m_destinationImageClone.cols <= 100 && ScaleFactor != 0)
        {
            break;
        }
        // Number of destination image pyramid layer index start from 0 (total layer = pyramidLayer + 1)
        int pyramidLayer =
            getTopLayer(&m_destinationImageClone, static_cast<int>(sqrt(static_cast<double>(m_iMinReduceArea))));
        // Initial a pyramid
        std::vector<cv::Mat> vecMatSourceImagePyramid;

        cv::buildPyramid(SourceImage, vecMatSourceImagePyramid, pyramidLayer);

        learnPattern();

        s_TemplData *pointerTemplateData = &m_TemplateData;

        // In the first stage, the general angle and ROI were found with the top level (smallest)
        double destinationImageAngleStep =
            std::atan(2.0 / std::max(pointerTemplateData->vecPyramid[pyramidLayer].cols,
                                     pointerTemplateData->vecPyramid[pyramidLayer].rows)) *
            RADIANTODEGREE;

        std::vector<double> vecAngles;

        if (MatchAngle < VISION_TOLERANCE)
        {
            vecAngles.push_back(0.0);
        }
        else
        {
            // adds a set of positive angle tolerance
            for (double dAngle = 0; dAngle < MatchAngle + destinationImageAngleStep;
                 dAngle += destinationImageAngleStep)
            {
                vecAngles.push_back(dAngle);
            }
            // adds a set of negative angle tolerance
            for (double dAngle = -destinationImageAngleStep; dAngle > -MatchAngle - destinationImageAngleStep;
                 dAngle -= destinationImageAngleStep)
            {
                vecAngles.push_back(dAngle);
            }
        }

        cv::Point2f sourceImageCenterPoint((vecMatSourceImagePyramid[pyramidLayer].cols - 1) / 2.0f,
                                           (vecMatSourceImagePyramid[pyramidLayer].rows - 1) / 2.0f);
        int searchAngle = static_cast<int>(vecAngles.size());
        std::vector<s_MatchParameter> vecMatchParameter;
        // Caculate lowest score at every layer
        std::vector<double> vecLayerScore(pyramidLayer + 1, MatchScore);
        for (int iLayer = 1; iLayer <= pyramidLayer; iLayer++)
        {
            vecLayerScore[iLayer] = vecLayerScore[iLayer - 1] * 0.9;
        }
        bool bCalMaxByBlock = (vecMatSourceImagePyramid[pyramidLayer].size().area() /
                                   pointerTemplateData->vecPyramid[pyramidLayer].size().area() >
                               500) &&
                              MatchTarget > 10;
        // 1st matching angle search
        for (int i = 0; i < searchAngle; i++)
        {
            cv::Mat matRotatedSourceImage, matR = cv::getRotationMatrix2D(sourceImageCenterPoint, vecAngles[i], 1);
            cv::Mat matResult;
            cv::Point ptMaxLoc;
            double dValue, dMaxVal;
            double dRotate = std::clock();
            cv::Size sizeBest = getBestRotationSize(vecMatSourceImagePyramid[pyramidLayer].size(),
                                                    pointerTemplateData->vecPyramid[pyramidLayer].size(), vecAngles[i]);
            float fTranslationX = (sizeBest.width - 1) / 2.0f - sourceImageCenterPoint.x;
            float fTranslationY = (sizeBest.height - 1) / 2.0f - sourceImageCenterPoint.y;
            matR.at<double>(0, 2) += fTranslationX;
            matR.at<double>(1, 2) += fTranslationY;

            // rotate smallest pyramid of source image
            cv::warpAffine(vecMatSourceImagePyramid[pyramidLayer], matRotatedSourceImage, matR, sizeBest,
                           cv::InterpolationFlags::INTER_LINEAR, cv::BorderTypes::BORDER_CONSTANT,
                           cv::Scalar(pointerTemplateData->iBorderColor));

            // matching with dst image
            matchTemplate(matRotatedSourceImage, pointerTemplateData, matResult, pyramidLayer);

            // case of top big search area and lot of target number -> do rough skipping with block object
            if (bCalMaxByBlock)
            {
                s_BlockMax blockMax(matResult, pointerTemplateData->vecPyramid[pyramidLayer].size());
                blockMax.GetMaxValueLoc(dMaxVal, ptMaxLoc);

                // If max score of all found object in this layer/angle -> skip this angle loop
                if (dMaxVal < vecLayerScore[pyramidLayer])
                {
                    continue;
                }
                // Add to result match vector
                vecMatchParameter.push_back(s_MatchParameter(
                    cv::Point2f(ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dMaxVal, vecAngles[i]));
                // Find all match object and add
                for (int j = 0; j < MatchTarget + MATCH_CANDIDATE_NUM - 1; j++)
                {
                    // Get top left position of the found object
                    ptMaxLoc = getNextMaxLoc(matResult, ptMaxLoc, pointerTemplateData->vecPyramid[pyramidLayer].size(),
                                             dValue, m_dMaxOverlap, blockMax);
                    if (dMaxVal < vecLayerScore[pyramidLayer])
                    {
                        continue;
                    }
                    // Add this angle match param
                    vecMatchParameter.push_back(s_MatchParameter(
                        cv::Point2f(ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dValue, vecAngles[i]));
                }
            }
            else
            {
                cv::minMaxLoc(matResult, nullptr, &dMaxVal, nullptr, &ptMaxLoc);
                if (dMaxVal < vecLayerScore[pyramidLayer])
                {
                    continue;
                }
                vecMatchParameter.push_back(s_MatchParameter(
                    cv::Point2f(ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dMaxVal, vecAngles[i]));

                for (int j = 0; j < MatchTarget + MATCH_CANDIDATE_NUM - 1; j++)
                {
                    // get location to next object
                    ptMaxLoc = getNextMaxLoc(matResult, ptMaxLoc, pointerTemplateData->vecPyramid[pyramidLayer].size(),
                                             dValue, m_dMaxOverlap);
                    if (dMaxVal < vecLayerScore[pyramidLayer])
                    {
                        continue;
                    }
                    vecMatchParameter.push_back(s_MatchParameter(
                        cv::Point2f(ptMaxLoc.x - fTranslationX, ptMaxLoc.y - fTranslationY), dValue, vecAngles[i]));
                }
            }
        }

        std::sort(vecMatchParameter.begin(), vecMatchParameter.end(), compareScoreBig2Small);

        int iMatchSize = static_cast<int>(vecMatchParameter.size());
        int iDestinationImageW = pointerTemplateData->vecPyramid[pyramidLayer].cols,
            iDestinationImageH = pointerTemplateData->vecPyramid[pyramidLayer].rows;

        std::vector<s_MatchParameter> vecAllResult;
        // For each object found from the first matching
        for (int i = 0; i < static_cast<int>(vecMatchParameter.size()); i++)
        {
            double destinationRotateAngle = -vecMatchParameter[i].dMatchAngle * DEGREETORADIAN;
            cv::Point2f ptLT = ptRotatePt2f(vecMatchParameter[i].point, sourceImageCenterPoint, destinationRotateAngle);
            double destinationImageAngleStep =
                std::atan(2.0 / std::max(iDestinationImageW, iDestinationImageH)) * RADIANTODEGREE; // change min to max
            vecMatchParameter[i].dAngleStart = vecMatchParameter[i].dMatchAngle - destinationImageAngleStep;
            vecMatchParameter[i].dAngleEnd = vecMatchParameter[i].dMatchAngle + destinationImageAngleStep;
            if (pyramidLayer <= 0)
            {
                vecMatchParameter[i].point = cv::Point2d(ptLT * ((pyramidLayer == 0) ? 1 : 2));
                vecAllResult.push_back(vecMatchParameter[i]);
            }
            else
            {
                // Decend down to the last layer
                for (int iLayer = pyramidLayer - 1; iLayer >= 0; iLayer--)
                {
                    // search angle
                    destinationImageAngleStep =
                        std::atan(2.0 / std::max(pointerTemplateData->vecPyramid[iLayer].cols,
                                                 pointerTemplateData->vecPyramid[iLayer].rows)) *
                        RADIANTODEGREE; // change min to max
                    std::vector<double> vecAngles;
                    double dMatchedAngle = vecMatchParameter[i].dMatchAngle;

                    if (MatchAngle < VISION_TOLERANCE)
                    {
                        vecAngles.push_back(0.0);
                    }
                    else
                    {
                        for (int k = -1; k <= 1; k++)
                        {
                            vecAngles.push_back(dMatchedAngle + destinationImageAngleStep * k);
                        }
                    }

                    cv::Point2f ptSourceImageCenter((vecMatSourceImagePyramid[iLayer].cols - 1) / 2.0f,
                                                    (vecMatSourceImagePyramid[iLayer].rows - 1) / 2.0f);
                    searchAngle = static_cast<int>(vecAngles.size());

                    std::vector<s_MatchParameter> vecNewMatchParameter(searchAngle);
                    int iMaxScoreIndex = 0;
                    double dBigValue = -1;
                    for (int j = 0; j < searchAngle; j++)
                    {
                        cv::Mat matResult, matRotatedSourceImage;
                        double dMaxValue = 0;
                        cv::Point ptMaxLoc;
                        getRotatedROI(vecMatSourceImagePyramid[iLayer], pointerTemplateData->vecPyramid[iLayer].size(),
                                      ptLT * 2, vecAngles[j], matRotatedSourceImage);
                        matchTemplate(matRotatedSourceImage, pointerTemplateData, matResult, iLayer);
                        cv::minMaxLoc(matResult, nullptr, &dMaxValue, nullptr, &ptMaxLoc);
                        vecNewMatchParameter[j] = s_MatchParameter(ptMaxLoc, dMaxValue, vecAngles[j]);

                        if (vecNewMatchParameter[j].dMatchScore > dBigValue)
                        {
                            iMaxScoreIndex = j;
                            dBigValue = vecNewMatchParameter[j].dMatchScore;
                        }
                    }
                    if (vecNewMatchParameter[iMaxScoreIndex].dMatchScore < vecLayerScore[iLayer])
                    {
                        break;
                    }
                    double dNewMatchAngle = vecNewMatchParameter[iMaxScoreIndex].dMatchAngle;
                    // Let the coordinate system return to (0, 0) when rotating (getRotatedROI)
                    cv::Point2f ptPaddingLT =
                        ptRotatePt2f(ptLT * 2, ptSourceImageCenter, dNewMatchAngle * DEGREETORADIAN) -
                        cv::Point2f(3, 3);
                    cv::Point2f point(vecNewMatchParameter[iMaxScoreIndex].point.x + ptPaddingLT.x,
                                      vecNewMatchParameter[iMaxScoreIndex].point.y + ptPaddingLT.y);
                    // Re-spin
                    point = ptRotatePt2f(point, ptSourceImageCenter, -dNewMatchAngle * DEGREETORADIAN);
                    if (iLayer == 0)
                    {
                        // vecNewMatchParameter[iMaxScoreIndex].point = point * (0 == 0 ? 1 : 2);
                        vecNewMatchParameter[iMaxScoreIndex].point = point;
                        vecAllResult.push_back(vecNewMatchParameter[iMaxScoreIndex]);
                    }
                    else
                    {
                        // Update MatchAngle ptLT
                        vecMatchParameter[i].dMatchAngle = dNewMatchAngle;
                        vecMatchParameter[i].dAngleStart =
                            vecMatchParameter[i].dMatchAngle - destinationImageAngleStep / 2;
                        vecMatchParameter[i].dAngleEnd =
                            vecMatchParameter[i].dMatchAngle + destinationImageAngleStep / 2;
                        ptLT = point;
                    }
                }
            }
        }
        filterWithScore(&vecAllResult, MatchScore);

        // Finally filter out overlapping
        iDestinationImageW = pointerTemplateData->vecPyramid[0].cols,
        iDestinationImageH = pointerTemplateData->vecPyramid[0].rows;

        for (int i = 0; i < static_cast<int>(vecAllResult.size()); i++)
        {
            cv::Point2f ptLT, ptRT, ptRB, ptLB;
            double destinationRotateAngle = -vecAllResult[i].dMatchAngle * DEGREETORADIAN;
            ptLT = vecAllResult[i].point;
            ptRT = cv::Point2f(ptLT.x + iDestinationImageW * static_cast<float>(std::cos(destinationRotateAngle)),
                               ptLT.y - iDestinationImageW * static_cast<float>(std::sin(destinationRotateAngle)));
            ptLB = cv::Point2f(ptLT.x + iDestinationImageH * static_cast<float>(std::sin(destinationRotateAngle)),
                               ptLT.y + iDestinationImageH * static_cast<float>(std::cos(destinationRotateAngle)));
            ptRB = cv::Point2f(ptRT.x + iDestinationImageH * static_cast<float>(std::sin(destinationRotateAngle)),
                               ptRT.y + iDestinationImageH * static_cast<float>(std::cos(destinationRotateAngle)));
            // record rotation rectangle
            auto ptRectCenter =
                cv::Point2f((ptLT.x + ptRT.x + ptLB.x + ptRB.x) / 4.0f, (ptLT.y + ptRT.y + ptLB.y + ptRB.y) / 4.0f);
            vecAllResult[i].rectR = cv::RotatedRect(ptRectCenter, pointerTemplateData->vecPyramid[0].size(),
                                                    static_cast<float>(vecAllResult[i].dMatchAngle));
        }

        filterWithRotatedRect(&vecAllResult, 5, m_dMaxOverlap);
        // Finally filter out overlapping

        // Sort by score
        std::sort(vecAllResult.begin(), vecAllResult.end(), compareScoreBig2Small);

        ResultData.clear();
        iMatchSize = static_cast<int>(vecAllResult.size());

        int iWidth = pointerTemplateData->vecPyramid[0].cols, iHeight = pointerTemplateData->vecPyramid[0].rows;

        for (int i = 0; i < iMatchSize; i++)
        {
            s_SingleTargetMatch sstm;
            double destinationRotateAngle = -vecAllResult[i].dMatchAngle * DEGREETORADIAN;

            sstm.ptLT = vecAllResult[i].point;

            sstm.ptRT = cv::Point2d(sstm.ptLT.x + iWidth * std::cos(destinationRotateAngle),
                                    sstm.ptLT.y - iWidth * std::sin(destinationRotateAngle));
            sstm.ptLB = cv::Point2d(sstm.ptLT.x + iHeight * std::sin(destinationRotateAngle),
                                    sstm.ptLT.y + iHeight * std::cos(destinationRotateAngle));
            sstm.ptRB = cv::Point2d(sstm.ptRT.x + iHeight * std::sin(destinationRotateAngle),
                                    sstm.ptRT.y + iHeight * std::cos(destinationRotateAngle));
            sstm.sourceImageCenterPoint = cv::Point2d((sstm.ptLT.x + sstm.ptRT.x + sstm.ptRB.x + sstm.ptLB.x) / 4,
                                                      (sstm.ptLT.y + sstm.ptRT.y + sstm.ptRB.y + sstm.ptLB.y) / 4);
            sstm.dMatchedAngle = -vecAllResult[i].dMatchAngle;
            sstm.dMatchScore = vecAllResult[i].dMatchScore;

            if (sstm.dMatchedAngle < -180)
            {
                sstm.dMatchedAngle += 360;
            }
            if (sstm.dMatchedAngle > 180)
            {
                sstm.dMatchedAngle -= 360;
            }
            ResultData.push_back(sstm);

            // Deposit MATCH ROI
            if (i + 1 == MatchTarget)
            {
                break;
            }
        }
    }
    switch (Mode)
    {
    case MTF_WIDGET:
        emit drawResultMTF();
        break;
    case VISION_PLUGIN:
        emit drawResult();
        break;
    case NONE:
        break;
    default:
        break;
    }
}

int visionAlgorithm::getTopLayer(cv::Mat *matTempl, int iMinDestinationImageLength)
{
    int pyramidLayer = 0;
    int iMinReduceArea = iMinDestinationImageLength * iMinDestinationImageLength;
    int iArea = matTempl->cols * matTempl->rows;
    while (iArea > iMinReduceArea)
    {
        iArea /= 4;
        pyramidLayer++;
    }
    return pyramidLayer;
}

cv::Size visionAlgorithm::getBestRotationSize(cv::Size sizeSourceImage, cv::Size sizeDestinationImage,
                                              double destinationRotateAngle)
{
    double destinationRotateRadian = destinationRotateAngle * DEGREETORADIAN;
    cv::Point ptLT(0, 0), ptLB(0, sizeSourceImage.height - 1),
        ptRB(sizeSourceImage.width - 1, sizeSourceImage.height - 1), ptRT(sizeSourceImage.width - 1, 0);
    cv::Point2f sourceImageCenterPoint((sizeSourceImage.width - 1) / 2.0f, (sizeSourceImage.height - 1) / 2.0f);
    cv::Point2f ptLT_R = ptRotatePt2f(cv::Point2f(ptLT), sourceImageCenterPoint, destinationRotateRadian);
    cv::Point2f ptLB_R = ptRotatePt2f(cv::Point2f(ptLB), sourceImageCenterPoint, destinationRotateRadian);
    cv::Point2f ptRB_R = ptRotatePt2f(cv::Point2f(ptRB), sourceImageCenterPoint, destinationRotateRadian);
    cv::Point2f ptRT_R = ptRotatePt2f(cv::Point2f(ptRT), sourceImageCenterPoint, destinationRotateRadian);

    float fTopY = std::max(std::max(ptLT_R.y, ptLB_R.y), std::max(ptRB_R.y, ptRT_R.y));
    float fBottomY = std::min(std::min(ptLT_R.y, ptLB_R.y), std::min(ptRB_R.y, ptRT_R.y));
    float fRightX = std::max(std::max(ptLT_R.x, ptLB_R.x), std::max(ptRB_R.x, ptRT_R.x));
    float fLeftX = std::min(std::min(ptLT_R.x, ptLB_R.x), std::min(ptRB_R.x, ptRT_R.x));

    if (destinationRotateAngle > 360)
    {
        destinationRotateAngle -= 360;
    }
    else if (destinationRotateAngle < 0)
    {
        destinationRotateAngle += 360;
    }

    if (std::fabs(std::fabs(destinationRotateAngle) - 90) < VISION_TOLERANCE ||
        std::fabs(std::fabs(destinationRotateAngle) - 270) < VISION_TOLERANCE)
    {
        return cv::Size(sizeSourceImage.height, sizeSourceImage.width);
    }
    if (std::fabs(destinationRotateAngle) < VISION_TOLERANCE ||
        std::fabs(std::fabs(destinationRotateAngle) - 180) < VISION_TOLERANCE)
    {
        return sizeSourceImage;
    }

    double dAngle = destinationRotateAngle;

    if (dAngle > 0 && dAngle < 90)
    {
    }
    else if (dAngle > 90 && dAngle < 180)
    {
        dAngle -= 90;
    }
    else if (dAngle > 180 && dAngle < 270)
    {
        dAngle -= 180;
    }
    else if (dAngle > 270 && dAngle < 360)
    {
        dAngle -= 270;
    }

    float fH1 = sizeDestinationImage.width * std::sin(dAngle * DEGREETORADIAN) * std::cos(dAngle * DEGREETORADIAN);
    float fH2 = sizeDestinationImage.height * std::sin(dAngle * DEGREETORADIAN) * std::cos(dAngle * DEGREETORADIAN);

    int iHalfHeight = static_cast<int>(std::ceil(fTopY - sourceImageCenterPoint.y - fH1));
    int iHalfWidth = static_cast<int>(std::ceil(fRightX - sourceImageCenterPoint.x - fH2));

    cv::Size sizeRet(iHalfWidth * 2, iHalfHeight * 2);

    bool bWrongSize = (sizeDestinationImage.width < sizeRet.width && sizeDestinationImage.height > sizeRet.height) ||
                      (sizeDestinationImage.width > sizeRet.width && sizeDestinationImage.height < sizeRet.height ||
                       sizeDestinationImage.area() > sizeRet.area());
    if (bWrongSize)
    {
        sizeRet = cv::Size(static_cast<int>(fRightX - fLeftX + 0.5), static_cast<int>(fTopY - fBottomY + 0.5));
    }

    return sizeRet;
}

cv::Point2f visionAlgorithm::ptRotatePt2f(cv::Point2f ptInput, cv::Point2f ptOrg, double dAngle)
{
    double dWidth = ptOrg.x * 2;
    double dHeight = ptOrg.y * 2;
    double dY1 = dHeight - ptInput.y, dY2 = dHeight - ptOrg.y;

    double dX = (ptInput.x - ptOrg.x) * std::cos(dAngle) - (dY1 - ptOrg.y) * std::sin(dAngle) + ptOrg.x;
    double dY = (ptInput.x - ptOrg.x) * std::sin(dAngle) + (dY1 - ptOrg.y) * std::cos(dAngle) + dY2;

    dY = -dY + dHeight;
    return cv::Point2f(static_cast<float>(dX), static_cast<float>(dY));
}

void visionAlgorithm::matchTemplate(cv::Mat &matSourceImage, s_TemplData *pointerTemplateData, cv::Mat &matResult,
                                    int iLayer)
{
    cv::matchTemplate(matSourceImage, pointerTemplateData->vecPyramid[iLayer], matResult, 2);
    coefficientDenominator(matSourceImage, pointerTemplateData, matResult, iLayer);
}

void visionAlgorithm::coefficientDenominator(cv::Mat &matSourceImage, s_TemplData *pointerTemplateData,
                                             cv::Mat &matResult, int iLayer)
{
    if (pointerTemplateData->vecResultEqual1[iLayer])
    {
        matResult = cv::Scalar::all(1);
        return;
    }
    double *q0 = nullptr, *q1 = nullptr, *q2 = nullptr, *q3 = nullptr;

    cv::Mat sum, sqsum;
    cv::integral(matSourceImage, sum, sqsum, CV_64F);

    q0 = (double *)sqsum.data;
    q1 = q0 + pointerTemplateData->vecPyramid[iLayer].cols;
    q2 = (double *)(sqsum.data + pointerTemplateData->vecPyramid[iLayer].rows * sqsum.step);
    q3 = q2 + pointerTemplateData->vecPyramid[iLayer].cols;

    auto p0 = (double *)sum.data;
    double *p1 = p0 + pointerTemplateData->vecPyramid[iLayer].cols;
    auto p2 = (double *)(sum.data + pointerTemplateData->vecPyramid[iLayer].rows * sum.step);
    double *p3 = p2 + pointerTemplateData->vecPyramid[iLayer].cols;

    int sumStep = sum.data ? static_cast<int>(sum.step / sizeof(double)) : 0;
    int sqStep = sqsum.data ? static_cast<int>(sqsum.step / sizeof(double)) : 0;

    //
    double dTemplMean0 = pointerTemplateData->vecTemplMean[iLayer][0];
    double dTemplNorm = pointerTemplateData->vecTemplNorm[iLayer];
    double dInvArea = pointerTemplateData->vecInvArea[iLayer];
    //

    for (int i = 0; i < matResult.rows; i++)
    {
        float *rrow = matResult.ptr<float>(i);
        int idx = i * sumStep;
        int idx2 = i * sqStep;

        for (int j = 0; j < matResult.cols; j += 1, idx += 1, idx2 += 1)
        {
            double num = rrow[j], t;
            double wndMean2 = 0, wndSum2 = 0;

            t = p0[idx] - p1[idx] - p2[idx] + p3[idx];
            wndMean2 += t * t;
            num -= t * dTemplMean0;
            wndMean2 *= dInvArea;

            t = q0[idx2] - q1[idx2] - q2[idx2] + q3[idx2];
            wndSum2 += t;

            // t = std::sqrt (MAX (wndSum2 - wndMean2, 0)) * dTemplNorm;
            double diff2 = MAX(wndSum2 - wndMean2, 0);
            if (diff2 <= std::min(0.5, 10 * FLT_EPSILON * wndSum2))
            {
                t = 0; // avoid rounding errors
            }
            else
            {
                t = std::sqrt(diff2) * dTemplNorm;
            }

            if (std::fabs(num) < t)
            {
                num /= t;
            }
            else if (std::fabs(num) < t * 1.125)
            {
                num = num > 0 ? 1 : -1;
            }
            else
            {
                num = 0;
            }

            rrow[j] = static_cast<float>(num);
        }
    }
}

cv::Point visionAlgorithm::getNextMaxLoc(cv::Mat &matResult, cv::Point ptMaxLoc, cv::Size sizeTemplate,
                                         double &dMaxValue, double dMaxOverlap)
{
    // The area that compares the overlapping proportion must be considered
    int iStartX = ptMaxLoc.x - sizeTemplate.width * (1 - dMaxOverlap);
    int iStartY = ptMaxLoc.y - sizeTemplate.height * (1 - dMaxOverlap);
    // Blackened
    cv::rectangle(matResult,
                  cv::Rect(iStartX, iStartY, 2 * sizeTemplate.width * (1 - dMaxOverlap),
                           2 * sizeTemplate.height * (1 - dMaxOverlap)),
                  cv::Scalar(-1), -1);
    // Get the next maximum value
    cv::Point ptNewMaxLoc;
    cv::minMaxLoc(matResult, nullptr, &dMaxValue, nullptr, &ptNewMaxLoc);
    return ptNewMaxLoc;
}

cv::Point visionAlgorithm::getNextMaxLoc(cv::Mat &matResult, cv::Point ptMaxLoc, cv::Size sizeTemplate,
                                         double &dMaxValue, double dMaxOverlap, s_BlockMax &blockMax)
{
    // The area that compares the overlapping proportion must be considered
    int iStartX = static_cast<int>(ptMaxLoc.x - sizeTemplate.width * (1 - dMaxOverlap));
    int iStartY = static_cast<int>(ptMaxLoc.y - sizeTemplate.height * (1 - dMaxOverlap));
    cv::Rect rectIgnore(iStartX, iStartY, 2 * sizeTemplate.width * (1 - dMaxOverlap),
                        2 * sizeTemplate.height * (1 - dMaxOverlap));
    // Blackened
    cv::rectangle(matResult, rectIgnore, cv::Scalar(-1), -1);
    blockMax.UpdateMax(rectIgnore);
    cv::Point ptReturn;
    blockMax.GetMaxValueLoc(dMaxValue, ptReturn);
    return ptReturn;
}

void visionAlgorithm::getRotatedROI(cv::Mat &matSourceImage, cv::Size size, cv::Point2f ptLT, double dAngle,
                                    cv::Mat &matROI)
{
    double dAngle_radian = dAngle * DEGREETORADIAN;
    cv::Point2f ptC((matSourceImage.cols - 1) / 2.0f, (matSourceImage.rows - 1) / 2.0f);
    cv::Point2f ptLT_rotate = ptRotatePt2f(ptLT, ptC, dAngle_radian);
    cv::Size sizePadding(size.width + 6, size.height + 6);

    cv::Mat rMat = getRotationMatrix2D(ptC, dAngle, 1);
    rMat.at<double>(0, 2) -= ptLT_rotate.x - 3;
    rMat.at<double>(1, 2) -= ptLT_rotate.y - 3;

    cv::warpAffine(matSourceImage, matROI, rMat, sizePadding);
}

void visionAlgorithm::filterWithScore(std::vector<s_MatchParameter> *vec, double dScore)
{
    std::sort(vec->begin(), vec->end(), compareScoreBig2Small);
    int searchAngle = vec->size(), iIndexDelete = searchAngle + 1;
    for (int i = 0; i < searchAngle; i++)
    {
        if ((*vec)[i].dMatchScore < dScore)
        {
            iIndexDelete = i;
            break;
        }
    }
    if (iIndexDelete == searchAngle + 1) // No element is smaller than dScore
    {
        return;
    }
    vec->erase(vec->begin() + iIndexDelete, vec->end());
}

void visionAlgorithm::filterWithRotatedRect(std::vector<s_MatchParameter> *vec, int iMethod, double dMaxOverLap)
{
    int iMatchSize = static_cast<int>(vec->size());
    cv::RotatedRect rect1, rect2;
    for (int i = 0; i < iMatchSize - 1; i++)
    {
        if (vec->at(i).bDelete)
        {
            continue;
        }
        for (int j = i + 1; j < iMatchSize; j++)
        {
            if (vec->at(j).bDelete)
            {
                continue;
            }
            rect1 = vec->at(i).rectR;
            rect2 = vec->at(j).rectR;
            std::vector<cv::Point2f> vecInterSec;
            int iInterSecType = rotatedRectangleIntersection(rect1, rect2, vecInterSec);
            if (iInterSecType == cv::RectanglesIntersectTypes::INTERSECT_NONE) // no intersection
            {
                continue;
            }
            if (iInterSecType == cv::RectanglesIntersectTypes::INTERSECT_FULL) // One rectangle wraps another
            {
                int iDeleteIndex;
                if (iMethod == 0)
                {
                    iDeleteIndex = (vec->at(i).dMatchScore <= vec->at(j).dMatchScore) ? j : i;
                }
                else
                {
                    iDeleteIndex = (vec->at(i).dMatchScore >= vec->at(j).dMatchScore) ? j : i;
                }
                vec->at(iDeleteIndex).bDelete = true;
            }
            else // Intersection > 0
            {
                if (vecInterSec.size() < 3) // One or two intersection
                {
                    continue;
                }
                int iDeleteIndex;
                // Seeking area to overlap proportions
                sortPtWithCenter(vecInterSec);
                double dArea = contourArea(vecInterSec);
                double dRatio = dArea / rect1.size.area();
                // If it is greater than the maximum overlapping ratio, the score is high
                if (dRatio > dMaxOverLap)
                {
                    if (iMethod == 0)
                    {
                        iDeleteIndex = (vec->at(i).dMatchScore <= vec->at(j).dMatchScore) ? j : i;
                    }
                    else
                    {
                        iDeleteIndex = (vec->at(i).dMatchScore >= vec->at(j).dMatchScore) ? j : i;
                    }
                    vec->at(iDeleteIndex).bDelete = true;
                }
            }
        }
    }
    std::vector<s_MatchParameter>::iterator it;
    for (it = vec->begin(); it != vec->end();)
    {
        if ((*it).bDelete)
        {
            it = vec->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool comparePtWithAngle(const std::pair<cv::Point2f, double> lhs, const std::pair<cv::Point2f, double> rhs)
{
    return lhs.second < rhs.second;
}

void visionAlgorithm::sortPtWithCenter(std::vector<cv::Point2f> &vecSort)
{
    int searchAngle = static_cast<int>(vecSort.size());
    cv::Point2f sourceImageCenterPoint;
    for (int i = 0; i < searchAngle; i++)
    {
        sourceImageCenterPoint += vecSort[i];
    }
    sourceImageCenterPoint /= searchAngle;

    cv::Point2f vecX(1, 0);

    std::vector<std::pair<cv::Point2f, double>> vecPtAngle(searchAngle);
    for (int i = 0; i < searchAngle; i++)
    {
        vecPtAngle[i].first = vecSort[i]; // point
        cv::Point2f vec1(vecSort[i].x - sourceImageCenterPoint.x, vecSort[i].y - sourceImageCenterPoint.y);
        float fNormVec1 = vec1.x * vec1.x + vec1.y * vec1.y;
        float fDot = vec1.x;

        if (vec1.y < 0) // If you click above the center
        {
            vecPtAngle[i].second = std::acos(fDot / fNormVec1) * RADIANTODEGREE;
        }
        else if (vec1.y > 0) // Below
        {
            vecPtAngle[i].second = 360 - std::acos(fDot / fNormVec1) * RADIANTODEGREE;
        }
        else // The point and the center in the same Y
        {
            if (vec1.x - sourceImageCenterPoint.x > 0)
            {
                vecPtAngle[i].second = 0;
            }
            else
            {
                vecPtAngle[i].second = 180;
            }
        }
    }
    std::sort(vecPtAngle.begin(), vecPtAngle.end(), comparePtWithAngle);
    for (int i = 0; i < searchAngle; i++)
    {
        vecSort[i] = vecPtAngle[i].first;
    }
}

void visionAlgorithm::learnPattern()
{
    m_TemplateData.clear();

    int pyramidLayer =
        getTopLayer(&m_destinationImageClone, static_cast<int>(sqrt(static_cast<double>(m_iMinReduceArea))));
    buildPyramid(m_destinationImageClone, m_TemplateData.vecPyramid, pyramidLayer);
    s_TemplData *templateData = &m_TemplateData;
    templateData->iBorderColor = cv::mean(m_destinationImageClone).val[0] < 128 ? 255 : 0;
    int searchAngle = templateData->vecPyramid.size();
    templateData->resize(searchAngle);

    for (int i = 0; i < searchAngle; i++)
    {
        double invArea =
            1. / (static_cast<double>(templateData->vecPyramid[i].rows) * templateData->vecPyramid[i].cols);
        cv::Scalar templMean, templSdv;
        double templNorm = 0, templSum2 = 0;

        cv::meanStdDev(templateData->vecPyramid[i], templMean, templSdv);
        templNorm = templSdv[0] * templSdv[0] + templSdv[1] * templSdv[1] + templSdv[2] * templSdv[2] +
                    templSdv[3] * templSdv[3];

        if (templNorm < DBL_EPSILON)
        {
            templateData->vecResultEqual1[i] = true;
        }
        templSum2 = templNorm + templMean[0] * templMean[0] + templMean[1] * templMean[1] +
                    templMean[2] * templMean[2] + templMean[3] * templMean[3];

        templSum2 /= invArea;
        templNorm = std::sqrt(templNorm);
        templNorm /= std::sqrt(invArea); // care of accuracy here

        templateData->vecInvArea[i] = invArea;
        templateData->vecTemplMean[i] = templMean;
        templateData->vecTemplNorm[i] = templNorm;
    }
    templateData->bIsPatternLearned = true;
}
