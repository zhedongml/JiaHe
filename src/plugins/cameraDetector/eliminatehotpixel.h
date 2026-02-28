#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>


#define VIMBACAMERA_API __declspec(dllexport)

using namespace std;
struct PixPostion {
	int x = 0;
	int y = 0;
	int pixR = 0;
	int neighThick = 1;
};
class VIMBACAMERA_API EliminateHotPixel
{

public:
	static EliminateHotPixel* getInstance();
	cv::Mat deHotAndDeadPixel(cv::Mat rawImg, int cameraName);
	cv::Mat deHotAndDeadPixelByInpaint(cv::Mat& rawImg, int cameraName);
	cv::Mat deHotAndDeadPixelByNeighbourVal(cv::Mat& rawImg, int cameraName);
	void loadHotAndDeadPixelCfg(string fileName, int cameraName);
	void setDeHotPixMode(int type);
private:
	EliminateHotPixel();
	~EliminateHotPixel();
private:
	map<int,vector<PixPostion>> hotpixelInfo;
	int getNeighbourVal(cv::Mat& rawImg,int x,int y,int pixR = 0,int neighThickness =1);
	static EliminateHotPixel* self;
	int mode = 0;
};

