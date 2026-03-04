#pragma once
#include <QObject>
#include <json.hpp>
#include "measuremtf_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"


using BT::NodeStatus;
using Json = nlohmann::json;

namespace IQT
{
	class MEASUREMTF_EXPORT IQT4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  IQT4Recipe* getInstance();
		~IQT4Recipe();
		IQT4Recipe(QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);
		void loadConfig();
		void saveMetricsInfo(QString info);

		// =================== Colorimeter =================== //
		NodeStatus IQ_Connect();
		NodeStatus IQ_Connect_Async();
		NodeStatus IQ_Camera_StopGrabbing();
		NodeStatus IQ_Camera_StartGrabbing();
		NodeStatus IQ_Camera_SetBinning(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetBinningMode(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetBinningSelector(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetBitDepth(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetExposureByAuto(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetExposureByFix(BT::TreeNode& node);
		NodeStatus IQ_Camera_SetOverExposure(BT::TreeNode& node);
		//RX
		NodeStatus IQ_RX_SetRXSync(BT::TreeNode& node);

		NodeStatus IQ_RX_SetRXASync(BT::TreeNode& node);

		NodeStatus IQ_RX_SetCylinder(BT::TreeNode& node);


		//filter
		NodeStatus IQ_SwitchColorFilter(BT::TreeNode& node);
		NodeStatus IQ_SwitchNDFilter(BT::TreeNode& node);

		//capture
		NodeStatus IQ_UpdateFlipRotation();
		NodeStatus IQ_LoadCalibrationData(BT::TreeNode& node);
		NodeStatus IQ_LoadDarkCalibrationData(BT::TreeNode& node);
		NodeStatus IQ_LoadFFCCalibrationData(BT::TreeNode& node);
		NodeStatus IQ_Capture_XYZImage_Sync(BT::TreeNode& node);
		NodeStatus IQ_Capture_GrayImage_Sync(BT::TreeNode& node);
		NodeStatus IQ_Capture_LuminanceImage_Sync(BT::TreeNode& node);
		NodeStatus IQ_Capture_XYZImage_Async(BT::TreeNode& node);
		NodeStatus IQ_Capture_GrayImage_Async(BT::TreeNode& node);
		NodeStatus IQ_Capture_LuminanceImage_Async(BT::TreeNode& node);
		NodeStatus IQ_WaitForCaptureEnd(BT::TreeNode& node);

		//motion
		NodeStatus IQ_SetVID(BT::TreeNode& node);
		NodeStatus IQ_SetSphere(BT::TreeNode& node);
		NodeStatus IQ_SetFocusMotion(BT::TreeNode& node);
		NodeStatus IQ_SetLightFocusMotion(BT::TreeNode& node);
		NodeStatus IQ_ThroughFocus_Diopter(BT::TreeNode& node);
		NodeStatus IQ_ThroughFocus_MotionPos(BT::TreeNode& node);

		// =================== IQT Nodes =================== //

		//Metrics
		NodeStatus IQ_Metrics_CreateDBTable();
		NodeStatus IQ_Metrics_LoadLimitConfig(BT::TreeNode& node);
		NodeStatus IQ_Metrics_CreateResultFile(BT::TreeNode& node);
		NodeStatus IQ_Metrics_Calculate(BT::TreeNode& node);
		NodeStatus IQ_Metrics_Location_Clear(BT::TreeNode& node);
		NodeStatus IQ_StartParallelCalculate(BT::TreeNode& node);
		NodeStatus IQ_StopParallelCalculate(BT::TreeNode& node);
		NodeStatus IQ_WaitParallelCalculateEnd(BT::TreeNode& node);
		NodeStatus IQ_Metrics_ExportToCSV();
		NodeStatus IQ_CameraSimulator_Create(BT::TreeNode& node);
		NodeStatus IQ_CameraSimulator_StartWork(BT::TreeNode& node);

		NodeStatus IQ_SetEyeboxList(BT::TreeNode& node);
		NodeStatus IQ_SetDutID(BT::TreeNode& node);
		NodeStatus IQ_SetTestState(BT::TreeNode& node);//offline&slb state

		NodeStatus IQ_LoadFixExposureTime(BT::TreeNode& node);
		NodeStatus IQ_DeleteFixExposureTime();
		NodeStatus IQ_UpdateFixExposureTimeConfig();

		NodeStatus IQ_Polarizer_AutoScan(BT::TreeNode& node);
		NodeStatus IQ_Metrics_Luminance_Dialog(BT::TreeNode& node);

	private:
		static IQT4Recipe* self;
		std::string m_configPath = "./config/Algorithmconfig.json";
		Json settingJsonObj;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		IQT4Recipe* obj = ((IQT4Recipe*)ObjectManager::getInstance()->getObject("IQT4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("IQT4Recipe object not found !");
		}
		factory.registerSimpleAction(
			"IQ_Metrics_Location_Clear",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_Location_Clear(node);
			}, {});

		factory.registerSimpleAction(
			"Colorimeter_Connect_All",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Connect();
			}, {});

		factory.registerSimpleAction(
			"Colorimeter_Connect_All_Async",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Connect_Async();
			}, {});

		factory.registerSimpleAction(
			"Colorimeter_Camera_StopGrabbing",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_StopGrabbing();
			}, {});

		factory.registerSimpleAction(
			"Colorimeter_Camera_StartGrabbing",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_StartGrabbing();
			}, {});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetBinning",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetBinning(node);
			},
			{
				BT::InputPort<std::string>("binning","int, e.g. 1/2/4"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetBinningMode",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetBinningMode(node);
			},
			{
				BT::InputPort<std::string>("binning_mode","[Average,Sum]","string, e.g.  Average/Sum")
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetBinningSelector",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetBinningSelector(node);
			},
			{
				BT::InputPort<std::string>("binning_selector","[Sensor,Logic]","string, e.g. 0:Sensor/1:Logic")
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetBitDepth",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetBitDepth(node);
			},
			{
				BT::InputPort<std::string>("bit_depth","[8,10,12]","int, e.g. 8/10/12")
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetExposureByAuto",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetExposureByAuto(node);
			},
			{
				BT::InputPort<std::string>("init_time","float"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetExposureByFix",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetExposureByFix(node);
			},
			{
				BT::InputPort<std::string>("exposure_time","float"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Camera_SetOverExposure",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Camera_SetOverExposure(node);
			},
			{
				BT::InputPort<std::string>("over_exposure_factor","float"),
			});

		factory.registerSimpleAction(
			"Colorimeter_RX_SetRXSync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_RX_SetRXSync(node);
			},
			{
				BT::InputPort<std::string>("RX_Sphere","float"),
				BT::InputPort<std::string>("RX_Cylinder","float"),
				BT::InputPort<std::string>("RX_Axis","int"),
			});

		factory.registerSimpleAction(
			"Colorimeter_RX_SetRXASync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_RX_SetRXASync(node);
			},
			{
				BT::InputPort<std::string>("RX_Sphere","float"),
				BT::InputPort<std::string>("RX_Cylinder","float"),
				BT::InputPort<std::string>("RX_Axis","int"),
			});

		factory.registerSimpleAction(
			"Colorimeter_RX_SetCylinder",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_RX_SetCylinder(node);
			},
			{
				BT::InputPort<std::string>("RX_Cylinder","float"),
				BT::InputPort<std::string>("RX_Axis","int"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Filter_ColorSwitch",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SwitchColorFilter(node);
			},
			{
				BT::InputPort<std::string>("color_filter","string, e.g.  X/Y/Z/Block/Clear"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Filter_NDSwitch",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SwitchNDFilter(node);
			},
			{
				BT::InputPort<std::string>("nd_filter","string, e.g.  ND0/ND1/ND2/ND3/Block/Clear"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_SetVID",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetVID(node);
			},
			{
				BT::InputPort<std::string>("vid","float, unit:mm"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_SetSphere",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetSphere(node);
			},
			{
				BT::InputPort<std::string>("sphere","float, unit:diopter"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_SetFocus",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetFocusMotion(node);
			},
			{
				BT::InputPort<std::string>("focus","float, unit:mm"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_SetLightFocus",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetLightFocusMotion(node);
			},
			{
				BT::InputPort<std::string>("color","string, e.g. R/G/B/W"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_AutoFocus_Position",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_ThroughFocus_MotionPos(node);
			},
			{
				BT::InputPort<std::string>("file_name","string"),
				BT::InputPort<std::string>("direction", "string, e.g. H/V/AVG"),
				BT::InputPort<std::string>("focus_min", "double"),
				BT::InputPort<std::string>("focus_max", "double"),
				BT::InputPort<std::string>("rough_step", "double"),
				BT::InputPort<std::string>("is_use_fine", "0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("fine_range", "double"),
				BT::InputPort<std::string>("fine_step", "double"),
				BT::InputPort<std::string>("is_chess_mode", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_lpmm_unit", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("freq", "double"),
			});

		factory.registerSimpleAction(
			"Colorimeter_Motion_AutoFocus_Diopter",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_ThroughFocus_Diopter(node);
			},
			{
				BT::InputPort<std::string>("file_name","string"),
				BT::InputPort<std::string>("direction", "string, e.g. H/V/AVG"),
				BT::InputPort<std::string>("diopter", "double"),
				BT::InputPort<std::string>("scan_range", "double"),
				BT::InputPort<std::string>("rough_step_num", "int"),
				BT::InputPort<std::string>("is_use_fine", "0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("fine_step_num", "int"),
				BT::InputPort<std::string>("is_chess_mode", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_lpmm_unit", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("freq", "double"),
				BT::InputPort<std::string>("is_save_images","0","bool, e.g. 0/1")
			});

		factory.registerSimpleAction(
			"Colorimeter_LoadCalibrationData",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_LoadCalibrationData(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("binning","1","int, e.g.  1/2/4/8"),
				BT::InputPort<std::string>("colorfilter_list","X,Y,Z","string, e.g.  X,Y,Z"),
				BT::InputPort<std::string>("ndfliter_list","ND0","string, e.g.  ND0,ND1"),
				BT::InputPort<std::string>("lightsource_list","R,G,B","string, e.g.  R,G,B"),
				BT::InputPort<std::string>("is_load_dark","1","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_colorshift","1","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_ffc","1","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_distortion","1","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_exposure", "1","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_fliprotation","1", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_luminance","1", "bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_load_fourcolor", "0","bool, e.g. 0/1"),
			});

		factory.registerSimpleAction(
			"Colorimeter_LoadDarkCalibrationData",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_LoadDarkCalibrationData(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("binning","1","int, e.g.  1/2/4/8"),
				BT::InputPort<std::string>("ndfliter_list","ND0","string, e.g.  ND0,ND1"),
				BT::InputPort<std::string>("colorfilter_list","X,Y,Z","string, e.g.  X,Y,Z"),
				BT::InputPort<std::string>("lightsource_list","R,G,B","string, e.g.  R,G,B"),

			});

		factory.registerSimpleAction(
			"Colorimeter_LoadFFCCalibrationData",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_LoadFFCCalibrationData(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("binning","1","int, e.g.  1/2/4/8"),
				BT::InputPort<std::string>("ndfliter_list","ND0","string, e.g.  ND0,ND1"),
				BT::InputPort<std::string>("colorfilter_list","X,Y,Z","string, e.g.  X,Y,Z"),
				BT::InputPort<std::string>("lightsource_list","R,G,B","string, e.g.  R,G,B"),

			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_XYZImage_Sync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_XYZImage_Sync(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning","int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("x_exposure_time","int"),
				BT::InputPort<std::string>("y_exposure_time","int"),
				BT::InputPort<std::string>("z_exposure_time","int"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
			    BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")
			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_XYZImage_Async",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_XYZImage_Async(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning","int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("x_exposure_time","int"),
				BT::InputPort<std::string>("y_exposure_time","int"),
				BT::InputPort<std::string>("z_exposure_time","int"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")
			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_GrayImage_Sync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_GrayImage_Sync(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning", "int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("color_filter", "string, e.g.  X/Y/Z/Block/Clear"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("exposure_time","double"),
				BT::InputPort<std::string>("et_factor","double"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_hdr","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("max_exposure_time","double"),
				BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")
			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_LuminanceImage_Sync",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_LuminanceImage_Sync(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning", "int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("color_filter", "string, e.g.  X/Y/Z/Block/Clear"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("exposure_time","double"),
				BT::InputPort<std::string>("et_factor","double"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_hdr","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("max_exposure_time","double"),
				BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")
			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_GrayImage_Async",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_GrayImage_Async(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning", "int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("color_filter", "string, e.g.  X/Y/Z/Block/Clear"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("exposure_time","double"),
				BT::InputPort<std::string>("et_factor", "double"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_hdr","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("max_exposure_time","double"),
                BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")
			});

		factory.registerSimpleAction(
			"Colorimeter_Capture_LuminanceImage_Async",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Capture_LuminanceImage_Async(node);
			},
			{
				BT::InputPort<std::string>("aperture","3","double"),
				BT::InputPort<std::string>("dutid","string"),
				BT::InputPort<std::string>("eyeboxid", "int"),
				BT::InputPort<std::string>("binning", "int"),
				BT::InputPort<std::string>("nd_filter", "string"),
				BT::InputPort<std::string>("color_filter", "string, e.g.  X/Y/Z/Block/Clear"),
				BT::InputPort<std::string>("light_source", "string, e.g. R/G/B/W"),
				BT::InputPort<std::string>("image_type", "string"),
				BT::InputPort<std::string>("is_auto_et","bool, e.g. 0/1"),
				BT::InputPort<std::string>("exposure_time","double"),
				BT::InputPort<std::string>("et_factor","double"),
				BT::InputPort<std::string>("is_convert_16bit","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("convert_method","0","int, e.g. 0/1"),
				BT::InputPort<std::string>("save_raw_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("save_calibration_image","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("is_hdr","0","bool, e.g. 0/1"),
				BT::InputPort<std::string>("max_exposure_time","double"),
				BT::InputPort<std::string>("nameRule","#LightSource#_#Pattern#_#EyeboxID#_#ColorFilter#_#NDFilter#","string")

			});

		factory.registerSimpleAction(
			"Colorimeter_WaitForCaptureEnd",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_WaitForCaptureEnd(node);
			}, {
				BT::InputPort<std::string>("timeout","int, unit:s"),
			});

		factory.registerSimpleAction(
			"IQ_Metrics_LoadLimitConfig",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_LoadLimitConfig(node);
			},
			{
				BT::InputPort<std::string>("file_name","string"),
			});

		factory.registerSimpleAction(
			"IQ_Metrics_CreateDBTable",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_CreateDBTable();
			},
			{});

		factory.registerSimpleAction(
			"IQ_Metrics_CreateResultFile",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_CreateResultFile(node);
			},
			{
				BT::InputPort<std::string>("root_dir","string"),
				BT::InputPort<std::string>("dut_seq","string"),
				BT::InputPort<std::string>("csvname","string"),
				BT::InputPort<std::string>("all_csvname","string")
			});

		factory.registerSimpleAction(
			"IQ_Metrics_Calculate",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_Calculate(node);
			},
			{
				BT::InputPort<std::string>("metrics_name","string, e.g. FOVOffset/FOV/Rotation/GridDistortion/LuminanceRolloff/CheckerContrast/GridDenseMTF/LuminanceEfficiency/PupilSwim/LateralColor/Flare/ColorUniformity")
			});

		factory.registerSimpleAction(
			"IQ_Metrics_ExportToCSV",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_ExportToCSV();
			},
			{});

		factory.registerSimpleAction(
			"IQ_SetTestState",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetTestState(node);
			},
			{
				BT::InputPort<std::string>("is_dut","[0,1]","bool"),
				BT::InputPort<std::string>("is_update_slb","[0,1]","bool"),
				BT::InputPort<std::string>("size","[Large,Small]","string, e.g. 0:Large,1:Small"),
				BT::OutputPort<std::string>("size_key","string")
			});

		factory.registerSimpleAction(
			"IQ_StartParallelCalculate",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_StartParallelCalculate(node);
			},
			{
				BT::InputPort<std::string>("metrics","","string list"),
				BT::InputPort<std::string>("eyeboxlist","","string list"),
			});

		factory.registerSimpleAction(
			"IQ_StopParallelCalculate",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_StopParallelCalculate(node);
			}, {});

		factory.registerSimpleAction(
			"IQ_WaitParallelCalculateEnd",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_WaitParallelCalculateEnd(node);
			},
			{
				BT::InputPort<std::string>("timeout","int"),
			});

		factory.registerSimpleAction(
			"IQ_SetEyeboxList",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetEyeboxList(node);
			},
			{
				BT::InputPort<std::string>("eyeboxlist","5","string list"),
				BT::OutputPort<std::string>("key", "string"),
			});

		factory.registerSimpleAction(
			"IQ_SetDutID",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_SetDutID(node);
			},
			{
				BT::InputPort<std::string>("id_value","string"),
				BT::OutputPort<std::string>("id_key","string")
			});

		factory.registerSimpleAction(
			"IQ_UpdateFlipRotation",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_UpdateFlipRotation();
			},
			{});

		factory.registerSimpleAction(
			"IQ_LoadFixExposureTime",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_LoadFixExposureTime(node);
			},
			{
				BT::InputPort<std::string>("is_match_eb","int")
			});

		//factory.registerSimpleAction(
		//	"IQ_DeleteFixExposureTime",
		//	[=](BT::TreeNode& node)-> BT::NodeStatus
		//	{
		//		return obj->IQ_DeleteFixExposureTime();
		//	},
		//	{});

		factory.registerSimpleAction(
			"IQ_UpdateFixExposureTimeConfig",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_UpdateFixExposureTimeConfig();
			},
			{});

		//factory.registerSimpleAction(
		//	"IQ_SetFixExposureTimeMatchRule",
		//	[=](BT::TreeNode& node)-> BT::NodeStatus
		//	{
		//		return obj->IQ_SetFixExposureTimeMatchRule(node);
		//	},
		//	{
		//		BT::InputPort<std::string>("is_match_eb","int"),
		//	});

		factory.registerSimpleAction(
			"IQ_CameraSimulator_Create",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_CameraSimulator_Create(node);
			},
			{
				BT::InputPort<std::string>("img_folder","string"),
				BT::InputPort<std::string>("timer_interval","int"),
			});

		factory.registerSimpleAction(
			"IQ_CameraSimulator_StartWork",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_CameraSimulator_StartWork(node);
			}, {});

		factory.registerSimpleAction(
			"IQ_Polarizer_AutoScan",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Polarizer_AutoScan(node);
			},
			{
				BT::InputPort<std::string>("max_angle","double"),
				BT::InputPort<std::string>("min_angle","double"),
				BT::InputPort<std::string>("step","double"),
				BT::InputPort<std::string>("roi_x","int"),
				BT::InputPort<std::string>("roi_y","int"),
				BT::InputPort<std::string>("roi_width","int"),
				BT::InputPort<std::string>("roi_height","int"),
				BT::InputPort<std::string>("save_dir","string")
			});

		factory.registerSimpleAction(
			"IQ_Metrics_Luminance_Dialog",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->IQ_Metrics_Luminance_Dialog(node);
			},
			{
				BT::InputPort<std::string>("color","string"),
				BT::InputPort<std::string>("eyebox","int")
			});
	}
}


