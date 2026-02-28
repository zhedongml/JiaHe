#include "ml_hydrus_2config.h"
#include <fstream>			  
#include <fstream>
#include <iostream>
#include <json.hpp>
/* #include <QtDebug>
 #include <QFile>
 #include <QTextStream>	*/
 //#include <QDebug>
using namespace std;
using Json = nlohmann::json;
using namespace algorithm;
double HydrusParameters::magnification = 9.25;
double HydrusParameters::pixel_size = 4.6e-3;
double HydrusParameters::FocalLength = 37.8;
double HydrusParameters::LuminaceActive = 0.9;
double HydrusParameters::RolloffLength = 7.25;
int HydrusParameters::RolloffWidth = 10;
double HydrusParameters::crArea = 0.8;
int HydrusParameters::chessAlgortithm = 2;
int HydrusParameters::singlecheckerWidth = 100;
int HydrusParameters::pointsClusters = 25;
int HydrusParameters::xyClassification = 50;
int HydrusParameters::roiStart = 0;
int HydrusParameters::downSampling = 24;
int HydrusParameters::brightnesslevel = 1000;
int HydrusParameters::gridAlgorthm = 1;
int HydrusParameters::distortionAlgorithm = 2;
int HydrusParameters::deadPointsAlgorithm = 4;
int HydrusParameters::crosshairAlgortihm = 4;
int HydrusParameters::roioffset = 40;
int HydrusParameters::roiwidth = 30;
int HydrusParameters::roiheight = 50;
int HydrusParameters::degrees = 12;
double HydrusParameters::W_ND0_baselineAngle = -0.761268;
double HydrusParameters::W_ND1_baselineAngle = -1.1415;
double HydrusParameters::W_ND2_baselineAngle = -0.571029;
double HydrusParameters::W_ND3_baselineAngle = -0.571029;
double HydrusParameters::X_ND0_baselineAngle = -1.1415;
double HydrusParameters::X_ND1_baselineAngle = -0.568266;
double HydrusParameters::X_ND2_baselineAngle = -0.574995;
double HydrusParameters::X_ND3_baselineAngle = -0.574995;
double HydrusParameters::Y_ND0_baselineAngle = -1.14168;
double HydrusParameters::Y_ND1_baselineAngle = -0.568266;
double HydrusParameters::Y_ND2_baselineAngle = -0.7701;
double HydrusParameters::Y_ND3_baselineAngle = -1.14168;
double HydrusParameters::Z_ND0_baselineAngle = -0.568266;
double HydrusParameters::Z_ND1_baselineAngle = -0.568266;
double HydrusParameters::Z_ND2_baselineAngle = -0.568266;
double HydrusParameters::Z_ND3_baselineAngle = -0.568266;
double HydrusParameters::roationAngle = 0;

cv::Point2f HydrusParameters::W_ND0_opticalcenter = { 3948.04, 3045.93 };
cv::Point2f HydrusParameters::W_ND1_opticalcenter = { 3947.46, 3046.05 };
cv::Point2f HydrusParameters::W_ND2_opticalcenter = { 3948.46, 3045.52 };
cv::Point2f HydrusParameters::W_ND3_opticalcenter = { 3947.46, 3046.05 };
cv::Point2f HydrusParameters::X_ND0_opticalcenter = { 3946.46, 3046.07 };
cv::Point2f HydrusParameters::X_ND1_opticalcenter = { 3945.47, 3046.55 };
cv::Point2f HydrusParameters::X_ND2_opticalcenter = { 3947.46, 3045.53 };
cv::Point2f HydrusParameters::X_ND3_opticalcenter = { 3945.91, 3045.54 };
cv::Point2f HydrusParameters::Y_ND0_opticalcenter = { 3946.47, 3046.54 };
cv::Point2f HydrusParameters::Y_ND1_opticalcenter = { 3946.47, 3046.54 };
cv::Point2f HydrusParameters::Y_ND2_opticalcenter = { 3947.25, 3046.03 };
cv::Point2f HydrusParameters::Y_ND3_opticalcenter = { 3946.47, 3046.54 };
cv::Point2f HydrusParameters::Z_ND0_opticalcenter = { 3947.47, 3046.53 };
cv::Point2f HydrusParameters::Z_ND1_opticalcenter = { 3946.93, 3046.53 };
cv::Point2f HydrusParameters::Z_ND2_opticalcenter = { 3947.47, 3046.53 };
cv::Point2f HydrusParameters::Z_ND3_opticalcenter = { 3947.47, 3046.53 };
//cv::Rect HydrusParameters::ROI(2500, 2200, 3000, 3000);
//cv::Rect HydrusParameters::ROI(2900, 1800, 3050, 3050);
cv::Rect HydrusParameters::ROI(2800, 2000, 3050, 3050);
double HydrusParameters::kET = 0;
double HydrusParameters::bET = 0;

        bool MLHydrusConfig::isInitFromJson = false;

algorithm::MLHydrusConfig::MLHydrusConfig()
{
    if (!MLHydrusConfig::isInitFromJson)
    {
        if (DEBUG==1)
        {
             loadJsonConfig("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\Hydrus2Algorithmconfig.json");
        }
         else
        {
             loadJsonConfig("./config/Hydrus2Algorithmconfig.json");
        }
       MLHydrusConfig::isInitFromJson = true;
    }
}

algorithm::MLHydrusConfig::~MLHydrusConfig()
{
}

void algorithm::MLHydrusConfig::loadJsonConfig(const char *path)
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
                HydrusParameters::magnification = systemJson["magnification"].get<double>();
                HydrusParameters::pixel_size = systemJson["pixel_size"].get<double>();
                HydrusParameters::FocalLength = systemJson["FocalLength"].get<double>();
                std::vector<float> W_ND0_opticalCenters = systemJson["W_ND0_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::W_ND0_opticalcenter = cv::Point2f(W_ND0_opticalCenters.at(0), W_ND0_opticalCenters.at(1));
                std::vector<float> X_ND0_opticalCenters = systemJson["X_ND0_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::X_ND0_opticalcenter = cv::Point2f(X_ND0_opticalCenters.at(0), X_ND0_opticalCenters.at(1));
                std::vector<float> X_ND1_opticalCenters = systemJson["X_ND1_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::X_ND1_opticalcenter = cv::Point2f(X_ND1_opticalCenters.at(0), X_ND1_opticalCenters.at(1));
                std::vector<float> X_ND2_opticalCenters = systemJson["X_ND2_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::X_ND2_opticalcenter = cv::Point2f(X_ND2_opticalCenters.at(0), X_ND2_opticalCenters.at(1));
                std::vector<float> X_ND3_opticalCenters = systemJson["X_ND3_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::X_ND3_opticalcenter = cv::Point2f(X_ND3_opticalCenters.at(0), X_ND3_opticalCenters.at(1));
                std::vector<float> Y_ND0_opticalCenters = systemJson["Y_ND0_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Y_ND0_opticalcenter = cv::Point2f(Y_ND0_opticalCenters.at(0), Y_ND0_opticalCenters.at(1));
                std::vector<float> Y_ND1_opticalCenters = systemJson["Y_ND1_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Y_ND1_opticalcenter = cv::Point2f(Y_ND1_opticalCenters.at(0), Y_ND1_opticalCenters.at(1));
                std::vector<float> Y_ND2_opticalCenters = systemJson["Y_ND2_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Y_ND2_opticalcenter = cv::Point2f(Y_ND2_opticalCenters.at(0), Y_ND2_opticalCenters.at(1));
                std::vector<float> Y_ND3_opticalCenters = systemJson["Y_ND3_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Y_ND3_opticalcenter = cv::Point2f(Y_ND3_opticalCenters.at(0), Y_ND3_opticalCenters.at(1));
                std::vector<float> Z_ND0_opticalCenters = systemJson["Z_ND0_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Z_ND0_opticalcenter = cv::Point2f(Z_ND0_opticalCenters.at(0), Z_ND0_opticalCenters.at(1));
                std::vector<float> Z_ND1_opticalCenters = systemJson["Z_ND1_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Z_ND1_opticalcenter = cv::Point2f(Z_ND1_opticalCenters.at(0), Z_ND1_opticalCenters.at(1));
                std::vector<float> Z_ND2_opticalCenters = systemJson["Z_ND2_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Z_ND2_opticalcenter = cv::Point2f(Z_ND2_opticalCenters.at(0), Z_ND2_opticalCenters.at(1));
                std::vector<float> Z_ND3_opticalCenters = systemJson["Z_ND3_opticalcenter"].get<std::vector<float>>();
                HydrusParameters::Z_ND3_opticalcenter = cv::Point2f(Z_ND3_opticalCenters.at(0), Z_ND3_opticalCenters.at(1));
                std::vector<int> rect = systemJson["ROI"].get<std::vector<int>>();
                HydrusParameters::ROI = cv::Rect(rect.at(0), rect.at(1), rect.at(2), rect.at(3));
                HydrusParameters::roationAngle = systemJson["rotaionAngle"].get<double>();
            }
            {
                Json& crosshairJson = settingJsonObj["crosshair"];
                HydrusParameters::roiStart = crosshairJson["roiStart"].get<int>();
                HydrusParameters::crosshairAlgortihm = crosshairJson["crosshairAlgortihm"].get<int>();
            }
            {
                Json& crAreaPerJson = settingJsonObj["CRAreaPercent"];
                HydrusParameters::crArea = crAreaPerJson["crArea"].get<double>();

                Json& luminanceJson = settingJsonObj["Luminance"];
                HydrusParameters::LuminaceActive = luminanceJson["LuminaceActive"].get<double>();

                Json& luminanceRolloffJson = settingJsonObj["LuminanceRolloff"];
                HydrusParameters::RolloffLength = luminanceRolloffJson["RolloffLength"].get<double>();
                HydrusParameters::RolloffWidth = luminanceRolloffJson["RolloffWidth"].get<int>();

                Json& whitepointJson = settingJsonObj["whitepoint"];
                HydrusParameters::downSampling = whitepointJson["downSampling"].get<int>();
                HydrusParameters::degrees = whitepointJson["degrees"].get<int>();
                HydrusParameters::brightnesslevel = whitepointJson["brightnesslevel"].get<int>();
              
                Json& gridAlgJson = settingJsonObj["gridDetectAlgorithm"];
                HydrusParameters::gridAlgorthm = gridAlgJson["gridAlgorthm"].get<int>();
               
                Json& distortAlgJson = settingJsonObj["distortionAlgorithm"];
                HydrusParameters::distortionAlgorithm = distortAlgJson["distortionAlgorithm"].get<int>();

                Json& deadPointsAlgJson = settingJsonObj["deadPointsAlgorithm"];
                HydrusParameters::deadPointsAlgorithm = deadPointsAlgJson["deadPointsAlgorithm"].get<int>();

                Json& baseAngleJson = settingJsonObj["imageRotation"];
                HydrusParameters::roioffset = baseAngleJson["roioffset"].get<int>();
                HydrusParameters::roiwidth = baseAngleJson["roiwidth"].get<int>();
                HydrusParameters::roiheight = baseAngleJson["roiheight"].get<int>();
                HydrusParameters::W_ND0_baselineAngle = baseAngleJson["W_ND0_baselineAngle"].get<double>();
                HydrusParameters::W_ND1_baselineAngle = baseAngleJson["W_ND1_baselineAngle"].get<double>();
                HydrusParameters::W_ND2_baselineAngle = baseAngleJson["W_ND2_baselineAngle"].get<double>();
                HydrusParameters::W_ND3_baselineAngle = baseAngleJson["W_ND3_baselineAngle"].get<double>();
                HydrusParameters::X_ND0_baselineAngle = baseAngleJson["X_ND0_baselineAngle"].get<double>();
                HydrusParameters::X_ND1_baselineAngle = baseAngleJson["X_ND1_baselineAngle"].get<double>();
                HydrusParameters::X_ND2_baselineAngle = baseAngleJson["X_ND2_baselineAngle"].get<double>();
                HydrusParameters::X_ND3_baselineAngle = baseAngleJson["X_ND3_baselineAngle"].get<double>();
                HydrusParameters::Y_ND0_baselineAngle = baseAngleJson["Y_ND0_baselineAngle"].get<double>();
                HydrusParameters::Y_ND1_baselineAngle = baseAngleJson["Y_ND1_baselineAngle"].get<double>();
                HydrusParameters::Y_ND2_baselineAngle = baseAngleJson["Y_ND2_baselineAngle"].get<double>();
                HydrusParameters::Y_ND3_baselineAngle = baseAngleJson["Y_ND3_baselineAngle"].get<double>();
                HydrusParameters::Z_ND0_baselineAngle = baseAngleJson["Z_ND0_baselineAngle"].get<double>();
                HydrusParameters::Z_ND1_baselineAngle = baseAngleJson["Z_ND1_baselineAngle"].get<double>();
                HydrusParameters::Z_ND2_baselineAngle = baseAngleJson["Z_ND2_baselineAngle"].get<double>();
                HydrusParameters::Z_ND3_baselineAngle = baseAngleJson["Z_ND3_baselineAngle"].get<double>();
            }
            {
                Json &checkerboradJson = settingJsonObj["checkerboard"];
                HydrusParameters::chessAlgortithm = checkerboradJson["chessAlgortithm"].get<int>();
                HydrusParameters::singlecheckerWidth = checkerboradJson["singlecheckerWidth"].get<int>();
                HydrusParameters::pointsClusters = checkerboradJson["pointsClusters"].get<int>();
                HydrusParameters::xyClassification = checkerboradJson["xyClassification"].get<int>();
                HydrusParameters::kET = checkerboradJson["kET"].get<double>();
                HydrusParameters::bET = checkerboradJson["bET"].get<double>();


            }
        }
    

}

Newpara algorithm::MLHydrusConfig::GetNewPara(cv::Mat img)
{ 
        int multiple = 1;
        cv::Rect rect = cv::Rect(0, 0, -1, -1);
        multiple = sqrt(7920*6004/img.total());
        int multiple1 = sqrt(7920 * 6004 / img.total());
        if (multiple == multiple1)
        {
            rect.x = HydrusParameters::ROI.x / multiple;
            rect.y = HydrusParameters::ROI.y / multiple;
            rect.width = HydrusParameters::ROI.width / multiple;
            rect.height = HydrusParameters::ROI.height / multiple;
        }
        double m_pixel_size = HydrusParameters::pixel_size;
        m_para.rect = rect;
        m_para.binNum = multiple;
        m_para.pixel_size = m_pixel_size * multiple;
      //  cout << "loadjason sucessfully" << endl;
        return m_para;
    
}
