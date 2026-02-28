#include "CaptureFiducialImage.h"
#include "piMotionActor/MoveLimitJX.h"
#include "AlignUtils.h"
#include "piMotionActor/Motion2DModel.h"
//#include "MLAlgorithm/ML_CoordinateTransformation.h"
#include"ImageDetection/ML_CoordinateTransformation.h"

CaptureFiducialImage::CaptureFiducialImage()
{
}

CaptureFiducialImage::~CaptureFiducialImage()
{
}

Result CaptureFiducialImage::calMvCaptureFiducialCoords2D(vector<cv::Point3f>& fiducials, const QString& sn, bool isCal)
{
    if (!isCal)
    {
        return Result();
    }

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig(sn);
    CaptureFiducialBase baseInfo;
    Result ret = getBaseInfo(baseInfo, dutConfig);
    if (!ret.success)
    {
        return ret;
    }

    ret = mvCoords(baseInfo, fiducials, false);
    return ret;
}

Result CaptureFiducialImage::calMvCaptureFiducialCoords2D(vector<cv::Point3f>& fiducials, const DutConfig& dutConfig)
{
    CaptureFiducialBase baseInfo;
    Result ret = getBaseInfo(baseInfo, dutConfig);
    if (!ret.success)
    {
        return ret;
    }

    ret = mvCoords(baseInfo, fiducials, false);
    return ret;
}

Result CaptureFiducialImage::calMvCaptureFiducialCoords(vector<cv::Point3f> &fiducials, const QString &sn, bool isCal)
{
    if (!isCal)
    {
        return Result();
    }

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig(sn);
    return calMvCaptureFiducialCoords(fiducials, dutConfig);
}

Result CaptureFiducialImage::calMvCaptureFiducialCoords(vector<cv::Point3f> &fiducials, const DutConfig &dutConfig)
{
    CaptureFiducialBase baseInfo;
    Result ret = getBaseInfo(baseInfo, dutConfig);
    if (!ret.success)
    {
        return ret;
    }

    if (baseInfo.moveOutInfo.enable)
    {
        ret = mvCoords(baseInfo, fiducials, baseInfo.moveOutInfo.offset);
    }
    else
    {
        ret = mvCoords(baseInfo, fiducials);
    }
    return Result();
}

Result CaptureFiducialImage::getInitMoveCoordinate(const QString &sn, cv::Point3f &coord)
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig(sn);
    CaptureFiducialBase baseInfo;
    Result ret = getBaseInfo(baseInfo, dutConfig);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point3f mvAlignIG = baseInfo.alignIGCoord + baseInfo.mvRelativeDutIG;
    cv::Point3f mvRelEyeboxCenter = dutConfig.eyeboxCenter - baseInfo.inputGrating;
    coord = mvAlignIG + cv::Point3f(mvRelEyeboxCenter.x, mvRelEyeboxCenter.y, 0);

    {
        cv::Point2f dutAngle;
        Result ret = AlignUtils::getMotion2DAngle(ACS2DDUT, dutAngle);
        if (!ret.success)
        {
            return ret;
        }

        //TODO: test
        //cv::Point2f dutRotationAxisLength = cv::Point2f(0, CalibrationConfig::instance()->getDutRotationAxisLengthY((DUT_EYE_TYPE)dutConfig.eyeType));
        DutRotationAxisInfo dutRotationAxisLength = CalibrationConfig::instance()->getDutRotationAxisLength((DUT_EYE_TYPE)dutConfig.eyeType);
        coord = calMvByDutAngleSingle(coord, dutAngle, dutRotationAxisLength);
    }

    coord.z = coord.z - 20;

    if (baseInfo.moveOutInfo.enable){
        coord = coord + cv::Point3f(baseInfo.moveOutInfo.offset.x, baseInfo.moveOutInfo.offset.y, 0);
    }
    return Result();
}

Result CaptureFiducialImage::mvCoords(const CaptureFiducialBase &baseInfo, vector<cv::Point3f> &fiducials, bool calAngle)
{
    cv::Point3f mvAlignIG = baseInfo.alignIGCoord + baseInfo.mvRelativeDutIG;

    vector<cv::Point3f> list;
    for (cv::Point3f fiducial: baseInfo.fiducials){
        cv::Point3f mvRelFiducial = fiducial - baseInfo.inputGrating;
        cv::Point3f mvAlignFiducial = mvAlignIG + cv::Point3f(mvRelFiducial.x, mvRelFiducial.y, 0);

        list.push_back(mvAlignFiducial);
    }

    if (calAngle) {
        Result ret = calMvByDutAngle(baseInfo.eyeType, list, baseInfo);
        if (!ret.success)
        {
            return ret;
        }
    }

    fiducials = list;
    return Result();
}

Result CaptureFiducialImage::mvCoords(const CaptureFiducialBase &baseInfo, vector<cv::Point3f> &fiducials,const cv::Point2f &dutMoveOutOffset)
{
    vector<cv::Point3f> mvAlignFiducials;
    Result ret = mvCoords(baseInfo, mvAlignFiducials);
    if (!ret.success)
    {
        return ret;
    }

    vector<cv::Point3f> list;
    for (cv::Point3f mvAlignFiducial : mvAlignFiducials){
        cv::Point3f coord = mvAlignFiducial + cv::Point3f(dutMoveOutOffset.x, dutMoveOutOffset.y, 0);
        list.push_back(coord);
    }

    fiducials = list;
    return Result();
}

Result CaptureFiducialImage::getBaseInfo(CaptureFiducialBase &baseInfo, const DutConfig &dutConfig)
{
    baseInfo.fiducials = dutConfig.fiducials;
    baseInfo.inputGrating = dutConfig.inputGrating;
    baseInfo.mvRelativeDutIG = dutConfig.mvRelativeDutInputGrating;
    baseInfo.moveOutInfo = dutConfig.moveOutInfo;
    baseInfo.motion3DAndDutCoordRelate = CalibrationConfig::instance()->getMotion3DAndDutCoordRelate();
    baseInfo.eyeType = (DUT_EYE_TYPE)dutConfig.eyeType;

    cv::Point2f dutCoord;
    Result ret = MoveLimitJX::getCoordinate2D(dutCoord, ACS2DDUTPlane);
    if (!ret.success)
    {
        return ret;
    }
    baseInfo.alignIGCoord = cv::Point3f(dutCoord.x, dutCoord.y, 0);
    return Result();
}

Result CaptureFiducialImage::calMvByDutAngle(DUT_EYE_TYPE eyeType, vector<cv::Point3f> &fiducials,
                                             const CaptureFiducialBase &baseInfo)
{
    cv::Point2f angle;
    Result ret = AlignUtils::getMotion2DAngle(ACS2DDUT, angle);
    if (!ret.success)
    {
        return ret;
    }

    cv::Point2f dutRotationAxisLength = cv::Point2f(0, CalibrationConfig::instance()->getDutRotationAxisLengthY(eyeType));
    //DutRotationAxisInfo dutRotationAxisLength = CalibrationConfig::instance()->getDutRotationAxisLength(eyeType);
    fiducials = calMvByDutAngle(fiducials, angle, dutRotationAxisLength, baseInfo);

    return Result();
}

vector<cv::Point3f> CaptureFiducialImage::calMvByDutAngle(const vector<cv::Point3f> &fiducials, cv::Point2f dutAngle,
                                                          cv::Point2f dutRotationAxisLength,
                                                          const CaptureFiducialBase &baseInfo)
{
    cv::Point2f dutRotationAxisLengthNew = dutRotationAxisLength;
    vector<cv::Point3f> newfiducials;
    for (int i = 0; i < fiducials.size(); i++)
    {
        cv::Point3f point = fiducials[i];
        dutRotationAxisLengthNew.y = dutRotationAxisLength.y + (baseInfo.inputGrating.y - baseInfo.fiducials[i].y);
        dutRotationAxisLengthNew.x = dutRotationAxisLength.x - (baseInfo.inputGrating.x - baseInfo.fiducials[i].x);

        cv::Point3f newPoint = calMvByDutAngleSingle(point, dutAngle, dutRotationAxisLengthNew);
        newfiducials.push_back(newPoint);
    }

    return newfiducials;
}

vector<cv::Point3f> CaptureFiducialImage::calMvByDutAngle(const vector<cv::Point3f> &fiducials, cv::Point2f dutAngle,
                                                          DutRotationAxisInfo dutRotationAxisInfo,
                                                          const CaptureFiducialBase &baseInfo)
{
    vector<cv::Point3f> newfiducials;
    for (int i = 0; i < fiducials.size(); i++)
    {
        cv::Point3f point = fiducials[i];
        dutRotationAxisInfo.axisLength.y = dutRotationAxisInfo.axisLength.y + (baseInfo.inputGrating.y - baseInfo.fiducials[i].y);
        dutRotationAxisInfo.axisLength.x = dutRotationAxisInfo.axisLength.x + (-baseInfo.inputGrating.x + baseInfo.fiducials[i].x);

        cv::Point3f newPoint = calMvByDutAngleSingle(point, dutAngle, dutRotationAxisInfo);
        newfiducials.push_back(newPoint);
    }

    return newfiducials;
}

cv::Point3f CaptureFiducialImage::calMvByDutAngleSingle(const cv::Point3f &coord, cv::Point2f dutAngle,
                                                      cv::Point2f dutRotationAxisLength)
{
    float offsetZ = dutRotationAxisLength.y * sin(dutAngle.y * 3.14 / 180.0);
    float offsetY = dutRotationAxisLength.y - dutRotationAxisLength.y * cos(dutAngle.y * 3.14 / 180.0);

    float offsetZ_X = dutRotationAxisLength.x * sin(dutAngle.x * 3.14 / 180.0);
    float offsetX = dutRotationAxisLength.x - dutRotationAxisLength.x * cos(dutAngle.x * 3.14 / 180.0);

    cv::Point3f newPoint;
    newPoint.x = coord.x + offsetX;
    newPoint.y = coord.y + offsetY;
    newPoint.z = coord.z - offsetZ + offsetZ_X;
    return newPoint;
}

cv::Point3f CaptureFiducialImage::calMvByDutAngleSingle(const cv::Point3f &coord, cv::Point2f dutAngle,
                                                        DutRotationAxisInfo dutRotationAxisInfo)
{
    // X rotate
    float destX_Z = sin((dutRotationAxisInfo.initAngle.x + dutAngle.x) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.x;
    float destInitX_Z = sin((dutRotationAxisInfo.initAngle.x) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.x;
    float offsetX_Z = destX_Z - destInitX_Z;

    float destX = cos((dutRotationAxisInfo.initAngle.x + dutAngle.x) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.x;
    float destInitX = cos((dutRotationAxisInfo.initAngle.x) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.x;
    float offsetX = destX - destInitX;

    // Y rotate
    float destY_Z = sin((dutRotationAxisInfo.initAngle.y - dutAngle.y) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.y;
    float destInitY_Z = sin((dutRotationAxisInfo.initAngle.y) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.y;
    float offsetY_Z = destY_Z - destInitY_Z;

    float destY = cos((dutRotationAxisInfo.initAngle.y - dutAngle.y) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.y;
    float destInitY = cos((dutRotationAxisInfo.initAngle.y) / 180.0 * 3.14) * dutRotationAxisInfo.axisLength.y;
    float offsetY = destY - destInitY;

    cv::Point3f newPoint;
    newPoint.x = coord.x + offsetX;
    newPoint.y = coord.y + offsetY;
    newPoint.z = coord.z + offsetX_Z + offsetY_Z;
    return newPoint;
}
