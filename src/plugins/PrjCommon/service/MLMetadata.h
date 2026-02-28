#pragma once

#include "MLFrame.h"

namespace CORE
{
struct CaptureData
{
	//string sAperture;
	Color colorLight = Color::G;
	std::string moduleName;
	ColorFilter color;
	ND_Filter ndFilter;
	double vid;
	double exposureTime;
	std::string rx;
	MLFrame frame;
};


typedef struct CaptureListData
{
	std::string datatime;

	std::map<std::string , CaptureData> frameList;

} CaptureListData;

typedef struct ColorShiftData
{
	std::string datatime;

	cv::Point2i PointX;
	cv::Point2i PointY;
	cv::Point2i PointZ;

	cv::Mat resultX;
	cv::Mat resultY;
	cv::Mat resultZ;

} ColorShiftData;

typedef struct PostProcessData
{
	std::string datatime;

	Rotation rotate;
	Mirror mirror;
	cv::Mat resultX;
	cv::Mat resultY;
	cv::Mat resultZ;

} PostProcessData;

typedef struct DistortionData
{
	std::string datatime;

	cv::Mat cameraMatrix;
	cv::Mat coefficient;
	cv::Mat resultX;
	cv::Mat resultY;
	cv::Mat resultZ;

} DistortionData;

typedef struct FFCData
{
	std::string datatime;

	cv::Mat resultX;
	cv::Mat resultY;
	cv::Mat resultZ;
} FFCData;

typedef struct FourColorData
{
	std::string datatime;

	cv::Mat R;
	cv::Mat resultxx;
	cv::Mat resultyy;
	cv::Mat resultYY;
    cv::Mat resultXX;
    cv::Mat resultZZ;
} FourColorData;

typedef struct BinningData
{
	std::string datatime;

	int binning;
	cv::Mat result;
} BinningData;


class __declspec(dllexport) MLMetaData
{
public:
	MLMetaData();
	~MLMetaData();

	void SetCaptureData(std::string channel, CaptureData data);

	void SetColorShiftData(ColorShiftData data);

	void SetPostProcessData(PostProcessData data);

	void SetDistortionData(DistortionData data);

	void SetFFCData(FFCData data);

	void SetFourColorData(FourColorData data);

	void SetBinningData(BinningData data);

	void ClearCaptureData();

	bool onlyYChannel();

public:
	CaptureListData m_CaptureData;
	ColorShiftData m_ColorShift;
	PostProcessData m_PostProcess;
	DistortionData m_Distortion;
	FFCData m_FFC;
	FourColorData m_FourColor;
	BinningData m_Binning;

};

extern "C" std::string __declspec(dllexport) CurrentDatetimeToString();
} // namespace CORE