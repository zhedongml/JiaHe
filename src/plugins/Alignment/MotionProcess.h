#pragma once

#include <QObject>
#include <opencv2/core.hpp>
#include <QDateTime>
#include "processConfig.h"
#include "dutConfig.h"
#include "slbConfig.h"
#include "waferConfig.h"

#include "WaitPause.h"

namespace AAProcess
{
	enum LogType
	{
		Normal,
		Warn,
		Error
	};

	enum ModuleName
	{
		DutModuleXYZ,
		DutModuleDxDyDz,
		ImagingModuleXYZ,
		ImagingModuleDxDy,
		Collimator,
		PLC,
		MV,
		keyence,
		ProjectionDxDy
	};

	struct OffsetRotate
	{
		double rotate;
		cv::Point2f offset;
		bool result = false;
	};

	class MotionProcess : public QObject
	{
		Q_OBJECT

	public:
		static MotionProcess& getInstance();
		~MotionProcess();

	public:
		//AA Device Connect
		std::string ConnectMVCamera();
		std::string ConnectMeasureCameraMotionModule();
		std::string ConnectDutMotionModule();
		std::string ConnectProjectorMotionModule();
		std::string ConnectKeyence();
		std::string ConnectCollimator();
		std::string ConnectPLC();
		std::string ConnectPolarizer();

		void setTreeSystemRun(bool isRun);
		//DUT Alignment
		std::string LoadDUT();
		std::string DutParallelAdjustment();
		std::string Ranging();
		std::string IsAutoIdentifyFiducial(bool isAuto);
		std::string FindFiducial();
		std::string EntrancePupilAlignment();
		std::string EyeboxScanning(int eyeBoxIndex);
		std::string GetDutTypeName(std::string cust_type, std::string& dut_name, int& dut_nums);
		std::string SetIsSaveFiducialImage(bool isSave, QString rootDir = "", QString dutSeq = "");

		//SLB Alignment
		std::string SLBAlignment();
		std::string LoadSLB();

		//Wafer
		void setWaferDutID(int dut_id);

		void StopTreeSystem(bool isStopTreeSystem);
		std::string JudgeHolderState(std::string& holder_type);

		// continue or stop
		void continueRun(bool isContinue, cv::Point2f pixel = cv::Point2f());
		void notifyPause(bool isPause);
		bool isStop();
		std::string SetExposureTime(bool is_auto_exposure,double exposure_time);

	private:
		explicit MotionProcess(QObject* parent = nullptr);

		//init config
		std::string InitConfig();
		//print log and pop up
		std::string PrintLog(LogType logType, std::string logMsg, bool isPopUpEnable = false);

		std::string FindFiducialToCalculate();

		//check hardware connection status, input ModuleName
		std::string CheckModuleConnectStatus(ModuleName moduleName);
		template<typename T, typename... Args>
		std::string CheckModuleConnectStatus(T first, Args... rest) {
			std::string res = CheckModuleConnectStatus(first);
			if (res != "") return res;

			return CheckModuleConnectStatus(rest...);  // recursion
		}

		//print position
		void PrintModulePosition(ModuleName moduleName);
		template<typename T, typename... Args>
		void PrintModulePosition(T first, Args... rest) {
			PrintModulePosition(first);
			PrintModulePosition(rest...);  // recursion
		}

		//check hardware is moving
		bool CheckModuleIsMoving(ModuleName moduleName);
		template<typename T, typename... Args>
		bool CheckModuleIsMoving(T first, Args... rest) {
			return CheckModuleIsMoving(first) || CheckModuleIsMoving(rest...);
		}

		//stop hardware move
		void StopModuleMove(ModuleName moduleName);
		template<typename T, typename... Args>
		void StopModuleMove(T first, Args... rest) {
			StopModuleMove(first);
			StopModuleMove(rest...);  // recursion
		}

		template<typename T>
		void setSavePosition(QString strTitle, T pos);

		//detect circle
		cv::Vec3f ObtainCenterByDetectedCircle(cv::Mat& fidImg);

		// Calculate the absolute coordinates of two fiducial motors at the center of the MV
		std::vector<cv::Point2f> CalculateFidAbsPosInMVCenter(std::vector<cv::Point2f> pixelCoor, std::vector<cv::Point3f> motorCoor, cv::Point2f mvCenter);

		// calculate dut rotate and offset
		OffsetRotate CalculateOffsetAndRotate(std::vector<cv::Point2f> actual, std::vector<cv::Point2f> design);

		//intput: The offset of a point relative to its origin, dut rotate and offset
		//output: (Absolute coordinates.x, Absolute coordinates.y, Rotate dut back to the required angle)
		//Need to rotate the angle first before moving the offset
		cv::Point3f RestoreOriginalPointToMVCenterCoordinate(cv::Point2f point, OffsetRotate offsetRotate);

		//Absolute coordinates from any point to the center of MV
		cv::Point2f DutAnyPointToMVCenterCoordinate(cv::Point2f offsetFromOrigin, OffsetRotate offsetRotate);

		std::string AutoIdentifyFiducial(cv::Point2f& center, cv::Point3f fiducialAcs);
		std::string ManualIdentifyFiducial(cv::Point2f& center, cv::Point3f fiducialAcs, bool isAuto = false);
		Result waitPause(bool isPause);
		cv::Point2f manualFiducialPos(cv::Mat& mat);
		QImage matToQImageCopy(const cv::Mat& mat);

	public:
	signals:
		void messageBoxSignal(QString message);
		void notifyStopSignal(bool isStop);
		void moveAlignEyeboxSignal(int index);
		void alignEyeboxEndSignal(bool success);
		QPointF onSignalgetFiducialPos(QImage& image);

	private:
		std::map<std::string, waferConfigInfo> m_waferConfigInfoMap;
		std::map<std::string, dutConfigInfo> m_dutConfigInfoMap;
		processConfigInfo m_processConfigInfo;
		slbConfigInfo m_slbConfigInfo;
		bool m_isTreeSystemRun;
		int wafer_dut_id;

		QString m_collimatorDeltaX = "NULL";
		QString m_collimatorDeltaY = "NULL";

		std::string currentDutName;
		std::string currentWaferName = "";
		OffsetRotate dutOffsetRotate;

		double currentRangingPos = -9999;

		WaitPause m_waitPause;
		cv::Point2f m_fiducialPixel;
		bool m_bIsAutoFiducial;
		QString m_strFiducialRootDir;
		bool m_bSaveFiducialImage;
		QString m_dutSeq;
		bool m_is_auto_exposure;
		std::atomic<bool> m_isStopTreeSystem{ false };
	};
}



