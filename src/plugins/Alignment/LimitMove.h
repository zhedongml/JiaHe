#pragma once

#include <QObject>
//#include "MotionProcess.h"
#include "piMotionActor/configItem.h"

namespace AAProcess
{
	class LimitMove : public QObject
	{
		Q_OBJECT
	public:
		static LimitMove* getInstance();
		~LimitMove();

		std::string motion3DMoveAbsSync(cv::Point3f targetPos, motion3DType type);
		std::string motion3DMoveAbsAsync(cv::Point3f targetPos, motion3DType type);
		std::string motion3DMoveRel(cv::Point3f offsetPos, motion3DType type);
		std::string orientalMoveAbs(cv::Point3f targetPos);
		std::string orientalMoveAbs(int type, double targetPos);
		std::string orientalMoveRel(cv::Point3f offsetPos);
		std::string orientalMoveRel(int type, double offsetPos);
		//std::string moveAbsToSafePos(motion3DType type);

	private:
		explicit LimitMove(QObject* parent = nullptr);

		/*超出返回true*/
		bool perJudgeLimit3D(cv::Point3f targetPos, motion3DType type);
		bool perJudgeLimitOriental(cv::Point3f targetPos);

	private:
		static LimitMove* self;
	};
}
