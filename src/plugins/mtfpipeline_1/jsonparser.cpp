#include "jsonparser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <mutex> 

JsonParser* JsonParser::self = 0;
JsonParser::JsonParser() {

}
bool JsonParser::loadConfig(string path)
{
    string defaultpath = "./pipelineconfig.json";
    this->configpath = path;
    fstream ofile(path);
    if (!ofile.is_open()) {
        return false;
    }
   /* string strjson;
    string strline;
    while (getline(ofile, strline)) {
        strjson += strline;
    }*/
    
    Json::CharReaderBuilder cb;
    Json::Value jsonObject;
    JSONCPP_STRING errs;
    if (Json::parseFromStream(cb, ofile, &jsonObject, &errs)) {
        try {
            setPreProcConfig(jsonObject);
            setLSFProcConfig(jsonObject);
            setESFProcConfig(jsonObject);
            setFitProcConfig(jsonObject);
            setDFTProcConfig(jsonObject);
            setOverSampleProcConfig(jsonObject);
            setMTFPostProcConfig(jsonObject);
            ofile.close();
        }
        catch (...) {
            ofile.close();
            return false;
        }
    }
  
    return true;
}

bool JsonParser::writeConfigToFile()
{
    fstream ofile(this->configpath);
    if (!ofile.is_open()) {
        return false;
    }
    Json::Value  jsonObject;
    {
        setPreProcJsonVal(jsonObject);
        setFitProcJsonVal(jsonObject);
        setLSFProcJsonVal(jsonObject);
        setESFProcJsonVal(jsonObject);
        setDFTProcJsonVal(jsonObject);
        setOverSampleProcJsonVal(jsonObject);
        setMTFPostProcJsonVal(jsonObject);
    }
    ofile<<jsonObject.toStyledString()<<endl;
    ofile.close();
    return true;
}

JsonParser* JsonParser::getInstance()
{
    if (!self) {
        mutex mutex;
        mutex.lock();
        if (!self) {
            self = new JsonParser;
        }
        mutex.unlock();
    }

    return self;
}

void JsonParser::setPreProcConfig(const Json::Value& jsonObject)
{
    Json::Value preProcJsonObj = jsonObject["PreProcessor"];
    bool isDarkNoise = preProcJsonObj["isDarkNoise"].asBool();
    bool isPixelization = preProcJsonObj["isPixelization"].asBool();
    int meanWinSize = preProcJsonObj["meanWinSize"].asInt();
    bool hammingWind = preProcJsonObj["hammingWind"].asBool();
    PreProcessorconfig::hammingWind = hammingWind;
    PreProcessorconfig::meanWinSize = meanWinSize;
    PreProcessorconfig::isDarkNoise = isDarkNoise;
    PreProcessorconfig::isPixelization = isPixelization;
}
void JsonParser::setFitProcConfig(const Json::Value const& jsonObject) {
    Json::Value fitProcJsonObj = jsonObject["FitProcessor"];
    int polynominalOrder=fitProcJsonObj["polynominalOrder"].asInt();
    FitProcessorConfig::polynominalOrder = polynominalOrder;

    bool distortioncorrect = fitProcJsonObj["distortioncorrect"].asBool();
    FitProcessorConfig::distortioncorrect = distortioncorrect;

}
void JsonParser::setLSFProcConfig(const Json::Value const& jsonObject)
{
    Json::Value lsfProcJsonObj = jsonObject["LSFProcessor"];
    int meanwin = lsfProcJsonObj["meanWindowSize"].asInt();
    bool hamwin = lsfProcJsonObj["hammingWindow"].asBool();
    LSFProcessorConfig::meanWindowSize = meanwin;
    LSFProcessorConfig::hammingWindow = hamwin;
}
void JsonParser::setESFProcConfig(const Json::Value const& jsonObject)
{
    Json::Value esfProcJsonObj = jsonObject["ESFProcessor"];
    int meanwin = esfProcJsonObj["meanWindow"].asInt();
    bool isReverse = esfProcJsonObj["reverse"].asBool();
    ESFProcessorConfig::meanWindow = meanwin;
    ESFProcessorConfig::reverse = isReverse;
}

void JsonParser::setDFTProcConfig(const Json::Value const& jsonObject)
{
    Json::Value dftProcJsonObj = jsonObject["DFTProcessor"];
    bool useDFTProcess = dftProcJsonObj["dftProcess"].asBool();
}

void JsonParser::setMTFPostProcConfig(const Json::Value const& jsonObject)
{
    Json::Value mtfProcJsonObj = jsonObject["MTFPostProcessor"];
    bool anglecorrect = mtfProcJsonObj["anglecorrect"].asBool();
    bool smoothcorrect = mtfProcJsonObj["smoothcorrect"].asBool();
    bool ccdcorrect = mtfProcJsonObj["ccdcorrect"].asBool();
  
    bool derivationcorrect = mtfProcJsonObj["derivationcorrect"].asBool();
    bool mtfsyscorrect = mtfProcJsonObj["mtfsyscorrect"].asBool();
    MTFPostProcessorConfig::anglecorrect = anglecorrect;
    MTFPostProcessorConfig::ccdcorrect = ccdcorrect;
    MTFPostProcessorConfig::smoothcorrect = smoothcorrect;
    MTFPostProcessorConfig::derivationcorrect = derivationcorrect;
    MTFPostProcessorConfig::mtfsyscorrect = mtfsyscorrect;
}
void JsonParser::setOverSampleProcConfig(const Json::Value const& jsonObject) {
    Json::Value oversampleProcJsonObj = jsonObject["OverSampleProcessor"];
    bool  forwardProjection=oversampleProcJsonObj["forwardProjection"].asBool();
    OverSampleProcessorConfig::forwardProjection = forwardProjection;
}

void JsonParser::setPreProcJsonVal(Json::Value&jsonObject)
{
    Json::Value preProcJsonObj;
    preProcJsonObj["isDarkNoise"] = PreProcessorconfig::isDarkNoise;
    preProcJsonObj["isPixelization"] = PreProcessorconfig::isPixelization;
    preProcJsonObj["meanWinSize"] = PreProcessorconfig::meanWinSize;
    preProcJsonObj["hammingWind"] = PreProcessorconfig::hammingWind;
    jsonObject["PreProcessor"] = preProcJsonObj;
}
void JsonParser::setFitProcJsonVal(Json::Value& jsonObject) {
    Json::Value fitProcJsonObj;
    fitProcJsonObj["polynominalOrder"]= FitProcessorConfig::polynominalOrder;
    fitProcJsonObj["distortioncorrect"] = FitProcessorConfig::distortioncorrect;
    jsonObject["FitProcessor"] = fitProcJsonObj;
 
}
void JsonParser::setLSFProcJsonVal(Json::Value& jsonObject) {
    Json::Value lsfProcJsonObj;
    lsfProcJsonObj["meanWindowSize"] = LSFProcessorConfig::meanWindowSize;
    lsfProcJsonObj["hammingWindow"] = LSFProcessorConfig::hammingWindow;
    jsonObject["LSFProcessor"] = lsfProcJsonObj;
}
void JsonParser::setESFProcJsonVal(Json::Value& jsonObject) {
    Json::Value esfProcJsonObj;
    esfProcJsonObj["meanWindow"] = ESFProcessorConfig::meanWindow;
    esfProcJsonObj["reverse"] = ESFProcessorConfig::reverse;
    jsonObject["ESFProcessor"] = esfProcJsonObj;
}
void JsonParser::setDFTProcJsonVal(Json::Value& jsonObject) {
    Json::Value dftProcJsonObj;
    dftProcJsonObj["dftProcess"] = DFTProcessorConfig::dftProcess;
    jsonObject["DFTProcessor"] = dftProcJsonObj;
}
void JsonParser::setOverSampleProcJsonVal(Json::Value& jsonObject) {
    Json::Value oversampleProcJsonObj;
    oversampleProcJsonObj["forwardProjection"] = OverSampleProcessorConfig::forwardProjection;
    jsonObject["OverSampleProcessor"] = oversampleProcJsonObj;
}
void JsonParser::setMTFPostProcJsonVal(Json::Value& jsonObject) {
    Json::Value mtfProcJsonObj;
    mtfProcJsonObj["anglecorrect"]= MTFPostProcessorConfig::anglecorrect;
    mtfProcJsonObj["smoothcorrect"]= MTFPostProcessorConfig::smoothcorrect;
    mtfProcJsonObj["ccdcorrect"] = MTFPostProcessorConfig::ccdcorrect;
    mtfProcJsonObj["derivationcorrect"] = MTFPostProcessorConfig::derivationcorrect;
    mtfProcJsonObj["mtfsyscorrect"] = MTFPostProcessorConfig::mtfsyscorrect;
    jsonObject["MTFPostProcessor"] = mtfProcJsonObj;
}
