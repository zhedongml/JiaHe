#pragma once
//#include "pch.h"
#include "CrossCenter.h"
#include <fstream>
#include <json.hpp>
//#include"qlog.h"
using namespace std;
using namespace cv;
using namespace MLImageDetection;
using Json = nlohmann::json;
int CrossCenterParameter::roilength = 200;
int CrossCenterParameter::iterationInterval = 1;
int CrossCenterParameter::asfilterROI = 20;
int CrossCenterParameter::searchlength = 25;
int CrossCenterParameter::dataFilterVer = 60;
int CrossCenterParameter::dataFilterHor = 50;
int CrossCenterParameter::pointdistance = 20;
float CrossCenterParameter::anglediff = 30;
//bool CrossCenter::isInitFromJson = false;
bool CrossCenterParameter::isInitFromJson = false;

void CrossCenter::loadJsonConfig(const char *path)
{
    std::ifstream jsonFile(path);
    if (jsonFile.is_open())
    {
        std::string contents =
            std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
        jsonFile.close();
        Json settingJsonObj = Json::parse(contents);
        {
            Json &cneterGussian = settingJsonObj["cneterGussian"];
            CrossCenterParameter::iterationInterval = cneterGussian["iterationInterval"].get<int>();
            CrossCenterParameter::asfilterROI = cneterGussian["asfilterROI"].get<int>();
            CrossCenterParameter::searchlength = cneterGussian["searchlength"].get<int>();
            CrossCenterParameter::dataFilterVer = cneterGussian["dataFilterVer"].get<int>();
            CrossCenterParameter::dataFilterHor = cneterGussian["dataFilterHor"].get<int>();
        }
        {
            Json &centerMTF = settingJsonObj["centerMTF"];
            CrossCenterParameter::roilength = centerMTF["roilength"].get<int>();
        }
        {
            Json &centerGird = settingJsonObj["centerGird"];
            CrossCenterParameter::pointdistance = centerGird["pointdistance"].get<int>();
        }
        {
            Json &centerLines = settingJsonObj["centerLines"];
            CrossCenterParameter::anglediff = centerLines["anglediff"].get<float>();
        }
    }
}
float STDVector(vector<float> resultSet)
{
    float sum = std::accumulate(std::begin(resultSet), std::end(resultSet), 0.0);
    float mean = sum / resultSet.size(); 

    float accum = 0.0;
    std::for_each(std::begin(resultSet), std::end(resultSet),
                  [&](const double d) { accum += (d - mean) * (d - mean); });

    float stdev = sqrt(accum / (resultSet.size() - 1)); 
    return stdev;
}

void ReduceError(vector<float> &x)
{
    float sum = std::accumulate(std::begin(x), std::end(x), 0.0);
    float mean = sum / x.size();
    float std = STDVector(x);
    vector<float>::iterator x_it;
    for (x_it = x.begin(); x_it != x.end();)
    {
        if (abs(*x_it - mean) > 1.5 * std)
        {
            x_it = x.erase(x_it);
        }
        else
        {
            x_it++;
        }
    }
}

CrossCenter::CrossCenter()
{
}

CrossCenter::~CrossCenter()
{
}

CrossCenter::CrossCenter(Mat img, int off)
{
	this->m_img = img;
	//this->img = img;
    this->off = off;
    m_img_draw = convertTo3Channels(img);
	//m_img = img.clone();
	//this->roiLen = roiLen;
	//QLog::GetInstance().log_init("mydebug.log");
	//if (img.empty())
	//{
	//	QLog::GetInstance().qlog("CrossCenter:img empty/n");
	//}
    if (!CrossCenterParameter::isInitFromJson)
    {
        if (DEBUG==1)
        {
            loadJsonConfig("E:\\project\\hydrus3\\mtfiq\\src\\app\\config\\crosshairsConfig.json");

        }
        else
        {
            loadJsonConfig("./config/crosshairsConfig.json");
        }
        CrossCenterParameter::isInitFromJson = true;
    }
}
cv::Point2f CrossCenter::get_crossCenterMTF()
{
    cv::Point2f center = {0, 0};
    //center = find_centerGaussian(m_img); TODO:borderless crosshair
    //center = find_centerMTF(m_img); //TODO:nine cross
    center = find_centerMTF_manual(m_img);
    return center;
}
Point2f CrossCenter::get_crossCenter(int flag)
{
	cv::Point2f center = { 0,0 };
	if (flag == 1)//houghtLine
	{
		center = get_crossCenterLINES();
	}
	else if (flag == 2) // Gaussian fit
	{
		center = get_crossCenterGaussian();
	}
	else if (flag == 3) // centroid
	{
		center = get_crossCenterDIFF();
	}
	else if (flag == 4)// template
	{
        cv::Mat templa;
        if (DEBUG==1)
        {
            templa = cv::imread( "E:\\project\\hydrus3\\mtfiq\\src\\app\\config\\ALGConfig\\template4.tif");        
        }
        else
        {
             templa = cv::imread("./config/ALGConfig/template4.tif"); 
        }
        
		center = get_crossCenterMATCH(templa);
	}
    else if(flag == 5)
    {
        center = get_crossCenterMTF();
    }
    else if (flag==6)
    {
        center = get_crossCenteGradAndGaussian();
    }
    return center;
}
Point2f CrossCenter::get_crossCenterGaussian()
{
	find_centerGaussian(m_img);
	return m_center;
}

cv::Point2f CrossCenter::get_crossCenteGradAndGaussian()
{
    find_centerGradAndGaussian(m_img);
    return m_center;
}

vector<Point> CrossCenter::find_roi_center(Point2f center, float a1, float b1, float c1, float a2, float b2, float c2)
{
    try
    {
        vector<Point> rcenter;
        float k1 = fabs(a1 / b1);
        float k2 = fabs(a2 / b2);

        float kh = k1 < k2 ? a1 / b1 : a2 / b2;
        float kv = k1 > k2 ? a1 / b1 : a2 / b2;

        float anglev = atan(kv);
        float angleh = atan(kh);
        m_hAngle = angleh * 180 / CV_PI;
        m_vAngle = anglev * 180 / CV_PI;

        if (anglev > 0)
        {
            rcenter.push_back(Point(center.x + cos(anglev) * off, center.y - sin(anglev) * off));
            rcenter.push_back(Point(center.x - cos(anglev) * off, center.y + sin(anglev) * off));
        }
        else
        {
            rcenter.push_back(Point(center.x - cos(anglev) * off, center.y + sin(anglev) * off));
            rcenter.push_back(Point(center.x + cos(anglev) * off, center.y - sin(anglev) * off));
        }

        rcenter.push_back(Point(center.x - cos(angleh) * off, center.y + sin(angleh) * off));
        rcenter.push_back(Point(center.x + cos(angleh) * off, center.y - sin(angleh) * off));

        return rcenter;
    }
    catch (const std::exception &e)
    {
        // QLog::GetInstance().qlog("CrossCenter find_roi_center: %s/n", e.what());

		if (!this->m_img.empty())
		{
			imwrite("1.jpg", this->m_img);
		}
	}
}




vector<double> CrossCenter::GetVerticalLine()
{
    vector<double> ver;
    ver.push_back(m_a1);
    ver.push_back(m_b1);
    ver.push_back(m_c1);
    return ver;
}

vector<double> CrossCenter::GetHorizontalLine()
{
    vector<double> hor;
    hor.push_back(m_a2);
    hor.push_back(m_b2);
    hor.push_back(m_c2);
    return hor;
}

double CrossCenter::GetHorizontalLineTheta()
{
    return m_thetaH;
}

double CrossCenter::GetVerticalLineTheta()
{
    return m_thetaV;
}

Point2f CrossCenter::get_crossCenterDIFF()
{
	find_center(m_img);
	return m_center;
}


Point2f CrossCenter::get_crossCenterLINES()
{
	try
	{

	
	if (abs(find_centerLINES(m_img).x)>1e-6)
	{
		float std1, std2;
		std1 = STDVector(m_angle1);
		std2 = STDVector(m_angle2);
		// not used
		if ((std1>0.5 & std2>0.5)&& m_line_width>0)
		{
			m_angle1.clear();
			m_angle2.clear();
			int roi_size = 10 * m_line_width;
			center_offset = m_center - Point2f(roi_size / 2, roi_size / 2);
			center_offset.x = center_offset.x < 0 ? 0 : center_offset.x;
			center_offset.y = center_offset.y < 0 ? 0 : center_offset.y;
			Rect ROI = Rect(center_offset, Size(roi_size, roi_size));
			Rect full_roi = Rect(0, 0, m_img.cols, m_img.rows);
			this->m_img = this->m_img(ROI & full_roi);
			find_centerLINES(m_img);
		}
        return m_center + center_offset;
	}
	else
		return Point2f(0, 0);
	}
	catch (const std::exception&e)
	{
		//QLog::GetInstance().qlog("CrossCenter:%s/n", e.what());
		imwrite("1.jpg", this->m_img);
	}
}

Point2f CrossCenter::get_crossCenterMATCH(Mat templa)
{
    m_center=find_centerTemplate(m_img, templa);
	//find_centerMTACH(templa);
	return m_center;
}

vector<Point> CrossCenter::get_roi_center()
{
    return roi_center;
}

vector<cv::Point> CrossCenter::get_newroi_center()
{
    return new_roi_center;
}

int CrossCenter::get_line_width()
{
	return m_line_width;
}

cv::Mat CrossCenter::getImgDraw()
{
    return m_img_draw;
}

cv::Point2f CrossCenter::getRotationAngle()
{
    if (m_angle1.size() > 0 & m_angle2.size() > 0)
    {

        return cv::Point2f(m_angle1[0] * 180.0 / CV_PI, m_angle2[0] * 180.0 / CV_PI);
    }
    else
    {
        return cv::Point2f(0, 0);
    }
}
double CrossCenter::getHorizontalLineAngle()
{
    return m_hAngle;
}
void CrossCenter::SetbinNum(int bin)
{
    m_binNum = bin;
}

void MLImageDetection::CrossCenter::setROILength(int len)
{
    CrossCenterParameter::roilength = len;
}
