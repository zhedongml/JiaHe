#include "MLColorimeterMode.h"
#include "ninesquaremtfmodel.h"
#include "PrjCommon/metricsdata.h"
#include "resultprocessor.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDebug>
#include "metricsdatabase.h"
#include "Core/loggingwrapper.h"
//#include "MLAlgorithm/ml_crosshairpara.h"
#include "pluginsystem/Services.h"
#include "Focus/AutoFocusModel.h"

const std::string logFileHeader =
"score0,angle0,x0,y0,score1,angle1,x1,y1,score2,angle2,x2,y2,score3,angle3,x3,y3,score4,angle4,x4,y4,score5,angle5,"
"x5,y5,score6,angle6,x6,y6,score7,angle7,x7,y7,score8,angle8,x8,y8\n ";

NineSquareMTFModel* NineSquareMTFModel::self = nullptr;
NineSquareMTFModel::NineSquareMTFModel(QObject* parent): QObject(parent)
{
}

NineSquareMTFModel::~NineSquareMTFModel()
{
}

NineSquareMTFModel* NineSquareMTFModel::getInstance(QObject* parent)
{
    if (self == nullptr)
    {
        self = new NineSquareMTFModel(parent);
    }
    self->setParent(parent);
    return self;
}

void NineSquareMTFModel::setNineSquareImg(cv::Mat img)
{
    nineSquareImg = img;
}

void NineSquareMTFModel::writeMatchLogAll(QString writePath, QString writePathAll)
{
    std::string logAllfullPath = writePathAll.toStdString() + "/nineCrossPositionAll.csv";
    std::string logfullPath = writePath.toStdString() + "/nineCrossPosition.csv";
    std::ifstream infile(logAllfullPath);
    std::string DUTID = MetricsData::instance()->getDutBarCode().toStdString();
    if (!infile.good())
    {
        std::ofstream crossFileAll(logAllfullPath);
        crossFileAll << logFileHeader;
        std::ostream_iterator<double> vecFlatten(crossFileAll, ",");
        std::copy(MTFModelConfig::instance()->crossesPosition.begin(),
                  MTFModelConfig::instance()->crossesPosition.end(), vecFlatten);
    }
    else
    {
        std::ofstream crossFileAll;
        crossFileAll.open(logAllfullPath, std::ios::out | std::ios::app);
        crossFileAll << DUTID;
        std::ostream_iterator<double> vecFlatten(crossFileAll, ",");
        std::copy(MTFModelConfig::instance()->crossesPosition.begin(),
                  MTFModelConfig::instance()->crossesPosition.end(), vecFlatten);
    }
    std::ofstream crossFile(logfullPath);
    crossFile << logFileHeader;
    std::ostream_iterator<double> writeVector(crossFile, ",");
    std::copy(MTFModelConfig::instance()->crossesPosition.begin(), MTFModelConfig::instance()->crossesPosition.end(),
              writeVector);
    MTFModelConfig::instance()->crossesPosition.clear();
}

void NineSquareMTFModel::calculateMTF(bool isAutoMode, QList<int> list)
{
    // initiate save files
    QDateTime starttime = QDateTime::currentDateTime();
    MetricsData::instance()->setMTFStartTime(starttime.toTime_t());
    clearResult();
    QString mtfcsvName = MetricsData::instance()->getMTFRecipeName();
    QString filePath = MetricsData::instance()->getRecipeSeqDir();
    //LoggingWrapper::instance()->warn(QString("Capture image start."));
    
    if (useOffLineImage)
    {
        std::string path = offlineImagPath.toStdString();
        nineSquareImg = cv::imread(path.c_str(), cv::IMREAD_UNCHANGED);
    }
    else
    {
        uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();

        Result ret = MLColorimeterMode::Instance()->takeImage(nineSquareImg);
        if (!ret.success)
        {
            LoggingWrapper::instance()->warn(
                QString("Calculate MTF failed, Capture image error, %1").arg(QString::fromStdString(ret.errorMsg)));
            return;
        }

        ret = MLColorimeterMode::Instance()->imageRotation(nineSquareImg, nineSquareImg);
        if (!ret.success)
        {
            LoggingWrapper::instance()->warn(
                QString("Calculate MTF failed, Capture image error, %1").arg(QString::fromStdString(ret.errorMsg)));
            return;
        }

        uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
        qWarning() << "---- takt time ---- [MTF]"
                   << "get image time: " << deltaT << " ms...";
    }

    //LoggingWrapper::instance()->warn(QString("Capture image done."));

    QDate current_date = QDate::currentDate();
    QString currentDate = current_date.toString("yyyyMMdd");
    QString desPath = filePath + QDir::separator() + currentDate + QDir::separator() + dutId;
    QDir dir(desPath);
    if (!dir.exists())
    {
        dir.mkpath(desPath);
        QString fileName = desPath + "/img_mtf_display_"+ledColor + ".tif";

        std::string rawImgStr = fileName.toStdString();
        cv::imwrite(rawImgStr.c_str(), nineSquareImg);
    }
    // initiate save files done
    int aoiNum = 9;
    std::vector<CrossAreaConfig> aois;
    if (isAutoMode)
    {
        visionAlgorithm::GetInstance()->SourceImage = nineSquareImg;
        aois = MTFModelConfig::instance()->getAutoROIConfig();
        writeMatchLogAll(desPath, matchLogAll);
    }
    else
    {
        aois = MTFModelConfig::instance()->getAOIConfig();
    }
    aoiNum = aois.size();
    ResultProcessor* processor = new ResultProcessor();
    processor->setVid(vid);
    {
        cv::Rect allArea(0, 0, nineSquareImg.cols, nineSquareImg.rows);
        for (int i = 0; i < aoiNum; i++)
        {
            CrossAreaConfig aoi = aois.at(i);
            cv::Rect aoiRect(aoi.FromX, aoi.FromY, aoi.aoiWidth, aoi.aoiHeight);
            cv::Mat crossImg = nineSquareImg(aoiRect & allArea);

            //LoggingWrapper::instance()->warn(QString("MTF calculate AOI %1 start.").arg(i));
            uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
            CrossImgMTFModel* mtfModel = new CrossImgMTFModel();
            mtfModel->setImage(crossImg);
            mtfModel->setAOIId(aoi.id);
            mtfModel->calculateMTF(isAutoMode, list);
            uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
            qWarning() << "---- takt time ---- [MTF]"
                       << "aoi " << i << "- calculate MTF time : " << deltaT << " ms... ";
            //LoggingWrapper::instance()->warn(QString("MTF calculate AOI %1 done.").arg(i));

            if (saveMTFData)
            {
                uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
                mtfModel->saveMTFData(desPath, "", i);
                uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
                qWarning() << "---- takt time ---- [MTF]"
                << "aoi " << i << "- save MTF image & csv data time : " << deltaT << " ms... ";
            }
            
            st = QDateTime::currentDateTime().toMSecsSinceEpoch();
            PassResult *passRest = mtfModel->judgeResult();
            QVector<PassDetail> &details = passRest->passDetails;
            for each (PassDetail var in details)
            {
                MetricItem metric;
                std::string nystr = MTFModelConfig::instance()->getNyHeaderFromFre(var.freq.toStdString());
                QString des = var.dir + "MTF" + QString::number(passRest->aoiId) + "/" + var.freq;
                ;
                // if (aoiNum == 1 || passRest->aoiId == 5)
                {
                    QString headerdes = "MTF_" + ledColor.toUpper() + (var.dir.toUpper() == "V" ? "Y" : "X") +
                                        QString::fromStdString(nystr) + "_" + QString::number(passRest->aoiId + 1);
                    MetricsDataBase::getInstance()->updateMetricValue(headerdes, var.mtfVal, "5");
                    if (passRest->aoiId == 1)
                        MetricsDataBase::getInstance()->updateMetricValue("test_vid", vid, "5");
                }
                metric.lowLimit = var.lowLimit;
                metric.upLimit = var.upLimit;
                metric.value = var.mtfVal;
                metric.metricName = des;
                processor->addMetricItem(metric);
            }
                
            deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;

            qWarning() << "---- takt time ---- [MTF]"
                        << "aoi " << i << "- updated MTF test results time : " << deltaT << " ms... ";

            delete mtfModel;
        }
    }

    QString absFileName = filePath + QDir::separator() + mtfcsvName;
    processor->saveTOCVSData(absFileName);
    delete processor;
}

void NineSquareMTFModel::calculateMTFByAutoIdentifyNineCross(bool isAutoMode, QList<int> list)
{
    if (!isAutoMode){
        calculateMTF(isAutoMode, list);
        return;
    }

    // initiate save files
    QDateTime starttime = QDateTime::currentDateTime();
    MetricsData::instance()->setMTFStartTime(starttime.toTime_t());
    clearResult();
    QString mtfcsvName = MetricsData::instance()->getMTFRecipeName();
    QString filePath = MetricsData::instance()->getRecipeSeqDir();
    //LoggingWrapper::instance()->warn(QString("Capture image start."));

    if (useOffLineImage)
    {
        std::string path = offlineImagPath.toStdString();
        nineSquareImg = cv::imread(path.c_str(), cv::IMREAD_UNCHANGED);
    }
    else
    {
        uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
        
        Result ret = MLColorimeterMode::Instance()->takeImage(nineSquareImg);
        if (!ret.success)
        {
            LoggingWrapper::instance()->warn(
                QString("Capture image failed, %1").arg(QString::fromStdString(ret.errorMsg)));
            return;
        }

        ret = MLColorimeterMode::Instance()->imageRotation(nineSquareImg, nineSquareImg);
        if (!ret.success)
        {
            LoggingWrapper::instance()->warn(
                QString("Image rotation failed, %1").arg(QString::fromStdString(ret.errorMsg)));
            return;
        }

        uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
        qWarning() << "---- takt time ---- [MTF]"
                   << "get image time: " << deltaT << " ms...";
    }

    //LoggingWrapper::instance()->warn(QString("Capture image done."));
    cv::Mat imgdraw;
    vector<cv::Mat> crossImgs = autoIdentifyNineCross(nineSquareImg, imgdraw);

    QDate current_date = QDate::currentDate();
    QString currentDate = current_date.toString("yyyyMMdd");
    //QString desPath = filePath + QDir::separator() + currentDate + QDir::separator() + dutId;
    QString desPath = filePath + QDir::separator() + dutId;

    QDir dir(desPath);
    if (!dir.exists())
    {
        dir.mkpath(desPath);
        QString fileName = desPath + "/img_mtf_display_" + ledColor + ".tif";
        std::string rawImgStr = fileName.toStdString();
        cv::imwrite(rawImgStr.c_str(), nineSquareImg);
        QString fileNamedraw = desPath + "/img_mtf_draw_" + ledColor + ".tif";
        cv::imwrite(fileNamedraw.toStdString(), imgdraw);
    }
    
    ResultProcessor *processor = new ResultProcessor();
    processor->setVid(vid);
    {
        cv::Rect allArea(0, 0, nineSquareImg.cols, nineSquareImg.rows);
        for (int i =0; i < crossImgs.size(); i++)
        {
            cv::Mat crossImg = crossImgs.at(i);

            //LoggingWrapper::instance()->warn(QString("MTF calculate AOI %1 start.").arg(i));
            uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
            CrossImgMTFModel *mtfModel = new CrossImgMTFModel();
            mtfModel->setImage(crossImg);
            mtfModel->setAOIId(i);
            mtfModel->calculateMTF(isAutoMode, list);
            uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
            qWarning() << "---- takt time ---- [MTF]"
                       << "aoi " << i << "- calculate MTF time : " << deltaT << " ms... ";
            //LoggingWrapper::instance()->warn(QString("MTF calculate AOI %1 done.").arg(i));

            if (saveMTFData)
            {
                uint st = QDateTime::currentDateTime().toMSecsSinceEpoch();
                mtfModel->saveMTFData(desPath, "", i);
                uint deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;
                qWarning() << "---- takt time ---- [MTF]"
                           << "aoi " << i << "- save MTF image & csv data time : " << deltaT << " ms... ";
            }

            st = QDateTime::currentDateTime().toMSecsSinceEpoch();
            PassResult *passRest = mtfModel->judgeResult();
            QVector<PassDetail> &details = passRest->passDetails;
            for each (PassDetail var in details)
            {
                MetricItem metric;
                std::string nystr = MTFModelConfig::instance()->getNyHeaderFromFre(var.freq.toStdString());
                QString des = var.dir + "MTF" + QString::number(passRest->aoiId) + "/" + var.freq;
                ;
                // if (aoiNum == 1 || passRest->aoiId == 5)
                {
                    QString headerdes = "MTF_" + ledColor.toUpper() + (var.dir.toUpper() == "V" ? "Y":"X") +
                                        QString::fromStdString(nystr) + "_" + QString::number(passRest->aoiId + 1)
                                        + "_0"+eyeboxNum+"Eyebox";
                    MetricsDataBase::getInstance()->updateMetricValue(headerdes, var.mtfVal, eyeboxNum);
                    if (passRest->aoiId == 1)
                        MetricsDataBase::getInstance()->updateMetricValue("test_vid", vid, eyeboxNum);
                }
                metric.lowLimit = var.lowLimit;
                metric.upLimit = var.upLimit;
                metric.value = var.mtfVal;
                metric.metricName = des;
                processor->addMetricItem(metric);
            }

            deltaT = QDateTime::currentDateTime().toMSecsSinceEpoch() - st;

            qWarning() << "---- takt time ---- [MTF]"
                       << "aoi " << i << "- updated MTF test results time : " << deltaT << " ms... ";

            delete mtfModel;
        }
    }

    QString absFileName = filePath + QDir::separator() + mtfcsvName;
    processor->saveTOCVSData(absFileName);
    delete processor;
}

void NineSquareMTFModel::generateResult()
{
}

bool NineSquareMTFModel::createResutCSVFile(QString cvFileName)
{
    QFile csvFile(cvFileName);
    if (!csvFile.open(QIODevice::ReadWrite))
    {
        return false;
    }
    else
    {
        csvFile.close();
        return true;
    }
}

bool NineSquareMTFModel::getIsOffLineImg()
{
    return useOffLineImage;
}

void NineSquareMTFModel::setIsOffLineImg(bool val)
{
    useOffLineImage = val;
}

void NineSquareMTFModel::setOfflineImgPath(QString imgPath)
{
    offlineImagPath = imgPath;
}

void NineSquareMTFModel::setDUTBarCode(QString val)
{
    dutId = val;
}

Result NineSquareMTFModel::thruFocus(float expectFocus)
{
    float focusRet;
    Result ret = AutoFocusModel::instance()->recipeAutoFocus(focusRet, expectFocus);
    if(!ret.success){
        return ret;
    }
    vid = focusRet * (-0.001); // convert to meter and The direction is opposite
    return ret;
}

void NineSquareMTFModel::setLedColor(QString color)
{
    ledColor = color;
}

void NineSquareMTFModel::setEyeboxNum(QString num)
{
    eyeboxNum = num;
}

void NineSquareMTFModel::recipeStop(bool isStop)
{
    //CameraModel::GetInstance().recipeStop(isStop);
}

std::vector<cv::Rect> NineSquareMTFModel::autoIdentifyNineCrossRect(cv::Mat image)
{
    vector<cv::Point2f> nineCrossCenters;
    MLImageDetection::MultiCrossHairDetection muld;
    // TODO: to add the LEFT and right flag,now is 0 left
    MLImageDetection::MultiCrossHairRe hRe = muld.getMuliCrossHairCenterByDistance(image, MetricsData::instance()->getReticleEyeType(), 1);
    return hRe.rectVec;

    //muld.getMultiCrossHairCenterBySlope(image, nineCrossCenters);
    //int widthHeight = 500; // 120;
    //cv::Rect allArea(0, 0, image.cols, image.rows);
    //std::vector<cv::Rect> aois;
    //for (int i = 0; i < nineCrossCenters.size(); ++i)
    //{
    //    cv::Point2f center = nineCrossCenters.at(i);
    //    cv::Rect roi(center.x - widthHeight / 2, center.y - widthHeight / 2, widthHeight, widthHeight);
    //    aois.push_back(roi & allArea);
    //}
    //return aois;
}

std::vector<cv::Mat> NineSquareMTFModel::autoIdentifyNineCross(cv::Mat image, cv::Mat &imgdraw)
{
    MLImageDetection::MultiCrossHairDetection muld;
    // TODO: to add the LEFT and right flag,now is 0 left
    MLImageDetection::MultiCrossHairRe hRe = muld.getMuliCrossHairCenterByDistance(image, MetricsData::instance()->getReticleEyeType(), 1);
    std::vector<cv::Rect> aoiRects = hRe.rectVec;
    imgdraw = hRe.img_draw;
    //= autoIdentifyNineCrossRect(image);
    vector<cv::Mat> aois;
    for (int i = 0; i < aoiRects.size(); ++i)
    {
        cv::Mat crossImg = image(aoiRects.at(i));
        aois.push_back(crossImg);
    }
    return aois;
}

void NineSquareMTFModel::clearResult()
{
    for (int i = 0; i < resultVec.size(); i++)
    {
        PassResult* item = resultVec.at(i);
        if (item != nullptr)
            delete item;
    }
    resultVec.clear();
}