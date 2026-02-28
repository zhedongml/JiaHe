#include "AnalyzeRecipe.h"
#include <QFileDialog>
#include <QTextStream>
#include "AlignUtils.h"

AlignConfig AnalyzeRecipe::analyzCalibrateParam(QString param)
{
    AlignConfig &alignConfig = CalibrationConfig::instance()->getAlignConfigReference();
    
    DutConfig &dutConfig = CalibrationConfig::instance()->getDutConfigReference();
    alignConfig.calculateMvFidsByInputFlag = dutConfig.calculateMvFidsByInputFlag;
    alignConfig.moveOut = dutConfig.moveOutInfo.enable;

    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return alignConfig;
    }

    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "autoMode")
            {
                alignConfig.autoMode = val.toInt();
            }
            else if (key == "fiducialType")
            {
                alignConfig.fiducialType = AlignUtils::analyzeFiducialType(val);
                alignConfig.fiducialTypeStr = val.toStdString();
            }
            else if (key == "fiducialCount")
            {
                alignConfig.fiducialCount = val.toInt();
            }
            else if (key == "MoveOut")
            {
                alignConfig.moveOut = val.toInt();
                dutConfig.moveOutInfo.enable = val.toInt();
            }
            else if (key.toLower() == QString("calculateMvFidsByInputFlag").toLower())
            {
                alignConfig.calculateMvFidsByInputFlag = val.toInt();
                dutConfig.calculateMvFidsByInputFlag = val.toInt();
            }
        }
    }
    return alignConfig;
}

Result AnalyzeRecipe::analyzMvExposeParam(QString param)
{
    AlignConfig &alignConfig = CalibrationConfig::instance()->getAlignConfigReference();
    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, QString("Analyze Mv expose param error, param is %1.").arg(param).toStdString());
    }

    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "auto")
            {
                alignConfig.mvExpose.isAuto = (val.toInt() > 0);
            }
            else if (key == "exposureTime")
            {
                alignConfig.mvExpose.manualExposeTime = val.toInt();
            }
        }
    }
    return Result();
}

Result AnalyzeRecipe::analyzDutConfigParam(QString param, DutConfig &dutConfig)
{
    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, QString("Analyze dut config param error, param is %1.").arg(param).toStdString());
    }

    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 1)
        {
            QString key = paramPair[0].trimmed();
            if (key == "start")
            {
                // dutConfig.clear();
            }
        }

        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed().trimmed();
            QString val = paramPair[1].trimmed().trimmed();
            if (key.toLower() == "dutmodel" )
            {
                dutConfig.sn = val;
                Result ret = CalibrationConfig::instance()->setDefaultDutName(val);
                if(!ret.success){
                    return ret;
                }
            }
            else if (key == "eyeType")
            {
                dutConfig.eyeType = val.toInt();
            }
            else if (key == "eyeRelief")
            {
                dutConfig.eyeRelief = val.toFloat();
            }
            else if (key == "fiducials")
            {
                std::vector<cv::Point3f> list = analyzCoords(val);
                if (list.size() == 0)
                {
                    return Result(
                        false,
                        QString("Analyze dut config fiducials param error, param is %1.").arg(param).toStdString());
                }
                dutConfig.fiducials = list;
            }
            else if (key == "eyeboxs")
            {
                std::vector<cv::Point3f> list = analyzCoords(val);
                if (list.size() == 0)
                {
                    return Result(
                        false,
                        QString("Analyze dut config eyeboxs param error, param is %1.").arg(param).toStdString());
                }
                dutConfig.eyeboxs = list;
            }
            else if (key.toLower() == QString("eyeboxCenter").toLower())
            {
                QStringList coords = val.split(",");
                if (coords.size() >= 3){
                    dutConfig.eyeboxCenter = cv::Point3f(coords.at(0).trimmed().toFloat(), coords.at(1).trimmed().toFloat(), coords.at(2).trimmed().toFloat());
                }
                else{
                    return Result(
                        false,
                        QString("Analyze dut config eyeboxCenter param error, param is %1.").arg(param).toStdString());
                }
            }
            else if (key == "eyeboxOffset")
            {
                cv::Point3f centerEye = dutConfig.eyeboxCenter;
                QStringList offsets = val.split(",");
                if (offsets.size() == 1)
                {
                    float offset = offsets[0].toFloat();
                    dutConfig.eyeboxs = AlignUtils::calculateEyeboxs(centerEye, offset);
                }
                else if (offsets.size() == 4)
                {
                    QList<float> offsetList;
                    for (QString str : offsets)
                    {
                        offsetList.push_back(str.toFloat());
                    }
                    dutConfig.eyeboxs = AlignUtils::calculateEyeboxs(centerEye, offsetList);
                }
                else
                {
                    return Result(
                        false,
                        QString("Analyze dut config eyeboxOffset param error, param is %1.").arg(param).toStdString());
                }
            }
            else if (key == "eyeboxOffsetAll"){
                QStringList offsets = val.split(";");
                std::vector<cv::Point3f> offsetList;
                for (QString str : offsets)
                {
                    QStringList pointStr = str.split(",");
                    if (pointStr.size() != 3){
                        return Result(false, QString("Analyze dut config eyeboxOffsetAll param error, param is %1.")
                                                 .arg(param)
                                                 .toStdString());
                    }
                    offsetList.push_back(cv::Point3f(pointStr[0].toFloat(), pointStr[1].toFloat(), pointStr[2].toFloat()));
                }

                dutConfig.exeboxOffsetAll = offsetList;
            }
            else if (key.toLower() == QString("inputGrating").toLower())
            {
                QStringList coords = val.split(",");
                if (coords.size() >= 3)
                {
                    dutConfig.inputGrating =
                        cv::Point3f(coords.at(0).trimmed().toFloat(), coords.at(1).trimmed().toFloat(),
                                    coords.at(2).trimmed().toFloat());
                }
                else
                {
                    return Result(
                        false,
                        QString("Analyze dut config inputGrating param error, param is %1.").arg(param).toStdString());
                }
            }
            else if (key.toLower() == QString("calculateMvFidsByInputFlag").toLower())
            {
                dutConfig.calculateMvFidsByInputFlag = val.toInt() > 0;             
            }
        }
    }
    return Result();
}

Result AnalyzeRecipe::analyzEeyeboxParam(QString param, int &index, bool &moveout)
{
    index = 0;
    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, "Recipe eyebox index is null.");
    }

    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "index")
            {
                index = val.toInt();
            }
            else if (key == "indexName")
            {
                EYEBOX_INDEX eyeboxIndex;
                Result ret = AlignUtils::eyeboxIndexFromName(val, eyeboxIndex);
                if (!ret.success)
                {
                    return ret;
                }
                index = eyeboxIndex;
            }
            else if (key == "moveout")
            {
                moveout = val.toInt();
            }
        }
    }
    return Result();
}

QList<QString> AnalyzeRecipe::selectCalibrateInfo(QString fileName, DutConfig &dutConfig, AlignConfig &alignConfig)
{
    QList<QString> lineDatas = readFile(fileName);
    return selectInfo(lineDatas, dutConfig, alignConfig);
}

Result AnalyzeRecipe::analyzeLines(QList<QString> lines, DutConfig &dutConfig, AlignConfig &alignConfig)
{
    for (QString line : lines)
    {
        QStringList strList = line.split(":");
        if (strList.size() < 2)
        {
            continue;
        }

        QString cmd = strList.at(0).trimmed();
        QString param = strList.at(1).trimmed();

        if (cmd == "dutConfig")
        {
            Result ret = analyzDutConfigParam(param, dutConfig);
        }
        else if (cmd == "calibrate")
        {
            AlignConfig info = analyzCalibrateParam(param);
            alignConfig.setInfo(&info);
        }
    }
    return Result();
}

Result AnalyzeRecipe::analyzeCalibrateConfig(QString param, CalibrateConfig &calConfig)
{
    QStringList list = param.split("/");
    if (list.size() < 1)
    {
        return Result(false, QString("Analyze calibrate config param error, param is %1.").arg(param).toStdString());
    }

    for each (QString paramstr in list)
    {
        QStringList paramPair = paramstr.split("=");
        if (paramPair.size() == 1)
        {
            QString key = paramPair[0].trimmed();
            if (key == "start")
            {
                // calConfig.clear();
            }
        }

        if (paramPair.size() == 2)
        {
            QString key = paramPair[0].trimmed();
            QString val = paramPair[1].trimmed();
            if (key == "motion3DX" || key == "motion3DY")
            {
                calConfig.motion3DAndDutCoordRelate[key] = val;
            }
            else if (key == "acsMvCoordinateOfFiducials")
            {
                std::vector<cv::Point3f> vector = analyzCoords(val);
                calConfig.setAcsMvCoordinateOfFiducials(vector);
            }
            else if (key == "measureCameraAngle")
            {
                QStringList list = val.split(",");
                if (list.size() == 2)
                {
                    calConfig.measureCameraAngle = cv::Point2f(list.at(0).toFloat(), list.at(1).toFloat());
                }
            }
            else if (key == "dutAngleXY")
            {
                QStringList list = val.split(",");
                if (list.size() == 2)
                {
                    calConfig.dutAngleXY = cv::Point2f(list.at(0).toFloat(), list.at(1).toFloat());
                }
            }
        }
    }
    return Result();
}

QList<QString> AnalyzeRecipe::selectInfo(QList<QString> lineDatas, DutConfig &dutConfig, AlignConfig &alignConfig)
{
    QList<QString> list;
    for (QString line : lineDatas)
    {
        QStringList strList = line.split(":");
        if (strList.size() < 3)
        {
            continue;
        }

        QString mode = strList.at(0).trimmed();
        if (mode != "alignment")
        {
            continue;
        }
        QString cmd = strList.at(1).trimmed();
        QString param = strList.at(2).trimmed();

        if (cmd == "dutConfig")
        {
            Result ret = analyzDutConfigParam(param, dutConfig);
            if (param.contains("="))
            {
                list.append(splitParam(cmd, param));
            }
        }
        else if (cmd == "calibrate")
        {
            AlignConfig info = analyzCalibrateParam(param);
            alignConfig.setInfo(&info);
            if (param.contains("="))
            {
                list.append(splitParam(cmd, param));
            }
        }
    }
    return list;
}

std::vector<cv::Point3f> AnalyzeRecipe::analyzCoords(QString coordParam)
{
    std::vector<cv::Point3f> list;
    QStringList coordStrs = coordParam.split(";");
    for (QString coordStr : coordStrs)
    {
        QStringList coords = coordStr.split(",");
        if (coords.size() == 3)
        {
            list.push_back(cv::Point3f(coords.at(0).trimmed().toFloat(), coords.at(1).trimmed().toFloat(),
                                       coords.at(2).trimmed().toFloat()));
        }
    }
    return list;
}

QList<QString> AnalyzeRecipe::readFile(QString fileName)
{
    QList<QString> lineDatas;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line;
        QTextStream in(&file);
        line = in.readLine();
        while (!line.isNull())
        {
            line = in.readLine();
            lineDatas.push_back(line);
        }
    }
    file.close();
    return lineDatas;
}

QList<QString> AnalyzeRecipe::splitParam(QString cmd, QString param)
{
    QList<QString> list;
    QStringList strList = param.split("/");
    for (QString str : strList)
    {
        if (!str.trimmed().isEmpty())
        {
            list.push_back(cmd + ":" + str);
        }
    }
    return list;
}
