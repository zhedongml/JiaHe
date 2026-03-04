
#include "Core/loggingwrapper.h"
#include "OrientalMotor/OrientalMotorControl.h"
#include "LimitMove.h"
#include "processConfig.h"
#include "slbConfig.h"

namespace AAProcess
{
	LimitMove* LimitMove::self = nullptr;
	LimitMove* LimitMove::getInstance()
	{
		if (self == nullptr)
		{
			self = new LimitMove();
		}
		return self;
	}
	LimitMove::LimitMove(QObject* parent)
		: QObject(parent)
	{
	}

	bool LimitMove::perJudgeLimit3D(cv::Point3f targetPos, motion3DType type)
	{
		CoordinateLimit limit = ConfigItem::instance()->getMotion3DLimit(type);
		if (targetPos.x < limit.min.x || targetPos.x > limit.max.x ||
			targetPos.y < limit.min.y || targetPos.y > limit.max.y ||
			targetPos.z < limit.min.z || targetPos.z > limit.max.z)
		{
			return true;
		}
		return false;
	}

	bool LimitMove::perJudgeLimitOriental(cv::Point3f targetPos)
	{
		CoordinateLimit limit = ConfigItem::instance()->getOrientalLimit();
		if (targetPos.x < limit.min.x || targetPos.x > limit.max.x ||
			targetPos.y < limit.min.y || targetPos.y > limit.max.y ||
			targetPos.z < limit.min.z || targetPos.z > limit.max.z)
		{
			return true;
		}
		return false;
	}

	LimitMove::~LimitMove()
	{
		self = nullptr;
	}

	std::string LimitMove::motion3DMoveAbsAsync(cv::Point3f targetPos, motion3DType type)
	{
		if (!Motion3DModel::getInstance(type)->Motion3DisConnected()) {
			return "3D motion is not connected, 3D abs move failed.";
		}

		ML_Point3D currentPos = Motion3DModel::getInstance(type)->getPosition();

		//if (perJudgeLimit3D(targetPos, type)) {
		//	return "3D motion abs move exceed limit, abs move failed.";
		//}

		Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);
		if (!ret.success)
			return ret.errorMsg;


		//if (type == motion3DType::withCamera)
		//{
		//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);

		//	//if (currentPos.z > (targetPos.z * 1000))
		//	//{
		//	//	// 首先移动Z轴 

		//	//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Z", targetPos.z * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 然后移动X轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("X", targetPos.x * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 最后移动Y轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Y", targetPos.y * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//}
		//	//else
		//	//{
		//	//	// 首先移动X轴 

		//	//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("X", targetPos.x * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 然后移动Y轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Y", targetPos.y * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 最后移动Z轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Z", targetPos.z * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//}
		//}
		//else
		//{
		//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync(targetPos.x*1000, targetPos.y * 1000, targetPos.z * 1000);

		//	//if (currentPos.z < (targetPos.z * 1000))
		//	//{
		//	//	// 首先移动Z轴 

		//	//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Z", targetPos.z * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 然后移动X轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("X", targetPos.x * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 最后移动Y轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Y", targetPos.y * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//}
		//	//else
		//	//{
		//	//	// 首先移动X轴 

		//	//	Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("X", targetPos.x * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 然后移动Y轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Y", targetPos.y * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//	// 最后移动Z轴 

		//	//	ret = Motion3DModel::getInstance(type)->Motion3DMoveAbsAsync("Z", targetPos.z * 1000);
		//	//	if (!ret.success) {
		//	//		return ret.errorMsg;
		//	//	}
		//	//}
		//}

		return std::string("");
	}

	std::string LimitMove::motion3DMoveAbsSync(cv::Point3f targetPos, motion3DType type)
	{
		if (!Motion3DModel::getInstance(type)->Motion3DisConnected()) {
			return "3D motion is not connected, 3D abs move failed.";
		}

		ML_Point3D currentPos = Motion3DModel::getInstance(type)->getPosition();

		//if (perJudgeLimit3D(targetPos, type)) {
		//	return "3D motion abs move exceed limit, abs move failed.";
		//}

		if (type == motion3DType::withCamera)
		{
			if (currentPos.z > (targetPos.z * 1000))
			{
				// 首先移动Z轴 

				Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(currentPos.x, currentPos.y, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 然后移动X轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, currentPos.y, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 最后移动Y轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
			}
			else
			{
				// 首先移动X轴 

				Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, currentPos.y, currentPos.z);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 然后移动Y轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, currentPos.z);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 最后移动Z轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
			}
		}
		else
		{
			if (currentPos.z < (targetPos.z * 1000))
			{
				// 首先移动Z轴 

				Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(currentPos.x, currentPos.y, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 然后移动X轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, currentPos.y, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 最后移动Y轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
			}
			else
			{
				// 首先移动X轴 

				Result ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, currentPos.y, currentPos.z);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 然后移动Y轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, currentPos.z);
				if (!ret.success) {
					return ret.errorMsg;
				}
				// 最后移动Z轴 

				ret = Motion3DModel::getInstance(type)->Motion3DMoveAbs(targetPos.x * 1000, targetPos.y * 1000, targetPos.z * 1000);
				if (!ret.success) {
					return ret.errorMsg;
				}
			}
		}
		return std::string("");
	}

	std::string LimitMove::motion3DMoveRel(cv::Point3f offsetPos, motion3DType type)
	{
		if (!Motion3DModel::getInstance(type)->Motion3DisConnected()) {
			return "3D motion is not connected, 3D rel move failed.";
		}

		ML_Point3D currentPos = Motion3DModel::getInstance(type)->getPosition();
		cv::Point3f targetPos = cv::Point3f(currentPos.x / 1000.0 + offsetPos.x,
			currentPos.y / 1000.0 + offsetPos.y,
			currentPos.z / 1000.0 + offsetPos.z);

		return motion3DMoveAbsSync(targetPos, type);
	}

	std::string LimitMove::orientalMoveAbs(cv::Point3f targetPos)
	{
		if (!OrientalMotorControl::getInstance()->IsConnected()) {
			return "Oriental motor is not connected, oriental abs move failed.";
		}

		Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DX, targetPos.x);
		if (!ret.success) {
			return ret.errorMsg;
		}

		ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DY, targetPos.y);
		if (!ret.success) {
			return ret.errorMsg;
		}

		ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(OrientalAxle::DZ, targetPos.z);
		if (!ret.success) {
			return ret.errorMsg;
		}

		return std::string();
	}

	std::string LimitMove::orientalMoveAbs(int type, double targetPos)
	{
		if (!OrientalMotorControl::getInstance()->IsConnected()) {
			return "Oriental motor is not connected, oriental abs move failed.";
		}

		Result ret = OrientalMotorControl::getInstance()->MoveByDegreeAsync(type, targetPos);
		if (!ret.success) {
			return ret.errorMsg;
		}
		return std::string();
	}

	std::string LimitMove::orientalMoveRel(cv::Point3f offsetPos)
	{
		if (!OrientalMotorControl::getInstance()->IsConnected()) {
			return "Oriental motor is not connected, oriental rel move failed.";
		}
		cv::Point3d currentPos;
		currentPos.x = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DX);
		currentPos.y = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DY);
		currentPos.z = OrientalMotorControl::getInstance()->GetPosition(OrientalAxle::DZ);

		cv::Point3f targetPos = cv::Point3f(currentPos.x + offsetPos.x,
			currentPos.y + offsetPos.y,
			currentPos.z + offsetPos.z);
		return orientalMoveAbs(targetPos);
	}

	std::string LimitMove::orientalMoveRel(int type, double offsetPos)
	{
		if (!OrientalMotorControl::getInstance()->IsConnected()) {
			return "Oriental motor is not connected, oriental rel move failed.";
		}
		double currentPos = OrientalMotorControl::getInstance()->GetPosition(type);
		double targetPos = currentPos + offsetPos;
		return orientalMoveAbs(type, targetPos);
	}

	/*std::string LimitMove::moveAbsToSafePos(motion3DType type)
	{
		processConfigInfo info = processConfig::GetInstance().GetProcessConfigInfo();
		cv::Point3f safePos;
		if(type == motion3DType::withCamera){
			safePos = cv::Point3f(info.loadPos.imagingModule.x, info.loadPos.imagingModule.y, info.loadPos.imagingModule.z);
		}
		else if(type == motion3DType::withDUT){
			safePos = cv::Point3f(info.loadPos.dutModule.x, info.loadPos.dutModule.y, info.loadPos.dutModule.z);
		}
		else{
			return "motion type error";
		}
		return motion3DMoveAbsSync(safePos, type);
	}*/

	//std::string LimitMove::motion2DMoveAbs(cv::Point2f targetPos, motion2DType type)
	//{

	//	if(!Motion2DModel::getInstance(motion2DType::ACS2DCameraTilt)->GetIsConnected()){
	//		return "2D motion is not connected, 2D abs move failed.";
	//	}
	//	ML_Point currentPos = Motion2DModel::getInstance(motion2DType::ACS2DCameraTilt)->getPosition();
	//	if(perJudgeLimit2D(targetPos, type)){
	//		return "2D motion abs move exceed limit, abs move failed.";
	//	}
	//	Result ret = Motion2DModel::getInstance(motion2DType::ACS2DCameraTilt)->Motion2DMoveAbs(targetPos.x, targetPos.y);
	//	if(!ret.success){
	//		return ret.errorMsg;
	//	}

	//	return std::string("");
	//}

	//std::string LimitMove::motion2DMoveRel(cv::Point2f offsetPos, motion2DType type)
	//{
	//	if (!Motion2DModel::getInstance(motion2DType::ACS2DCameraTilt)->GetIsConnected())
	//	{
	//		return "2D motion is not connected, 2D rel move failed.";
	//	}
	//	ML_Point currentPos = Motion2DModel::getInstance(motion2DType::ACS2DCameraTilt)->getPosition();
	//	cv::Point2f targetPos = cv::Point2f(currentPos.x + offsetPos.x, currentPos.y + offsetPos.y);
	//	return motion2DMoveAbs(targetPos, type);
	//}

} // namespace AAProcess