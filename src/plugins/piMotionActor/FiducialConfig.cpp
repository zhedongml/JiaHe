#include "FiducialConfig.h"
#include <QMutex>
#include <QDebug>
#include "PrjCommon/metricsdata.h"
#include "MoveLimit.h"

FiducialConfig *FiducialConfig::m_instance = nullptr;
FiducialConfig *FiducialConfig::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new FiducialConfig();
        }
    }
    return m_instance;
}

std::string FiducialConfig::getTempleteFilePath(std::string fiducialType, int fiducialNumber)
{
    string file = m_json[fiducialType]["FiducialTempleteDir"].get<string>() +
                  m_json[fiducialType]["FiducialTemplete"][std::to_string(fiducialNumber)].get<string>();
    return file;
}

std::string FiducialConfig::getInputFiducialFilePath(std::string fiducialType)
{
    int fiducialNumber = m_json[fiducialType]["InputNearFiducial"].get<int>();
    return getTempleteFilePath(fiducialType, fiducialNumber);
}

int FiducialConfig::getInputFiducialNumber(std::string fiducialType)
{
    int fiducialNumber = m_json[fiducialType]["InputNearFiducial"].get<int>();
    return fiducialNumber;
}

bool FiducialConfig::fiduciualMotionEnabled()
{
    bool enabled = m_json["FiduciualMotionEnabled"].get<bool>();
    return enabled;
}

Result FiducialConfig::getFiduciualMotionPosList_old(std::string fiducialType, vector<cv::Point3f> &fidMotionList)
{
    vector<cv::Point3f> originList = fidMotionList;
    fidMotionList.clear();

    vector<cv::Point3f> fidMotionListRelDut;
    Result ret = getFiduciualMotionPosRelativeDut_old(fiducialType, fidMotionListRelDut);
    if (!ret.success)
    {
        return ret;
    }

    if (fidMotionListRelDut.size() != 3)
    {
        return Result(false, "Get fiduciual motion position error, size is not equal 3."); 
    }

    cv::Point3f dutCoord;
    ret = MoveLimit::instance()->getCoordinate(withDUT, dutCoord);
    if (!ret.success)
    {
        return Result(false, "Get fiduciual motion position error, " + ret.errorMsg);
    }

    for (cv::Point3f relPos : fidMotionListRelDut)
    {
        fidMotionList.push_back(dutCoord + relPos);
    }

    if (originList.size() == fidMotionList.size())
    {
        for (int i = 0; i < fidMotionList.size(); ++i)
        {
            if (abs(originList[i].x - fidMotionList[i].x) > 10 || 
                abs(originList[i].y - fidMotionList[i].y) > 10 ||
                abs(originList[i].z - fidMotionList[i].z) > 10)
            {
                return Result(false, "Fiducial config position is error,It deviates greatly from the estimated location information.");
            }
        }
    }

    return Result();
}

Result FiducialConfig::getFiduciualMotionPosList(std::string fiducialType,
                                                 vector<cv::Point3f> &fidMotionList)
{
    vector<cv::Point3f> originList = fidMotionList;
    fidMotionList.clear();

    vector<cv::Point3f> fidMotionListRelDut;
    Result ret = getFiduciualMotionPosRelativeDut(fiducialType, fidMotionListRelDut);
    if (!ret.success)
    {
        return ret;
    }

    if (fidMotionListRelDut.size() != 3)
    {
        return Result(false, "Get fiduciual motion position error, size is not equal 3.");
    }

    cv::Point3f dutCoord;
    ret = MoveLimit::instance()->getCoordinate(withDUT, dutCoord);
    if (!ret.success)
    {
        return Result(false, "Get fiduciual motion position error, " + ret.errorMsg);
    }

    for (cv::Point3f relPos : fidMotionListRelDut)
    {
        fidMotionList.push_back(dutCoord + relPos);
    }

    if (originList.size() == fidMotionList.size())
    {
        for (int i = 0; i < fidMotionList.size(); ++i)
        {
            if (abs(originList[i].x - fidMotionList[i].x) > 10 || abs(originList[i].y - fidMotionList[i].y) > 10 ||
                abs(originList[i].z - fidMotionList[i].z) > 10)
            {
                return Result(
                    false,
                    "Fiducial config position is error,It deviates greatly from the estimated location information.");
            }
        }
    }

    return Result();
}

Result FiducialConfig::getFiduciualMotionPosRelativeDut_old(std::string fiducialType, vector<cv::Point3f> &fidMotionList)
{
    int eyeType = MetricsData::instance()->getDutEyeType();

    cv::Point2f dutTilt;
    Result ret = MoveLimit::instance()->getCoordinate2DTilt(ACS2DDUT, dutTilt);
    if (!ret.success)
    {
        return Result(false, "Get fiduciual motion pos relative dut error, " + ret.errorMsg);
    }

    ret = getFiduciualMotionPosRelativeDut_old(fiducialType, eyeType, dutTilt, fidMotionList);
    return ret;
}

Result FiducialConfig::getFiduciualMotionPosRelativeDut(std::string fiducialType,
                                                        vector<cv::Point3f> &fidMotionList)
{
    int eyeType = MetricsData::instance()->getDutEyeType();

    cv::Point2f dutTilt;
    Result ret = MoveLimit::instance()->getCoordinate2DTilt(ACS2DDUT, dutTilt);
    if (!ret.success)
    {
        return Result(false, "Get fiduciual motion pos relative dut error, " + ret.errorMsg);
    }

    ret = getFiduciualMotionPosRelativeDut(fiducialType, eyeType, dutTilt, fidMotionList);
    return ret;
}

Result FiducialConfig::getFiduciualMotionPosRelativeDut_old(std::string fiducialType, int eyeType, cv::Point2f dutTilt,
                                                        vector<cv::Point3f> &fidMotionList)
{
    string eyeTypeStr = eyeType == 0 ? "Left" : "Right";
    cv::Point2f tilt;
    tilt.x = m_json[fiducialType][eyeTypeStr]["DutTipTilt"]["X"].get<float>();
    tilt.y = m_json[fiducialType][eyeTypeStr]["DutTipTilt"]["Y"].get<float>();

    if (abs(tilt.x - dutTilt.x) > 0.05 || abs(tilt.y - dutTilt.y) > 0.05)
    {
        return Result(false, QString("Fiduciual motion position relatived DUT is not exist, DUT tip/tilt is(%1,%2), EyeType is %3.")
                       .arg(dutTilt.x)
                       .arg(dutTilt.y)
                       .arg(eyeType)
                       .toStdString());
    }

    //if (abs(tilt.x) < 0.05 || abs(tilt.y) < 0.05)
    //{
    //    return Result(
    //        false, QString("Fiduciual motion position relatived DUT is invalid, config file DUT tip/tilt is(0,0), EyeType is %1.")
    //                   .arg(eyeType)
    //                   .toStdString());
    //}

    Json valFiducialPos = m_json[fiducialType][eyeTypeStr]["FiduciualMotionPosRelativeDut"];
    if (!valFiducialPos.is_null())
    {
        for (auto it = valFiducialPos.begin(); it != valFiducialPos.end(); ++it)
        {
            Json val = it.value();

            cv::Point3f pos;
            pos.x = val["X"].get<float>();
            pos.y = val["Y"].get<float>();
            pos.z = val["Z"].get<float>();
            if (abs(pos.x) < 0.0001 && abs(pos.y) < 0.0001 && abs(pos.z) < 0.0001)
            {
                return Result(false,
                    QString(
                        "Fiduciual motion position relatived DUT is invalid(0,0,0), EyeType is %1.")
                        .arg(eyeType)
                        .toStdString());
            }
            fidMotionList.push_back(pos);
        }
    }

    return Result();
}

Result FiducialConfig::getFiduciualMotionPosRelativeDut(std::string fiducialType, int eyeType,
                                                           cv::Point2f dutTilt, vector<cv::Point3f> &fidMotionList)
{
    string eyeTypeStr = eyeType == 0 ? "Left" : "Right";

    Json eyeTypeJson = m_json[fiducialType][eyeTypeStr];
    if (eyeTypeJson.is_null())
    {
        return Result(false, QString("Get fiduciual motion pos relative dut error, %1 is not exist %2 in %3.")
                                 .arg(FILE_NAME)
                                 .arg(QString::fromStdString(eyeTypeStr))
                                 .arg(QString::fromStdString(fiducialType))
                                 .toStdString());
    }

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    QString dutSn = dutConfig.sn;

    bool isExist = false;
    for (auto it = eyeTypeJson.begin(); it != eyeTypeJson.end(); ++it)
    {
        Json val = it.value();
        bool ret = judgeDutInfoSame(dutConfig, val);
        if (!ret)
        {
            continue;
        }
       
        cv::Point2f dutTiltJson(val["DutTipTilt"]["X"].get<float>(), val["DutTipTilt"]["Y"].get<float>());
        if (abs(dutTiltJson.x - dutTilt.x) > 0.05 || abs(dutTiltJson.y - dutTilt.y) > 0.05)
        {
            continue;
        }

        Json valFiducialPos = val["FiduciualMotionPosRelativeDut"];
        for (auto itFPos = valFiducialPos.begin(); itFPos != valFiducialPos.end(); ++itFPos)
        {
            Json valTmp = itFPos.value();

            cv::Point3f pos;
            pos.x = valTmp["X"].get<float>();
            pos.y = valTmp["Y"].get<float>();
            pos.z = valTmp["Z"].get<float>();
            if (abs(pos.x) < 0.0001 && abs(pos.y) < 0.0001 && abs(pos.z) < 0.0001)
            {
                return Result(false,
                              QString("Fiduciual motion position relatived DUT is invalid(0,0,0), EyeType is %1.")
                                  .arg(eyeType)
                                  .toStdString());
            }
            fidMotionList.push_back(pos);
        }

        isExist = true;
        break;
    }

    if (!isExist){
        return Result(false, QString("Fiduciual motion position relatived DUT is not exist, DUT tip/tilt is(%1,%2), "
                                     "EyeType is %3, DUT sn is %4.")
                                 .arg(dutTilt.x)
                                 .arg(dutTilt.y)
                                 .arg(eyeType)
                                 .arg(dutSn)
                                 .toStdString());
    }

    return Result();
}

bool FiducialConfig::judgeDutInfoSame(const DutConfig &dutConfig, Json eyeTypeOne)
{
    vector<cv::Point3f> fiJsonList;
    Json valFidRelInput = eyeTypeOne["FiduciualRelativeInputDutBase"];
    for (auto itFI = valFidRelInput.begin(); itFI != valFidRelInput.end(); ++itFI)
    {
        Json valTmp = itFI.value();
        cv::Point3f pos;
        pos.x = valTmp["X"].get<float>();
        pos.y = valTmp["Y"].get<float>();
        pos.z = valTmp["Z"].get<float>();
        fiJsonList.push_back(pos);
    }

    vector<cv::Point3f> fiBaseList;
    for (int i = 0; i < dutConfig.fiducials.size(); ++i)
    {
        fiBaseList.push_back(dutConfig.fiducials[i] - dutConfig.inputGrating);
    }

    if (fiBaseList.size() != fiJsonList.size())
    {
        return false;
    }

    for (int i = 0; i < fiBaseList.size(); ++i)
    {
        if (abs(fiBaseList[i].x - fiJsonList[i].x) > 2 || 
            abs(fiBaseList[i].y - fiJsonList[i].y) > 2 ||
            abs(fiBaseList[i].z - fiJsonList[i].z) > 2)
        {
            return false;
        }
    }

    return true;
}


Result FiducialConfig::writeFiduciualMotionPosRelativeDut_old(std::string fiducialType,
                                                          const cv::Point3f &dutMotion,
                                                          const vector<cv::Point3f> &mcMotions)
{
    //if (fiduciualMotionEnabled())
    //{
    //    return Result();
    //}

    if (fiducialType.empty() || mcMotions.size() == 0)
    {
        return Result(false, "Write fiduciual motion pos relative dut error, param is invalid.");
    }

    int eyeType = MetricsData::instance()->getDutEyeType();
    string eyeTypeStr = eyeType == 0 ? "Left" : "Right";

    cv::Point2f dutTilt;
    Result ret = MoveLimit::instance()->getCoordinate2DTilt(ACS2DDUT, dutTilt);
    if (!ret.success)
    {
        return Result(false, "Write fiduciual motion pos relative dut error, " + ret.errorMsg);
    }

    if (!m_json.contains(fiducialType))
    {
        return Result(false, QString("Write fiduciual motion pos relative dut error, %1 is not exist %2.")
                                 .arg(FILE_NAME)
                                 .arg(QString::fromStdString(fiducialType)).toStdString());
    }

    m_json[fiducialType][eyeTypeStr]["DutTipTilt"]["X"] = (int)round((dutTilt.x * 1000)) / 1000.0;
    m_json[fiducialType][eyeTypeStr]["DutTipTilt"]["Y"] = (int)round((dutTilt.y * 1000)) / 1000.0;

    Json &motionJson = m_json[fiducialType][eyeTypeStr]["FiduciualMotionPosRelativeDut"];

    motionJson.clear();
    for (int i = 0; i < mcMotions.size(); ++i)
    {
        Json fiCofig;
        fiCofig["X"] = (int)round(((mcMotions[i].x - dutMotion.x) * 1000)) / 1000.0;
        fiCofig["Y"] = (int)round(((mcMotions[i].y - dutMotion.y) * 1000)) / 1000.0;
        fiCofig["Z"] = (int)round(((mcMotions[i].z - dutMotion.z) * 1000)) / 1000.0;
        motionJson.push_back(fiCofig);
    }

    std::ofstream ofs(FILE_NAME.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, "Write " + FILE_NAME.toStdString() + " is failed.");
    }
    ofs << m_json.dump(4);
    ofs.close();
    return Result();
}

Result FiducialConfig::writeFiduciualMotionPosRelativeDut(std::string fiducialType,
                                                          const cv::Point3f &dutMotion,
                                                          const vector<cv::Point3f> &mcMotions)
{
    // if (fiduciualMotionEnabled())
    //{
    //    return Result();
    //}

    if (fiducialType.empty() || mcMotions.size() == 0)
    {
        return Result(false, "Write fiduciual motion pos relative dut error, param is invalid.");
    }

    int eyeType = MetricsData::instance()->getDutEyeType();
    string eyeTypeStr = eyeType == 0 ? "Left" : "Right";

    cv::Point2f dutTilt;
    Result ret = MoveLimit::instance()->getCoordinate2DTilt(ACS2DDUT, dutTilt);
    if (!ret.success)
    {
        return Result(false, "Write fiduciual motion pos relative dut error, " + ret.errorMsg);
    }

    if (!m_json.contains(fiducialType))
    {
        return Result(false, QString("Write fiduciual motion pos relative dut error, %1 is not exist %2.")
                                 .arg(FILE_NAME)
                                 .arg(QString::fromStdString(fiducialType))
                                 .toStdString());
    }

    Json &eyeTypeJson = m_json[fiducialType][eyeTypeStr];
    if (eyeTypeJson.is_null())
    {
        return Result(false, QString("Write fiduciual motion pos relative dut error, %1 is not exist %2 in %3.")
                                 .arg(FILE_NAME)
                                 .arg(QString::fromStdString(eyeTypeStr))
                                 .arg(QString::fromStdString(fiducialType))
                                 .toStdString());
    }

    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    std::string dutSn = dutConfig.sn.toStdString();

    bool isExist = false;
    for (auto it = eyeTypeJson.begin(); it != eyeTypeJson.end(); ++it)
    {
        Json &val = it.value();
        bool ret = judgeDutInfoSame(dutConfig, it.value());
        if (!ret)
        {
            continue;
        }

        cv::Point2f dutTiltJson(val["DutTipTilt"]["X"].get<float>(), val["DutTipTilt"]["Y"].get<float>());
        if (abs(dutTiltJson.x - dutTilt.x) > 0.05 || abs(dutTiltJson.y - dutTilt.y) > 0.05)
        {
            continue;
        }

        Json &motionJson = val["FiduciualMotionPosRelativeDut"];
        motionJson.clear();
        for (int i = 0; i < mcMotions.size(); ++i)
        {
            Json fiCofig;
            fiCofig["X"] = (int)round(((mcMotions[i].x - dutMotion.x) * 1000)) / 1000.0;
            fiCofig["Y"] = (int)round(((mcMotions[i].y - dutMotion.y) * 1000)) / 1000.0;
            fiCofig["Z"] = (int)round(((mcMotions[i].z - dutMotion.z) * 1000)) / 1000.0;
            motionJson.push_back(fiCofig);
        }

        Json &fidRelInput = val["FiduciualRelativeInputDutBase"];
        fidRelInput.clear();
        for (int i = 0; i < dutConfig.fiducials.size(); ++i)
        {
            Json fiCofig;
            fiCofig["X"] = (int)round(((dutConfig.fiducials[i].x - dutConfig.inputGrating.x) * 1000)) / 1000.0;
            fiCofig["Y"] = (int)round(((dutConfig.fiducials[i].y - dutConfig.inputGrating.y) * 1000)) / 1000.0;
            fiCofig["Z"] = (int)round(((dutConfig.fiducials[i].z - dutConfig.inputGrating.z) * 1000)) / 1000.0;
            fidRelInput.push_back(fiCofig);
        }

        isExist = true;
        break;
    }

    if (!isExist){

        Json eyeTypeJsonOne;
        eyeTypeJsonOne["DutSn"] = dutSn;
        eyeTypeJsonOne["DutTipTilt"]["X"] = (int)round((dutTilt.x * 1000)) / 1000.0;
        eyeTypeJsonOne["DutTipTilt"]["Y"] = (int)round((dutTilt.y * 1000)) / 1000.0;

        Json motionJson;
        for (int i = 0; i < mcMotions.size(); ++i)
        {
            Json fiCofig;
            fiCofig["X"] = (int)round(((mcMotions[i].x - dutMotion.x) * 1000)) / 1000.0;
            fiCofig["Y"] = (int)round(((mcMotions[i].y - dutMotion.y) * 1000)) / 1000.0;
            fiCofig["Z"] = (int)round(((mcMotions[i].z - dutMotion.z) * 1000)) / 1000.0;
            motionJson.push_back(fiCofig);
        }
        eyeTypeJsonOne["FiduciualMotionPosRelativeDut"] = motionJson;

        Json fidRelInput;
        for (int i = 0; i < dutConfig.fiducials.size(); ++i)
        {
            Json fiCofig;
            fiCofig["X"] = (int)round(((dutConfig.fiducials[i].x - dutConfig.inputGrating.x) * 1000)) / 1000.0;
            fiCofig["Y"] = (int)round(((dutConfig.fiducials[i].y - dutConfig.inputGrating.y) * 1000)) / 1000.0;
            fiCofig["Z"] = (int)round(((dutConfig.fiducials[i].z - dutConfig.inputGrating.z) * 1000)) / 1000.0;
            fidRelInput.push_back(fiCofig);
        }
        eyeTypeJsonOne["FiduciualRelativeInputDutBase"] = fidRelInput;

        eyeTypeJson.push_back(eyeTypeJsonOne);
    }

    std::ofstream ofs(FILE_NAME.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, "Write " + FILE_NAME.toStdString() + " is failed.");
    }
    ofs << m_json.dump(4);
    ofs.close();
    return Result();
}

void FiducialConfig::setFiducialCenter(bool isCenter)
{
    m_isFiducialCenter = isCenter;
}

bool FiducialConfig::getFiducialCenter()
{
    return m_isFiducialCenter;
}

FiducialConfig::FiducialConfig()
{
    load();
}

FiducialConfig::~FiducialConfig()
{
}

void FiducialConfig::load()
{
    std::ifstream jsonFile(FILE_NAME.toStdString());
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        m_json = Json::parse(contents);
    }
    else
    {
        qWarning() << QString("Load file error, %1").arg(FILE_NAME);
    }
}
