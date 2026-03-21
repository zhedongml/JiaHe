#include "ml_coordinateTransformation.h"
using namespace std;
using namespace MLImageDetection;
MLImageDetection::MLCoordinateTransformation::MLCoordinateTransformation()
{
}

MLImageDetection::MLCoordinateTransformation::~MLCoordinateTransformation()
{
}

RigidTrans2D MLImageDetection::MLCoordinateTransformation::getRigidTrans2D(std::vector<cv::Point2f> srcPoints, std::vector<cv::Point2f> destPoints)
{
	RigidTrans2D rotateOffset;

	if (srcPoints.size() == 1 & destPoints.size() == 1)
	{
		rotateOffset.rotationAngle = 0;
		rotateOffset.offset.x = destPoints[0].x - srcPoints[0].x;
		rotateOffset.offset.y = destPoints[0].y - srcPoints[0].y;
	}
	else if (srcPoints.size() == 2 & destPoints.size() == 2)
	{
		double srcsubx = srcPoints[1].x - srcPoints[0].x;
		double srcsuby = srcPoints[1].y - srcPoints[0].y;
		double thetasrc = atan(srcsuby / (srcsubx + 1e-6));
		double dstsubx = destPoints[1].x - destPoints[0].x;
		double dstsuby = destPoints[1].y - destPoints[0].y;
		double thetadst = atan(dstsuby / (dstsubx + 1e-6));
		double theta = thetadst - thetasrc;
		cv::Point2f	srcRotated;
		srcRotated.x = srcPoints[0].x * cos(theta) - srcPoints[0].y * sin(theta);
		srcRotated.y = srcPoints[0].y * cos(theta) + srcPoints[0].x * sin(theta);
		double offsetx = destPoints[0].x - srcRotated.x;
		double offsety = destPoints[0].y - srcRotated.y;
		
		rotateOffset.rotationAngle = theta;
		rotateOffset.offset = cv::Point2f(offsetx, offsety);
	}
	return rotateOffset;
}

RigidTrans3D MLImageDetection::MLCoordinateTransformation::getRigidTrans3D(std::vector<cv::Point3f> srcPoints, std::vector<cv::Point3f> dstPoints)
{
	return RigidTrans3D();
}

vector<cv::Point2f> MLImageDetection::MLCoordinateTransformation::getPointsAfterTrans2D(vector<cv::Point2f> srcPts, RigidTrans2D trans)
{
	vector<cv::Point2f>transPts;
	cv::Point2f offset = trans.offset;
	double rotationAngle = trans.rotationAngle;
	for (int i = 0; i < srcPts.size(); i++)
	{
		cv::Point2f temp;
		temp.x = srcPts[i].x * cos(rotationAngle) - srcPts[i].y * sin(rotationAngle) + offset.x;
		temp.y = srcPts[i].y * cos(rotationAngle) + srcPts[i].x * sin(rotationAngle) + offset.y;
		transPts.push_back(temp);
	}
	return transPts;
}

cv::Point2f MLImageDetection::MLCoordinateTransformation::getPointAfterTrans2D(cv::Point2f srcPts, RigidTrans2D trans)
{
	cv::Point2f transPt;
	cv::Point2f offset = trans.offset;
	double rotationAngle = trans.rotationAngle;

	cv::Point2f temp;
	transPt.x = srcPts.x * cos(rotationAngle) + srcPts.y * sin(rotationAngle) + offset.x;
	transPt.y = srcPts.y * cos(rotationAngle) - srcPts.x * sin(rotationAngle) + offset.y;

	return transPt;
}


