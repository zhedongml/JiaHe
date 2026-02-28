#include "pch.h"
#include "IQMetricUtl.h"
#include <json.hpp>
#include<fstream>
#include"ml_image_public.h"
using namespace std;
using Json = nlohmann::json;
using namespace MLImageDetection;
using namespace MLIQMetrics;
double IQMetricsParameters::magnification = 9.25;
double IQMetricsParameters::pixel_size = 3.2e-3;
double IQMetricsParameters::FocalLength = 40;
double IQMetricsParameters::crArea = 0.25;
double IQMetricsParameters::LuminaceActive = 0.9;
double IQMetricsParameters::RolloffLength = 21.25;
double IQMetricsParameters::RolloffLengthH = 21.25;
double IQMetricsParameters::RolloffLengthV = 16.25;
int IQMetricsParameters::RolloffWidth = 10;
double IQMetricsParameters::RolloffAreaRatio = 0.9;
cv::Point2f IQMetricsParameters::opticalCenter = {5632,4599};//{2192,3288};
cv::Rect IQMetricsParameters::ROIRect = {0,0,-1,-1};
double IQMetricsParameters::CTFROIHeight = 1;
double IQMetricsParameters::CTFROIWidth = 1;
int IQMetricsParameters::denseMTFWidth = 300;
int IQMetricsParameters::denseMTFHeight = 50;
int IQMetricsParameters::crossMTFHeight = 50;
int IQMetricsParameters::crossMTFWidth= 320;
int IQMetricsParameters::crossMTFHeightBig = 350;
int IQMetricsParameters::crossMTFWidthBig = 1630;
int IQMetricsParameters::gridMTFHeight = 50;
int IQMetricsParameters::gridMTFWidth = 300;
int IQMetricsParameters::checkerMTFWidth_2 = 350;
int IQMetricsParameters::checkerMTFHeight_2 = 150;
int IQMetricsParameters::checkerMTFWidth_05 = 80;
int IQMetricsParameters::checkerMTFHeight_05 = 40;
int IQMetricsParameters::ROI_center_offset = 80;
int IQMetricsParameters::ROI_center_offset_big = 400;

double IQMetricsParameters::mtfFreq =10.625;
double IQMetricsParameters::mtfFreq2 = 14.167;
double IQMetricsParameters::zoneARadius = 5;
vector<cv::Rect> IQMetricsParameters::ghostRectVec= { { 5610,1980,1000,1000},{ 3100,1300,1000,1000 },{ 6500,700,1000,1000 },{ 3400,160,1000,1000 } };
double IQMetricsParameters::CheckerDensity = 0.5;
int IQMetricsParameters::crossBinNum = 16;
vector<double>IQMetricsParameters::xsecVec = { 2,4 };
double IQMetricsParameters::xsecWidth = 3;
int IQMetricsParameters::avg_width = 5;
double IQMetricsParameters::flareAngle = 5;
double IQMetricsParameters::distortonTheHor = 14.1;
double IQMetricsParameters::distortonTheVer = 14.1;
double IQMetricsParameters::distortonTheHorBig = 58;
double IQMetricsParameters::distortonTheVerBig = 44;
double IQMetricsParameters::CheckerRECTMaskHor = 20;
double IQMetricsParameters::CheckerRECTMaskVer = 20;
double IQMetricsParameters::CheckerCIRCLEMaskRadius = 10;
int IQMetricsParameters::ResizeNum = 4;
double IQMetricsParameters::DUTRotationAngle = 1.9615;

bool IQMetricUtl::isInitFromJson = false;
IQMetricUtl* IQMetricUtl::self=nullptr;


IQMetricUtl::IQMetricUtl()
{
    if (!IQMetricUtl::isInitFromJson)
    {
        MLimagePublic pl;
 
        string filepath = "./config/IQMetricsParametersConfig.json";
        loadJsonConfig(filepath.c_str());        
        IQMetricUtl::isInitFromJson = true;
    }
}
IQMetricUtl::~IQMetricUtl()
{
}
IQMetricUtl* IQMetricUtl::instance()
{
	if (self == nullptr) {
		self = new IQMetricUtl();
	}
	return self;
    //return nullptr;
}

cv::Rect IQMetricUtl::getRect(ROIParaNew para, cv::Point2f center)
{
    int width = IQMetricsParameters::FocalLength * tan(para.width / 2 * CV_PI / 180.0) / IQMetricsParameters::pixel_size * 2;
    int height= IQMetricsParameters::FocalLength * tan(para.height / 2 * CV_PI / 180.0) / IQMetricsParameters::pixel_size * 2;
    cv::Rect rect;
    if (para.rotationAngle == 90 || para.rotationAngle == 270)
    {
        swap(height, width); 
    }
    rect.x = center.x - width / 2;
    rect.y = center.y - height / 2;
    rect.width = width;
    rect.height = height;
    return rect;
}

int MLIQMetrics::IQMetricUtl::getBinNum(cv::Size s)
{
    int num = round (sqrt((13376*9528.0/(s.area()))));   
    return num;
}

double MLIQMetrics::IQMetricUtl::getPix2Arcmin(cv::Size s)
{
    int binNum = getBinNum(s);
    double pixel = IQMetricsParameters::pixel_size;
    double focallength = IQMetricsParameters::FocalLength;
    double pixelPerDeg = atan(pixel * binNum / focallength) * 180.0 / CV_PI * 60;
    return pixelPerDeg;
}

double MLIQMetrics::IQMetricUtl::getPix2Degree(cv::Size s)
{
    int binNum = getBinNum(s);
    double pixel = IQMetricsParameters::pixel_size;
    double focallength = IQMetricsParameters::FocalLength;
    double pixelPerDeg = atan(pixel * binNum / focallength) * 180.0 / CV_PI;
    return pixelPerDeg;
}

void IQMetricUtl::loadJsonConfig(const char* path)
{
    std::ifstream jsonFile(path);
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        Json settingJsonObj = Json::parse(contents);
        {
            Json& systemJson = settingJsonObj["system"];
            //HuiLingDunParameters::magnification = systemJson["magnification"].get<double>();
            IQMetricsParameters::pixel_size = systemJson["pixel_size"].get<double>();
            IQMetricsParameters::FocalLength = systemJson["FocalLength"].get<double>();
            std::vector<float> W_ND0_opticalCenters = systemJson["W_opticalcenter"].get<std::vector<float>>();
            IQMetricsParameters::opticalCenter = cv::Point2f(W_ND0_opticalCenters.at(0), W_ND0_opticalCenters.at(1));
            std::vector<int> rect = systemJson["ROI"].get<std::vector<int>>();
            IQMetricsParameters::ROIRect = cv::Rect(rect.at(0), rect.at(1), rect.at(2), rect.at(3));
            //IQMetricsParameters::rotationAngle = systemJson["rotaionAngle"].get<double>();
            IQMetricsParameters::ResizeNum = systemJson["ResizeNum"].get<int>();
            IQMetricsParameters::DUTRotationAngle= systemJson["DUTRotationAngle"].get<double>();

        }
        {
            Json& crAreaPerJson = settingJsonObj["checkerContrast"];
            IQMetricsParameters::crArea = crAreaPerJson["crArea"].get<double>();  
        }
 
        {
            Json& mtfPerJson = settingJsonObj["mtfFreq"];
            IQMetricsParameters::mtfFreq = mtfPerJson["mtfFreq"].get<double>();
            IQMetricsParameters::mtfFreq2 = mtfPerJson["mtfFreq2"].get<double>();

        }
        {
            Json& mtfPerJson = settingJsonObj["denseMTF"];
            IQMetricsParameters::denseMTFWidth = mtfPerJson["denseMTFWidth"].get<double>();
            IQMetricsParameters::denseMTFHeight = mtfPerJson["denseMTFHeight"].get<double>();
        }
        {
            Json& mtfPerJson = settingJsonObj["GridMTF"];
            IQMetricsParameters::gridMTFWidth = mtfPerJson["gridMTFWidth"].get<double>();
            IQMetricsParameters::gridMTFHeight = mtfPerJson["gridMTFHeight"].get<double>();
        }
        {
            Json& mtfPerJson = settingJsonObj["CrossMTF"];
            IQMetricsParameters::crossMTFWidth = mtfPerJson["crossMTFWidth"].get<double>();
            IQMetricsParameters::crossMTFHeight = mtfPerJson["crossMTFHeight"].get<double>();
            IQMetricsParameters::ROI_center_offset = mtfPerJson["ROI_center_offset"].get<double>();
            IQMetricsParameters::crossMTFWidthBig = mtfPerJson["crossMTFWidthBig"].get<double>();
            IQMetricsParameters::crossMTFHeightBig = mtfPerJson["crossMTFHeightBig"].get<double>();
            IQMetricsParameters::ROI_center_offset_big = mtfPerJson["ROI_center_offset_big"].get<double>();
        }
        {
            Json& mtfPerJson = settingJsonObj["distortion"];
            IQMetricsParameters::distortonTheHor = mtfPerJson["distortonTheHor"].get<double>();
            IQMetricsParameters::distortonTheVer = mtfPerJson["distortonTheVer"].get<double>();
            IQMetricsParameters::distortonTheHorBig = mtfPerJson["distortonTheHorBig"].get<double>();
            IQMetricsParameters::distortonTheVerBig = mtfPerJson["distortonTheVerBig"].get<double>();
        }
        {
            Json& mtfPerJson = settingJsonObj["flare"];
            IQMetricsParameters::avg_width = mtfPerJson["avg_width"].get<double>();
            IQMetricsParameters::flareAngle = mtfPerJson["flareAngle"].get<double>();
            IQMetricsParameters::xsecWidth = mtfPerJson["xsecWidth"].get<double>();
            std::vector<double> rect = mtfPerJson["xsecVec"].get<std::vector<double>>();
            IQMetricsParameters::xsecVec = rect;
        }

    }
}

string MLIQMetrics::IQMetricUtl::fovTypeToString(FOVTYPE type)
{
    switch (type) {
    case FOVTYPE::BIGFOV:   return "BIGFOV";
    case FOVTYPE::SMALLFOV: return "SMALLFOV";
    }
    return "UNKNOWN";
}
