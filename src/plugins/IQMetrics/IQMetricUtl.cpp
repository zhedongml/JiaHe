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
cv::Rect IQMetricsParameters::ROIRect = { 2500,800,6800,4800 };//{2500,800,6800,4800};
double IQMetricsParameters::CTFROIHeight = 1;
double IQMetricsParameters::CTFROIWidth = 1;
int IQMetricsParameters::denseMTFWidth = 330;
int IQMetricsParameters::denseMTFHeight = 110;
int IQMetricsParameters::crossMTFHeight = 350;
int IQMetricsParameters::crossMTFWidth= 700;
int IQMetricsParameters::gridMTFHeight = 110;
int IQMetricsParameters::gridMTFWidth = 330;
int IQMetricsParameters::ROI_center_offset = 350;
double IQMetricsParameters::mtfFreq = 6.25;
double IQMetricsParameters::mtfFreq2 = 12.5;
double IQMetricsParameters::zoneARadius = 5;
vector<cv::Rect> IQMetricsParameters::ghostRectVec= { { 5610,1980,1000,1000},{ 3100,1300,1000,1000 },{ 6500,700,1000,1000 },{ 3400,160,1000,1000 } };
double IQMetricsParameters::CheckerDensity = 0.5;
int IQMetricsParameters::crossBinNum = 16;
vector<double>IQMetricsParameters::xsecVec = { 2,4 };
double IQMetricsParameters::xsecWidth = 3;
int IQMetricsParameters::avg_width = 5;
double IQMetricsParameters::flareAngle = 5;
double IQMetricsParameters::flareRotationAngle = 0;
double IQMetricsParameters::distortonTheHor = 21;
double IQMetricsParameters::distortonTheVer = 27;
double IQMetricsParameters::CheckerRECTMaskHor = 20;
double IQMetricsParameters::CheckerRECTMaskVer = 20;
double IQMetricsParameters::CheckerCIRCLEMaskRadius = 10;
int IQMetricsParameters::GridResizeNum = 4;
int IQMetricsParameters::SolidResizeNum = 4;
bool IQMetricsParameters::IsUseNewMethod = true;
double IQMetricsParameters::SystemRotationError = 0;
double IQMetricsParameters::SLBRotationAngle = 90;
double IQMetricsParameters::DUTRotationAngle = 90;
bool IQMetricsParameters::SLBFlipLR = true;
bool IQMetricsParameters::DUTFlipLR = false;
bool IQMetricsParameters::SLBFlipUD = false;
bool IQMetricsParameters::DUTFlipUD = true;
cv::Rect2f IQMetricsParameters::zoneBRect = {1.15,-3,17.5,9.5};
cv::Size IQMetricsParameters::CheckerSize = {11,14};
cv::Size IQMetricsParameters::GridSize = {15,19};
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

cv::Rect MLIQMetrics::IQMetricUtl::getZoneRect(cv::Rect2f rect, cv::Point2f center, int binNum)
{
    cv::Rect2f zoneMRect = rect;
    double hor = zoneMRect.width;
    double ver = zoneMRect.height;
    double pixel = IQMetricsParameters::pixel_size * binNum;
    double focallengh = IQMetricsParameters::FocalLength;
    double offsetx = tan(zoneMRect.x * CV_PI / 180) * focallengh / pixel;
    double offsety = tan(zoneMRect.y * CV_PI / 180) * focallengh / pixel;
    center = center + cv::Point2f(offsetx, offsety);
    double w = tan(hor / 2 / 180.0 * CV_PI) * focallengh / pixel * 2;
    double h = tan(ver / 2 / 180.0 * CV_PI) * focallengh / pixel * 2;
    cv::Rect rect0(center.x-w/2, center.y - h / 2, w, h);
    return rect0;
}



cv::Mat MLIQMetrics::IQMetricUtl::getRotationAndFlipImg(cv::Mat img, bool isSLB)
{
    MLImageDetection::MLimagePublic pl;
    cv::Mat imgrf;
    if (isSLB)
    {
        //cv::Mat imgR = pl.getRotationImg(img, IQMetricsParameters::SLBRotationAngle);
        cv::Mat imgR;
        rotateImg(img, imgR, IQMetricsParameters::SLBRotationAngle);
        imgrf = pl.flipImg(imgR, IQMetricsParameters::SLBFlipLR, IQMetricsParameters::SLBFlipUD);  // 控制SLB 图的旋转及翻转

    }
    else
    {
        //cv::Mat imgR = pl.getRotationImg(img, IQMetricsParameters::DUTRotationAngle);
        cv::Mat imgR;
        rotateImg(img, imgR, IQMetricsParameters::SLBRotationAngle);
        imgrf = pl.flipImg(imgR, IQMetricsParameters::DUTFlipLR, IQMetricsParameters::DUTFlipUD);
    }
    return imgrf;
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
    double focallength = IQMetricsParameters::FocalLength;   //焦距
    double pixelPerDeg = atan(pixel * binNum / focallength) * 180.0 / CV_PI;  // 像素在光学系统中对应的视场角 ji
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
            IQMetricsParameters::GridResizeNum = systemJson["GridResizeNum"].get<int>();
            IQMetricsParameters::SolidResizeNum = systemJson["SolidResizeNum"].get<int>();
            int flag = systemJson["IsUseNewMethod"].get<int>();
            if (flag == 0)
                IQMetricsParameters::IsUseNewMethod = false;
            else
                IQMetricsParameters::IsUseNewMethod = true;
            IQMetricsParameters::SystemRotationError = systemJson["SystemRotationError"].get<double>();
            IQMetricsParameters::SLBRotationAngle = systemJson["SLBRotationAngle"].get<double>();
            IQMetricsParameters::DUTRotationAngle = systemJson["DUTRotationAngle"].get<double>();
            IQMetricsParameters::SLBFlipLR = systemJson["SLBFlipLR"].get<bool>();
            IQMetricsParameters::DUTFlipLR = systemJson["DUTFlipLR"].get<bool>();
            IQMetricsParameters::SLBFlipUD = systemJson["SLBFlipUD"].get<bool>();
            IQMetricsParameters::DUTFlipUD = systemJson["DUTFlipUD"].get<bool>();              
            std::vector<double> rectZoneB = systemJson["zoneBRect"].get<std::vector<double>>();
            IQMetricsParameters::zoneBRect = cv::Rect2f(rectZoneB.at(0), rectZoneB.at(1), rectZoneB.at(2), rectZoneB.at(3));
            std::vector<int> sizeVec = systemJson["CheckerSize"].get<std::vector<int>>();
            std::vector<int> sizeVec1 = systemJson["GridSize"].get<std::vector<int>>();
            IQMetricsParameters::CheckerSize = cv::Size(sizeVec[0], sizeVec[1]);
            IQMetricsParameters::GridSize = cv::Size(sizeVec1[0], sizeVec1[1]);
            if (IQMetricsParameters::SLBRotationAngle == 90 || IQMetricsParameters::SLBRotationAngle == 270)
            {
                IQMetricsParameters::CheckerSize = cv::Size(sizeVec[1], sizeVec[0]);
                IQMetricsParameters::GridSize = cv::Size(sizeVec1[1], sizeVec1[0]);
            }
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
        }
        {
            Json& mtfPerJson = settingJsonObj["distortion"];
            IQMetricsParameters::distortonTheHor = mtfPerJson["distortonTheHor"].get<double>();
            IQMetricsParameters::distortonTheVer = mtfPerJson["distortonTheVer"].get<double>();
        }
        {
            Json& mtfPerJson = settingJsonObj["flare"];
            IQMetricsParameters::avg_width = mtfPerJson["avg_width"].get<double>();
            IQMetricsParameters::flareAngle = mtfPerJson["flareAngle"].get<double>();
            IQMetricsParameters::flareRotationAngle = mtfPerJson["flareRotationAngle"].get<double>();
            IQMetricsParameters::xsecWidth = mtfPerJson["xsecWidth"].get<double>();
            std::vector<double> rect = mtfPerJson["xsecVec"].get<std::vector<double>>();
            IQMetricsParameters::xsecVec = rect;
        }

    }
}

void MLIQMetrics::IQMetricUtl::rotateImg(cv::Mat src, cv::Mat& dst, double angle)
{
    // 如果角度为 0，直接返回原图
    if (abs(angle) < 1e-6)
    {
        dst = src.clone();  // 保持原图不变
        return;
    }
    if (abs(angle - 90) < 1e-6)
        rotate(src, dst, cv::ROTATE_90_CLOCKWISE);
    else if(abs(angle-180)<1e-6)
        rotate(src, dst, cv::ROTATE_180);
    else if(abs(angle-270)<1e-6)
        rotate(src, dst, cv::ROTATE_90_COUNTERCLOCKWISE);
}

string MLIQMetrics::IQMetricUtl::fovTypeToString(FOVTYPE type)
{
    switch (type) {
    case FOVTYPE::BIGFOV:   return "BIGFOV";
    case FOVTYPE::SMALLFOV: return "SMALLFOV";
    }
    return "UNKNOWN";
}
