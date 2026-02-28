#include "ml_rigidTrans3D.h"
#include "LogPlus.h"
using namespace MLImageDetection;
MLImageDetection::MLRigidTrans3D::MLRigidTrans3D()
{
}

MLImageDetection::MLRigidTrans3D::~MLRigidTrans3D()
{
}
EyeboxACS MLImageDetection::MLRigidTrans3D::getEyboxLocation(FiducialBase *f, vector<cv::Point3f> eyebox)
{
    EyeboxACS re;
    //if (f->manualFlag)
    //{
    //    FiducialManual *fiducialManual = (FiducialManual*)f;
    //    re=getEyeboxLocationManual(fiducialManual, eyebox);
    //   
    //}
    //else
    //{
    //    FiducialAuto *fiducialAuto = (FiducialAuto*)f;
    //    re = getEyeboxLocationAuto(fiducialAuto, eyebox);
    //}
    vector<cv::Point3f> srcPoints, dstPoints;
    srcPoints = f->fiudcialWafer;
    dstPoints = f->fiducialACS;
    FiducialDetect fd;
    RigidTrans3D translation = getRigidTrans3D(srcPoints, dstPoints);
    vector<cv::Point3f> dst = getTranslationLoc(translation, eyebox);
    re.eyebox = dst; 
    re.eulerAngle = cv::Point3d(translation.eulerAngle.x(), translation.eulerAngle.y(), translation.eulerAngle.z());
    return re;
}

EyeboxACS MLImageDetection::MLRigidTrans3D::getEyeboxLocationManual(FiducialManual *f, vector<cv::Point3f> eyebox)
{
    EyeboxACS re;
    vector<cv::Point3f> srcPoints, dstPoints;
    srcPoints = f->fiudcialWafer;
    FiducialDetect fd;
    for (int i = 0; i < f->fiducialPixel.size(); i++)
    {
        cv::Point3f pt = getFiducialACSLoc(f->fiducialPixel[i], f->fiducialACS[i]);
        dstPoints.push_back(pt);
    }
    RigidTrans3D translation = getRigidTrans3D(srcPoints, dstPoints);
    if (translation.result.success)
    {
        vector<cv::Point3f> dst = getTranslationLoc(translation, eyebox);
        re.eyebox = dst;
    }
    else
    {
        re.result = translation.result;
    }

    return re;
}

EyeboxACS MLImageDetection::MLRigidTrans3D::getEyeboxLocationAuto(FiducialAuto *f, vector<cv::Point3f> eyebox)
{
    EyeboxACS re;
    vector<cv::Point3f> srcPoints, dstPoints;
    srcPoints = f->fiudcialWafer;
    FiducialDetect fd;
    for (int i = 0; i < f->fiducialImg.size(); i++)
    {
        vector<cv::Point2f> fiucialPixel = fd.FiducialCoordinate(f->fiducialImg[i], BMW);
        cv::Mat img_draw = fd.getImgDraw();
        re.img_draw.push_back(img_draw);

        // to do: to delete
        string name = "./config/ALGRE/" + to_string(i) + "_fid" + ".jpg";
        imwrite(name, img_draw);

        cv::Point3f pt = getFiducialACSLoc(fiucialPixel[0], f->fiducialACS[i]);
        dstPoints.push_back(pt);
    }
    RigidTrans3D translation = getRigidTrans3D(srcPoints, dstPoints);
    if (translation.result.success)
    {
        vector<cv::Point3f> dst = getTranslationLoc(translation, eyebox);
        re.eyebox = dst;
    }
    else
    {
        re.result = translation.result;
    }
    return re;
}


RigidTrans3D MLImageDetection::MLRigidTrans3D::getRigidTrans3D(std::vector<cv::Point3f> srcPoints,
                                                        std::vector<cv::Point3f> dstPoints)
{
    std::string str = "ALG Error-----[getRigidTrans3D]-----";
    RigidTrans3D trans;
    if (srcPoints.size() == dstPoints.size() && srcPoints.size()>=3)
    {
        cv::Point3f centerSrc, centerDst;
        centerSrc = centroid(srcPoints);
        centerDst = centroid(dstPoints);
        int pointsNum = srcPoints.size();
        cv::Mat srcMat(3, pointsNum, CV_32FC1);
        cv::Mat dstMat(3, pointsNum, CV_32FC1);
        float *srcDat = (float *)(srcMat.data);
        float *dstDat = (float *)(dstMat.data);
        for (int i = 0; i < pointsNum; ++i)
        {
            srcDat[i] = srcPoints[i].x - centerSrc.x;
            srcDat[pointsNum + i] = srcPoints[i].y - centerSrc.y;
            srcDat[pointsNum * 2 + i] = srcPoints[i].z - centerSrc.z;
            dstDat[i] = dstPoints[i].x - centerDst.x;
            dstDat[pointsNum + i] = dstPoints[i].y - centerDst.y;
            dstDat[pointsNum * 2 + i] = dstPoints[i].z - centerDst.z;
        }
        // SVD分解
        cv::Mat matS = srcMat * dstMat.t();
        cv::Mat matU, matW, matV;
        cv::SVDecomp(matS, matW, matU, matV);
        cv::Mat matTemp = matU * matV;
        double det = cv::determinant(matTemp);
        float datM[] = {1, 0, 0, 0, 1, 0, 0, 0, det};
        cv::Mat matM(3, 3, CV_32FC1, datM);
        cv::Mat matR = matV.t() * matM * matU.t();
        Eigen::Matrix3d R;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                R(i, j) = matR.at<float>(i, j);
            }
        }
        // cv::cv2eigen(matR, R);
        float X = centerDst.x - (centerSrc.x * R(0, 0) + centerSrc.y * R(0, 1) + centerSrc.z * R(0, 2));
        float Y = centerDst.y - (centerSrc.x * R(1, 0) + centerSrc.y * R(1, 1) + centerSrc.z * R(1, 2));
        float Z = centerDst.z - (centerSrc.x * R(2, 0) + centerSrc.y * R(2, 1) + centerSrc.z * R(2, 2));
        Eigen::Vector3d displacement = {X, Y, Z};
        Eigen::Vector3d eulerAngle = rotationMatrixToEulerAngles(R);
        eulerAngle = eulerAngle * 180 / CV_PI;
        trans.matR = matR;
        trans.R = R;
        trans.displacement = displacement;
        trans.eulerAngle = eulerAngle;
        //  cout << R << endl;
        //  cout << displacement << endl;
        //  cout << eulerAngle << endl;
    }
    else
    {
        string sMsg = "The srcPoints num is not equal to the dstPoints num!";
        string errorMessage = str + sMsg;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, errorMessage.c_str());
        //  throw invalid_argument(errorMessage);
        trans.result.success = false;
        trans.result.errorMsg = sMsg; 
    }
    return trans;
}

vector<cv::Point3f> MLImageDetection::MLRigidTrans3D::getTranslationLoc(RigidTrans3D trans, vector<cv::Point3f> src)
{
    vector<cv::Point3f> dst;
    for (int i = 0; i < src.size(); i++)
    {
        cv::Mat eyeMat = cv::Mat::zeros(cv::Size(1, 3), CV_32FC1);
        eyeMat.at<float>(0, 0) = src[i].x;
        eyeMat.at<float>(1, 0) = src[i].y;
        eyeMat.at<float>(2, 0) = src[i].z;
        cv::Mat s = trans.matR * eyeMat;
        cv::Point3f eyeAcs;
        eyeAcs.x = s.at<float>(0, 0) + trans.displacement[0];
        eyeAcs.y = s.at<float>(1, 0) + trans.displacement[1];
        eyeAcs.z = s.at<float>(2, 0) + trans.displacement[2];
        dst.push_back(eyeAcs);
    }
    return dst;
}

cv::Point3f MLImageDetection::MLRigidTrans3D::getFiducialACSLoc(cv::Point2f fiducialPixel, cv::Point3f fiducialACS)
{
    cv::Point3f pt;
    cv::Point2f center = cv::Point2f(m_size / 2);
    cv::Point2f dis;
    dis = (fiducialPixel - center) * PIXEL_SIZE / MAGNICICATION;
    pt.x = fiducialACS.x + dis.x;
    pt.y = fiducialACS.y + dis.y;
    pt.z = fiducialACS.z;
    return pt;
}

cv::Point3f MLImageDetection::MLRigidTrans3D::centroid(std::vector<cv::Point3f> pt)
{
    double srcSumX = 0.0f;
    double srcSumY = 0.0f;
    double srcSumZ = 0.0f;
    //计算质心
    int pointsNum = pt.size();
    for (int i = 0; i < pt.size(); ++i)
    {
        srcSumX += pt[i].x;
        srcSumY += pt[i].y;
        srcSumZ += pt[i].z;
    }
    cv::Point3f centerSrc;
    centerSrc.x = float(srcSumX / pointsNum);
    centerSrc.y = float(srcSumY / pointsNum);
    centerSrc.z = float(srcSumZ / pointsNum);
    return centerSrc;
}

Eigen::Vector3d MLImageDetection::MLRigidTrans3D::rotationMatrixToEulerAngles(Eigen::Matrix3d &R)
{

   // assert(isRotationMatirx(R));
    double sy = sqrt(R(0, 0) * R(0, 0) + R(1, 0) * R(1, 0));
    bool singular = sy < 1e-6;
    double x, y, z;
    if (!singular)
    {
        x = atan2(R(2, 1), R(2, 2));
        y = atan2(-R(2, 0), sy);
        z = atan2(R(1, 0), R(0, 0));
    }
    else
    {
        x = atan2(-R(1, 2), R(1, 1));
        y = atan2(-R(2, 0), sy);
        z = 0;
    }
    return {x, y, z};
}
