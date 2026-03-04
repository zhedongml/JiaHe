#pragma once

#include <QObject>
#include "alignment_global.h"
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/bt_factory.h"
#include "ObjectManager.h"
#include "calibratewidget.h"

using BT::NodeStatus;

namespace DutAA
{
	class ALIGNMENT_EXPORT DutAA4Recipe : public QObject
	{
		Q_OBJECT

	public:
		static  DutAA4Recipe* getInstance(CalibrateWidget* calibrateWidget);
		~DutAA4Recipe();
		DutAA4Recipe(CalibrateWidget* calibrateWidget,QObject* parent = nullptr);
		QString getNodeValueByName(BT::TreeNode& node, std::string name);

		// ¨T¨T¨T¨T¨T¨T¨T Devices Connect ¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus AA_Connect_MVCamera();
		NodeStatus AA_Connect_MeasureCameraMotionModule();
		NodeStatus AA_Connect_DutMotionModule();
		NodeStatus AA_Connect_ProjectorMotionModule();
		NodeStatus AA_Connect_Keyence();
		NodeStatus AA_Connect_Collimator();
		NodeStatus AA_Connect_PLC();
		NodeStatus AA_Connect_Polarizer();

		// ¨T¨T¨T¨T¨T¨T¨T DutAA Nodes(Alignment)¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus AA_Dut_LoadDUT(BT::TreeNode& node);
		NodeStatus AA_Dut_QrScanPos(BT::TreeNode& node);
		NodeStatus AA_Dut_AdjustLevel(BT::TreeNode& node);
		NodeStatus AA_Dut_FindFiducial(BT::TreeNode& node);
		NodeStatus AA_Dut_Ranging(BT::TreeNode& node);
		NodeStatus AA_Dut_Align_EntrancePupil(BT::TreeNode& node);
		NodeStatus AA_Dut_Align_EyeBox(BT::TreeNode& node);
		NodeStatus AA_Dut_GetDutType(BT::TreeNode& node);
		NodeStatus AA_SetWaferDutId(BT::TreeNode& node);

		// ¨T¨T¨T¨T¨T¨T¨T SlbAA Nodes(Alignment)¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus AA_SLB_Alignment(BT::TreeNode& node);
		NodeStatus AA_SLB_LoadSLB(BT::TreeNode& node);

		// ¨T¨T¨T¨T¨T¨T¨T Motion Move ¨T¨T¨T¨T¨T¨T¨T¨T //
		NodeStatus AA_Move_PupilDecenter(BT::TreeNode& node);

		NodeStatus AA_SetExposureTime(BT::TreeNode& node);
		NodeStatus AA_SetMotionMoveSpeed(BT::TreeNode& node);

		//// ¨T¨T¨T¨T¨T¨T¨T Init ¨T¨T¨T¨T¨T¨T¨T¨T //
		//NodeStatus AA_Dut_SetEyeRelief(BT::TreeNode& node);

		//NodeStatus DutAA_Connect();
		//NodeStatus DutAA_SetDutSN(BT::TreeNode& node);
		//NodeStatus DutAA_Calibrate(BT::TreeNode& node);
		//NodeStatus DutAA_SetMVExposure(BT::TreeNode& node);
		//NodeStatus DutAA_AlignEyebox(BT::TreeNode& node);
		//NodeStatus DutAA_SetProjectorAngle(BT::TreeNode& node);
		//NodeStatus DutAA_PupilDecenter(BT::TreeNode& node);
		//NodeStatus DutAA_CreateImageDir(BT::TreeNode& node);
		//NodeStatus DutAA_TurnOffLights();

		NodeStatus AA_MVCameraTest(BT::TreeNode& node);

	private:
		static DutAA4Recipe* self;
		CalibrateWidget* m_calibrateWidget;
	};

	inline void RegisterNodes(BT::BehaviorTreeFactory& factory)
	{
		DutAA4Recipe* obj = ((DutAA4Recipe*)ObjectManager::getInstance()->getObject("DutAA4Recipe"));
		if (!obj) {
			throw BT::RuntimeError("SystemRecipeNodes object not found !");
		}

		factory.registerSimpleAction(
			"AA_Connect_MVCamera",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_MVCamera();
			}, {});


		factory.registerSimpleAction(
			"AA_Connect_MeasureCameraMotionModule",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_MeasureCameraMotionModule();
			}, 
			{});

		factory.registerSimpleAction(
			"AA_Connect_DutMotionModule",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_DutMotionModule();
			},
			{});

		factory.registerSimpleAction(
			"AA_Connect_ProjectorMotionModule",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_ProjectorMotionModule();
			},
			{});

		factory.registerSimpleAction(
			"AA_Connect_Keyence",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_Keyence();
			},
			{});

		factory.registerSimpleAction(
			"AA_Connect_Collimator",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_Collimator();
			},
			{});

		factory.registerSimpleAction(
			"AA_Connect_PLC",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_PLC();
			},
			{});

		factory.registerSimpleAction(
			"AA_Connect_Polarizer",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Connect_Polarizer();
			},
			{});

		factory.registerSimpleAction(
			"AA_Dut_LoadDUT",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_LoadDUT(node);
			},
			{});
		
		factory.registerSimpleAction(
			"AA_Dut_QrScanPos",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_QrScanPos(node);
			},
			{});

		factory.registerSimpleAction(
			"AA_SetWaferDutId",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_SetWaferDutId(node);
			},
			{
				BT::InputPort<std::string>("wafer_dut_id","int"),
			});
		
		factory.registerSimpleAction(
			"AA_Dut_AdjustLevel",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_AdjustLevel(node);
			}, {});

		factory.registerSimpleAction(
			"AA_Dut_FindFiducial",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_FindFiducial(node);
			},
			{
				BT::InputPort<std::string>("is_auto","bool, e.g. 0,1"),
				BT::InputPort<std::string>("is_save","bool, e.g. 0,1"),
				BT::InputPort<std::string>("is_auto_exposure","bool, e.g. 0,1"),
				BT::InputPort<std::string>("exposure_time","double"),
				BT::InputPort<std::string>("root_dir","string, e.g. D:/FiducialImages"),
				BT::InputPort<std::string>("dut_seq", "string, e.g. dut_datatime")
			});

		factory.registerSimpleAction(
			"AA_Dut_Ranging",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj-> AA_Dut_Ranging(node);
			}, {});

		factory.registerSimpleAction(
			"AA_Dut_Align_EntrancePupil",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_Align_EntrancePupil(node);
			}, {});

		factory.registerSimpleAction(
			"AA_Dut_Align_EyeBox",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_Align_EyeBox(node);
			},
			{
				BT::InputPort<std::string>("eyebox_id","int")
			});

		factory.registerSimpleAction(
			"AA_Dut_GetDutType",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Dut_GetDutType(node);
			},
			{
				BT::OutputPort<std::string>("size_key","string"),
				BT::OutputPort<std::string>("eyetype_key","string"),
				BT::OutputPort<std::string>("wafer_dut_num_key","int"),
				
			    BT::InputPort<std::string>("cust_type","string")
			});

		factory.registerSimpleAction(
			"AA_SLB_Alignment",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_SLB_Alignment(node);
			},
			{});

		factory.registerSimpleAction(
			"AA_SLB_LoadSLB",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_SLB_LoadSLB(node);
			},
			{});

		factory.registerSimpleAction(
			"AA_Move_PupilDecenter",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_Move_PupilDecenter(node);
			},
			{
				BT::InputPort<std::string>("relative_x","double"),
				BT::InputPort<std::string>("relative_y","double"),
				BT::InputPort<std::string>("relative_z","double")
			});

		factory.registerSimpleAction(
			"AA_SetExposureTime",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_SetExposureTime(node);
			},
			{
				BT::InputPort<std::string>("is_auto_exposure","bool, e.g. 0,1"),
				BT::InputPort<std::string>("exposure_time","double")
			});

		factory.registerSimpleAction(
			"AA_SetMotionMoveSpeed",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_SetMotionMoveSpeed(node);
			},
			{
				BT::InputPort<std::string>("projector_dxdy","double"),
				BT::InputPort<std::string>("dut_x","double"),
				BT::InputPort<std::string>("dut_y","double"),
				BT::InputPort<std::string>("dut_z","double"),
				//BT::InputPort<std::string>("dut_dxdydz","double"),
				BT::InputPort<std::string>("imaging_x","double"),
				BT::InputPort<std::string>("imaging_y","double"),
				BT::InputPort<std::string>("imaging_z","double"),
				BT::InputPort<std::string>("imaging_dxdy","double"),
			    BT::InputPort<std::string>("reticle_x","double"),
				BT::InputPort<std::string>("reticle_y","double")
			});

		factory.registerSimpleAction(
			"AA_MVCameraTest",
			[=](BT::TreeNode& node)-> BT::NodeStatus
			{
				return obj->AA_MVCameraTest(node);
			},
			{
				BT::InputPort<std::string>("et","double")
			});
	}
}

