#ifndef MLALGORITHM_MLMACHINEVISION_H_
#define MLALGORITHM_MLMACHINEVISION_H_

#include "ml_distortion_correction.h"
#include <numeric>
#include<vector>
#include <map>
#include "ml_cross_center.h"
#include<iostream>
using namespace std;
namespace algorithm {

	struct MachinVisionPoints
	{
		cv::Point2f d1;
		cv::Point2f d2;
		cv::Point2f d3;
		cv::Point2f d4;
	};

	class ALGORITHM_API MLMachineVision: public MLDistortionCorrection
	{
	public:
		MLMachineVision();
		~MLMachineVision();
		MLMachineVision(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient, cv::Size img_size, double pixel_size, double magnification);
		std::vector<cv::Vec3f> GetMachineVisionPoints(cv::Mat img, int point_number);
		std::vector<cv::Vec3f> GetMachineVisionPoints_DPA(cv::Mat img, int point_number);// NEW 
		std::vector<cv::Vec3f> GetMachineVisionPoints_test(cv::Mat img,cv::Mat img1, int point_number);//test 

		std::vector<cv::Vec3f> GetDisplayFids_mock(cv::Mat img, int point_number);

		std::vector<cv::Point2f> GetGlobaPoints(std::vector<cv::Vec3f>& camera_points, cv::Point2f initial_point);
		std::vector<cv::Point2f> GetGlobaPoints(std::vector<cv::Point2f>& camera_points, cv::Point2f initial_point);
		cv::Vec3f FitCircle(std::vector<cv::Point2f>& points);
		cv::Point2f GetMiddle(std::vector<cv::Point2f>& points);

		std::vector<cv::Point2d> detect_display_fiducial(cv::Mat img, cv::Rect* roisPtr = nullptr, int roiNums = 2);
		std::string decodeMatrixRegion(cv::Mat img, int startx = 0, int starty = 0, int width = -1, int height = -1);
		std::vector<cv::Vec3f> getMdpaPoints(cv::Mat img, int startx, int starty, int width, int height, bool isRight = true, int radius = 140);
		// AA 
		std::vector<double> Displacement(cv::Mat DPA_img, cv::Rect roiDPA, cv::Mat WG_img, cv::Rect roiWG);
		map<string, std::vector<double>>ThreePointDistance(cv::Mat DPA_img, cv::Rect roiDPA, cv::Mat WG_img, cv::Rect roiWG, std::vector<std::vector<cv::Vec3f>> &Location);
		// eyebox scanning
		//map<string, std::vector<double>>FiducialPointstLocation(cv::Mat DPA_img, std::vector<cv::Rect> roiFiducial, int RoiFlag);
		cv::Point2f CircleFiducialDetect(cv::Mat fiducial_img);
		cv::Point2f LineFiducialDetect(cv::Mat fiducial_img);
		cv::Point2f crosscenterDectect(cv::Mat circleRoi);
		map<string, std::vector<double>>DPALocation(cv::Mat DPA_img, cv::Rect RoiDPA);

		//	IQT
		map<string, std::vector<double>>FiducialPointstLocation(cv::Mat DPA_img, std::vector<cv::Rect> roiFiducial, int RoiFlag, vector<cv::Vec3f>& fiducial_location);
		map<string, std::vector<double>>FiducialPointstLocation(vector<cv::Point2f>fiducial);
		vector<cv::Point3f >EyeboxFiducialOffset(map<string, vector<double> >f1, map<string, vector<double> >f2, cv::Point3f m1, cv::Point3f m2, cv::Point2f RalativeLocation  ,vector<cv::Point3f>eyeboxoffset);
		vector<cv::Point3f >EyeboxFiducialOffset(map<string, vector<double> >f1, map<string, vector<double> >f2, 
	    cv::Point3f m1, cv::Point3f m2, cv::Point2f f1Org,cv::Point2f f2Org, vector<cv::Point3f>eyeboxoffset);
	//	vector<cv::Point3f>EyeboxMotoLocation (vector<cv::Point2f>FiducialMot, vector<cv::Point2f>FidialOrg, vector<cv::Point3f>EyeBoxOffset);
		std::vector<cv::Point2f> FiducialMotorLocation(std::vector<cv::Point2f>FiducalGlobal,vector<cv::Point2f>MotorLocation);
	
		//IQT system function  rewrite			
		cv::Point3f FiducialMotoLocation(cv::Mat img,cv::Rect roiRect,int typeflag,cv::Point3f motoLocation);
		cv::Point3f FiducialMotoLicationMan(cv::Point2f fiducialPxl,cv::Point3f motoLocation);
		double CalculateRotateAngle(cv::Point2f& displacement);
		vector<cv::Point3f> Measure(vector<cv::Point3f>EyeboxOffst);
	
		cv::Point2f CalculatePxlLocation(cv::Mat img, cv::Rect roiRect, int typeflag);
	private:
		cv::Point2f CalculateGlobalLocation(cv::Point2f, cv::Point2f initial_point);
		cv::Point3f CalculateFiducialMotoLocaion(cv::Point2f gloalPoints, cv::Point3f m1);
			
	public:
	void setCenter(cv::Point2f center_);
	cv::Point2f getCenter();
	//IQT NEW
	vector<cv::Point2f> GetPxlLocation();
	vector<cv::Point3f>GetFiducialMotoLocaion();
	vector<cv::Point3f>GetFiducialOrgLocaion();
	void SetFiducialOrgLocation(cv::Point3f fiducialOrg);
	double GetRotateAngle();
	cv::Point2f GetDisplacement();
	void ClearLocation();

	private:
		//IQT para
		vector<cv::Point2f> m_pxllocation; // the pixel location of fiduial
		vector<cv::Point3f> m_FducialMotoLocation; // the fiducial location relative to the moto 
		vector<cv::Point3f> m_FiducialOrgLocation; // the fiducial locaton relative to the original point the custom defined
		double m_theta =0.0;
		cv::Point2f m_displacement;	
		double pixel_size_=1.85, magnification_=0.2;
		cv::Point2f m_center_=cv::Point2f(2012,1518);
	
	};

	class ALGORITHM_API MachineVisionImageRecognition
	{
	public:

		MachineVisionImageRecognition();
		~MachineVisionImageRecognition();

	public:
		cv::Point2f projectorInputgrationgOffset(cv::Mat img_input,cv::Mat img_projector, cv::Rect roiRect=cv::Rect(0,0,-1,-1));

		vector<cv::Point2f> circleDetect(cv::Mat img);
		vector<cv::Point2f> inputgratingDetec(cv::Mat img);
		vector<cv::Point2f> getGlobalpoint(vector<cv::Point2f>pxlCoordinate);
	private:
		double pixel_size_ = 1.85;
		double magnification_ = 0.2;
		cv::Point2f m_center_ = cv::Point2f(2012, 1518);
		

	};

}

#endif // !MLALGORITHM_MLMACHINEVISION_H_

