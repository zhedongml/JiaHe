#include "AxesTransfer2D.h"

AxesTransfer2D::AxesTransfer2D()
{
}

AxesTransfer2D::~AxesTransfer2D()
{
}

Result AxesTransfer2D::rigidTransform(RigidTrans2D& rigidTrans2D, const vector<cv::Point2f>& fiudcialWafer, const vector<cv::Point2f>& fiducialACS)
{
    if (fiudcialWafer.size() != 2 || fiducialACS.size() != 2) {
        return Result(false, "2D Rigid transform error, fiducial number is not 2.");;
    }

    rigidTrans2D = estimateRigidTransform(fiudcialWafer, fiducialACS);
    bool ret = judgeTransform(fiudcialWafer, fiducialACS, rigidTrans2D);
    if (!ret) {
        return Result(false, "2D Rigid transform error, please check fiducial coordinate information, they have no rigid transformation.");
    }

    return Result();
}

TransResult2D AxesTransfer2D::getEyeboxAcs(const FiducialBase2D& fiducialBase2D, const vector<cv::Point2f>& eyeboxWafer)
{
    TransResult2D transResult2D;
    if(fiducialBase2D.fiudcialWafer.size() != 2 || fiducialBase2D.fiducialACS.size() != 2){
        transResult2D.result = Result(false, "2D Rigid transform error, fiducial number is not 2.");
        return transResult2D;
    }

    if (eyeboxWafer.size() == 0) {
        transResult2D.result = Result(false, "2D Rigid transform error, eyebox wafer size is 0.");
        return transResult2D;
    }

    RigidTrans2D rigidTrans2D = estimateRigidTransform(fiducialBase2D.fiudcialWafer, fiducialBase2D.fiducialACS);
    bool ret = judgeTransform(fiducialBase2D.fiudcialWafer, fiducialBase2D.fiducialACS, rigidTrans2D);
    if(!ret){
        transResult2D.result = Result(false, "2D Rigid transform error, please check fiducial coordinate information, they have no rigid transformation.");
        return transResult2D;
    }
    
    vector<cv::Point2f> eyeboxAcsList = applyTransformList(eyeboxWafer, rigidTrans2D);

    transResult2D.eyeboxAcsList = eyeboxAcsList;
    transResult2D.angleDeg = rigidTrans2D.angleRad * 180.0f / CV_PI;
    return transResult2D;
}

cv::Point2f AxesTransfer2D::relativeAcs(const cv::Point2f& waferRelPos, const RigidTrans2D& rigidTrans2D)
{
    RigidTrans2D rigidTrans2DT = rigidTrans2D;
    rigidTrans2DT.translation = cv::Point2f(0,0);

    cv::Point2f relAcs = applyTransform(waferRelPos, rigidTrans2DT);
    return relAcs;
}

RigidTrans2D AxesTransfer2D::estimateRigidTransform(const vector<cv::Point2f>& pointsA, const vector<cv::Point2f>& pointsB)
{
    float angleRad;
    cv::Point2f translation;
    computeTransform(pointsA[0], pointsA[1], pointsB[0], pointsB[1], angleRad, translation);
    
    RigidTrans2D params;
    params.angleRad = angleRad;
    params.translation = translation;
    return params;
}

float AxesTransfer2D::normalizeAngleDeg(float angle)
{
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

void AxesTransfer2D::computeTransform(const cv::Point2f& A1, const cv::Point2f& A2, const cv::Point2f& B1, const cv::Point2f& B2, float& angleRadRet, cv::Point2f& translation)
{
    cv::Point2f vA = A2 - A1;
    cv::Point2f vB = B2 - B1;

    float angleA = std::atan2(vA.y, vA.x);
    float angleB = std::atan2(vB.y, vB.x);

    float angleRad = angleB - angleA;
    if (angleRad > CV_PI) angleRad -= 2 * CV_PI;
    if (angleRad < -CV_PI) angleRad += 2 * CV_PI;

    angleRadRet = angleRad;

    float cosTheta = std::cos(angleRad);
    float sinTheta = std::sin(angleRad);
    cv::Matx22f R(cosTheta, -sinTheta,
        sinTheta, cosTheta);

    translation = B1 - R * A1;
}

cv::Point2f AxesTransfer2D::applyTransform(const cv::Point2f& pt, float angleRad, const cv::Point2f& translation)
{
    //float angleRad = angleDeg * CV_PI / 180.0f;
    float cosTheta = std::cos(angleRad);
    float sinTheta = std::sin(angleRad);
    cv::Matx22f R(cosTheta, -sinTheta,
        sinTheta, cosTheta);
    return R * pt + translation;
}

vector<cv::Point2f> AxesTransfer2D::applyTransformList(const vector<cv::Point2f>& ptList, RigidTrans2D rigidTrans2D)
{
    vector<cv::Point2f> list;
    for(cv::Point2f point: ptList){
        cv::Point2f pointTrans = applyTransform(point, rigidTrans2D.angleRad, rigidTrans2D.translation);
        list.push_back(pointTrans);
    }
    
    return list;
}

cv::Point2f AxesTransfer2D::applyTransform(const cv::Point2f& point, RigidTrans2D rigidTrans2D)
{
    return applyTransform(point, rigidTrans2D.angleRad, rigidTrans2D.translation);
}

bool AxesTransfer2D::judgeTransform(const vector<cv::Point2f>& fiudcialWafer, const vector<cv::Point2f>& fiducialACS, RigidTrans2D rigidTrans2D)
{
    for(int i = 0; i < fiudcialWafer.size(); ++i)
    {
        cv::Point2f wafer = fiudcialWafer[i];
        cv::Point2f acs = fiducialACS[i];

        cv::Point2f transformed = applyTransform(wafer, rigidTrans2D.angleRad, rigidTrans2D.translation);
        
        float dx = transformed.x - acs.x;
        float dy = transformed.y - acs.y;
        float error = std::sqrt(dx * dx + dy * dy);

        //TODO: test
        //if(error > 0.001){
        if (error > 0.1) {
            return false;
        }
    }

    return true;
}

void AxesTransfer2D::testTransform(const cv::Point2f& A1, const cv::Point2f& A2,
    const cv::Point2f& B1, const cv::Point2f& B2)
{
    float angleRad;
    cv::Point2f translation;
    computeTransform(A1, A2, B1, B2, angleRad, translation);

    cv::Point2f A2_transformed = applyTransform(A2, angleRad, translation);

    float dx = A2_transformed.x - B2.x;
    float dy = A2_transformed.y - B2.y;
    float error = std::sqrt(dx * dx + dy * dy);
    std::cout << "Error: " << error << "\n" << std::endl;
}

void AxesTransfer2D::test()
{
    vector<Point2f> pointsA = { {0, 0}, {1, 0} }; 
    vector<Point2f> pointsB = { {0, 0}, {-0.866f, -0.5f} };

    RigidTrans2D params = estimateRigidTransform(pointsA, pointsB);
    for (Point2f point : pointsA)
    {
        cv::Point2f A2_trans = applyTransform(point, params.angleRad, params.translation);
        float x = A2_trans.x;
        float y = A2_trans.y;
    }

    //// 测试数据：原始坐标系 A 和目标坐标系 B 中的两个点
    //std::vector<std::tuple<cv::Point2f, cv::Point2f, cv::Point2f, cv::Point2f>> testCases = {
    //    // 旋转 90°
    //    { {0, 0}, {1, 0}, {0, 0}, {0, 1} },

    //    // 旋转 180°
    //    { {0, 0}, {1, 0}, {0, 0}, {-1, 0} },

    //    // 旋转 -90°
    //    { {0, 0}, {1, 0}, {0, 0}, {0, -1} },

    //    // 旋转 135°
    //    { {0, 0}, {1, 0}, {0, 0}, {-0.707f, 0.707f} },

    //    // 旋转 -150°
    //    { {0, 0}, {1, 0}, {0, 0}, {-0.866f, -0.5f} },

    //    // 平移 + 45° 旋转
    //    { {1, 1}, {2, 1}, {2, 2}, {1.293f, 2.707f} }
    //};

    //for (const auto& [A1, A2, B1, B2] : testCases) {
    //    testTransform(A1, A2, B1, B2);
    //}
}