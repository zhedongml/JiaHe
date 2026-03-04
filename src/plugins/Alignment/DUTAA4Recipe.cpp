#include "DUTAA4Recipe.h"
#include "metricsdata.h"
#include <Core\modemanager.h>
#include <Core\loggingwrapper.h>
#include "Alignment2D.h"
#include "pluginsystem\Services.h"
#include "MotionProcess.h"
#include "dutTypeConfig.h"
#include "MLUtilCommon.h"
#include "LimitMove.h"

//#include "OrientalMotor/OrientalMotorControl.h"
using namespace DutAA;
using namespace AAProcess;
DutAA4Recipe* DutAA4Recipe::self = nullptr;
DutAA4Recipe* DutAA4Recipe::getInstance(CalibrateWidget* calibrateWidget) {

    if (!self)
    {
        self = new DutAA4Recipe(calibrateWidget);
    }
    return self;
}

DutAA4Recipe::~DutAA4Recipe()
{
}

DutAA4Recipe::DutAA4Recipe(CalibrateWidget* calibrateWidget,QObject* parent)
    : m_calibrateWidget(calibrateWidget), QObject(parent)
{
    MotionProcess::getInstance().setTreeSystemRun(true);
    ObjectManager::getInstance()->registerObject("DutAA4Recipe", static_cast<void*>(this));
}

QString DutAA4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
    auto f_value = node.getInput<std::string>(name);
    if (!f_value)
    {
        throw BT::RuntimeError("Missing input [force]: ", f_value.error());
    }
    return QString::fromStdString(f_value.value());
}

void RegisterStopHandler(BT::TreeNode& node) {
	node.RegisterStopSignalCallback(
		[](bool isStopTreeSystem) {
			MotionProcess::getInstance().StopTreeSystem(isStopTreeSystem);
		}
	);
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_MVCamera()
{
    std::string msg = MotionProcess::getInstance().ConnectMVCamera();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_MVCamera ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_MeasureCameraMotionModule()
{
    std::string msg = MotionProcess::getInstance().ConnectMeasureCameraMotionModule();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_MeasureCameraMotionModule ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_DutMotionModule()
{
    std::string msg = MotionProcess::getInstance().ConnectDutMotionModule();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_DutMotionModule ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_ProjectorMotionModule()
{
    std::string msg = MotionProcess::getInstance().ConnectProjectorMotionModule();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_ProjectorMotionModule ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_Keyence()
{
    std::string msg = MotionProcess::getInstance().ConnectKeyence();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_Keyence ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_Collimator()
{
    std::string msg = MotionProcess::getInstance().ConnectCollimator();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_Collimator ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_PLC()
{
    std::string msg = MotionProcess::getInstance().ConnectPLC();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_PLC ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Connect_Polarizer()
{
    std::string msg = MotionProcess::getInstance().ConnectPolarizer();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Connect_Polarizer ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_LoadDUT(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().LoadDUT();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_LoadDUT ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_QrScanPos(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().DutQrScanPos();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_QrScan ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_AdjustLevel(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().DutParallelAdjustment();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_AdjustLevel ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_FindFiducial(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    QString isAuto = getNodeValueByName(node, "is_auto");
    bool isSave = getNodeValueByName(node, "is_save").toInt();
    bool is_auto_exposure = getNodeValueByName(node, "is_auto_exposure").toInt();
    double exposure_time = getNodeValueByName(node, "exposure_time").toDouble();
    QString rootDir = getNodeValueByName(node, "root_dir");
	QString dut_seq = getNodeValueByName(node, "dut_seq");

    std::string msg = MotionProcess::getInstance().SetExposureTime(is_auto_exposure, exposure_time);
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_FindFiducial ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    msg = MotionProcess::getInstance().SetIsSaveFiducialImage(isSave, rootDir, dut_seq);
    msg = MotionProcess::getInstance().IsAutoIdentifyFiducial(isAuto.toInt());
    msg = MotionProcess::getInstance().FindFiducial();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_FindFiducial ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_Ranging(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().Ranging();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_Ranging ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_Align_EntrancePupil(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().EntrancePupilAlignment();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_Align_EntrancePupil ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_Align_EyeBox(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    QString eyebox_id = getNodeValueByName(node, "eyebox_id");

    std::string msg = MotionProcess::getInstance().EyeboxScanning(eyebox_id.toInt());
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_Align_EyeBox ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Dut_GetDutType(BT::TreeNode& node)
{
    QString cust_type = getNodeValueByName(node, "cust_type");

    std::string dut_name;
    int wafer_dut_nums;
    std::string msg = MotionProcess::getInstance().GetDutTypeName(cust_type.toStdString(), dut_name, wafer_dut_nums);
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_Dut_GetDutType ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    
    MLUtils::TestState state;
    std::vector<std::string> parts = MLUtils::MLUtilCommon::instance()->split(dut_name, '_');
    if (parts.size() != 3)
    {
        QString message = QString("Recipe Node [ AA_Dut_GetDutType ] run error, dut name %1 is nonconforming.")
            .arg(QString::fromStdString(dut_name));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }

    LoggingWrapper::instance()->info(QString("Current Dut type: %1").arg(QString::fromStdString(dut_name)));

    state.IsDut = true;
    state.IsUpdateSLB = false;
    state.size = MLUtils::MLUtilCommon::instance()->TransStrToSize(parts[0]);
    state.eyeType = MLUtils::MLUtilCommon::instance()->TransStrToEyeType(parts[1]);
    state.cust_type = parts[2];
    MetricsData::instance()->SetTestState(state);

    node.setOutput("size_key", parts[0]);
    node.setOutput("eyetype_key", parts[1]);
    node.setOutput("wafer_dut_num_key", wafer_dut_nums);

    QString message = QString("=========================== Current Test State: IsDut=%1, IsUpdateSLB=%2, size=%3 ,eyeType=%4, custom_type=%5 ===========================")
        .arg("1")
        .arg("0")
        .arg(QString::fromStdString(parts[0]))
        .arg(QString::fromStdString(parts[1]))
        .arg(QString::fromStdString(parts[2]));
    LoggingWrapper::instance()->info(message);

    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_SetWaferDutId(BT::TreeNode& node)
{
    QString wafer_dut_id = getNodeValueByName(node, "wafer_dut_id");
    MotionProcess::getInstance().setWaferDutID(wafer_dut_id.toInt());
        
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_SLB_Alignment(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().SLBAlignment();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_SLB_Alignment ] run error, %1") .arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_SLB_LoadSLB(BT::TreeNode& node)
{
    RegisterStopHandler(node);
    MotionProcess::getInstance().StopTreeSystem(false);
    std::string msg = MotionProcess::getInstance().LoadSLB();
    if (msg != "")
    {
        QString message = QString("Recipe Node [ AA_SLB_LoadSLB ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_Move_PupilDecenter(BT::TreeNode& node)
{
    QString relative_x = getNodeValueByName(node, "relative_x");
    QString relative_y = getNodeValueByName(node, "relative_y");
    QString relative_z = getNodeValueByName(node, "relative_z");

    cv::Point3f offsetPos = cv::Point3f(relative_x.toDouble(),relative_y.toDouble(), relative_z.toDouble());
    std::string ret = LimitMove::getInstance()->motion3DMoveRel(offsetPos, motion3DType::withCamera);
    if (!ret.empty())
    {
        QString message = QString("Recipe Node [ AA_Move_PupilDecenter ] run error, %1").arg(QString::fromStdString(ret));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_SetExposureTime(BT::TreeNode& node)
{
    bool is_auto_exposure = getNodeValueByName(node, "is_auto_exposure").toInt();
    double exposure_time = getNodeValueByName(node, "exposure_time").toDouble();

    MotionProcess::getInstance().SetExposureTime(is_auto_exposure, exposure_time);
    return BT::NodeStatus::SUCCESS;
}

NodeStatus DutAA::DutAA4Recipe::AA_SetMotionMoveSpeed(BT::TreeNode& node)
{
    QString projector_dxdy = getNodeValueByName(node, "projector_dxdy");
    QString dut_x = getNodeValueByName(node, "dut_x");
    QString dut_y = getNodeValueByName(node, "dut_y");
    QString dut_z = getNodeValueByName(node, "dut_z");
    //int dut_dxdydz = getNodeValueByName(node, "dut_dxdydz").toInt();
    QString imaging_x = getNodeValueByName(node, "imaging_x");
    QString imaging_y = getNodeValueByName(node, "imaging_y");
    QString imaging_z = getNodeValueByName(node, "imaging_z");
    QString imaging_dxdy = getNodeValueByName(node, "imaging_dxdy");

    QString reticle_x = getNodeValueByName(node, "reticle_x");
    QString reticle_y = getNodeValueByName(node, "reticle_y");

    if (Motion3DModel::getInstance(withDUT)->Motion3DisConnected())
    {
        if(!dut_x.isEmpty())
            Motion3DModel::getInstance(withDUT)->SetSpeedX(dut_x.toDouble());
        int speed = Motion3DModel::getInstance(withDUT)->GetSpeed();
        if (!dut_y.isEmpty())
            Motion3DModel::getInstance(withDUT)->SetSpeedY(dut_y.toDouble());
        if (!dut_z.isEmpty())
            Motion3DModel::getInstance(withDUT)->SetSpeedZ(dut_z.toDouble());
    }
    if (Motion3DModel::getInstance(withCamera)->Motion3DisConnected())
    {
        if (!imaging_x.isEmpty())
            Motion3DModel::getInstance(withCamera)->SetSpeedX(imaging_x.toDouble());
        if (!imaging_y.isEmpty())
            Motion3DModel::getInstance(withCamera)->SetSpeedY(imaging_y.toDouble());
        if (!imaging_z.isEmpty())
            Motion3DModel::getInstance(withCamera)->SetSpeedZ(imaging_z.toDouble());
    }
    if (Motion2DModel::getInstance(ACS2DPro)->Motion2DisConnected())
    {
        if (!projector_dxdy.isEmpty())
            Motion2DModel::getInstance(ACS2DPro)->SetSpeed(projector_dxdy.toDouble());
    }
    if (Motion2DModel::getInstance(ACS2DCameraTilt)->Motion2DisConnected())
    {
        if (!imaging_dxdy.isEmpty())
            Motion2DModel::getInstance(ACS2DCameraTilt)->SetSpeed(imaging_dxdy.toDouble());
    }
    if (Motion2DModel::getInstance(ACS2DReticle)->Motion2DisConnected())
    {
        if (!reticle_y.isEmpty())
            Motion2DModel::getInstance(ACS2DReticle)->SetSpeedY(reticle_y.toDouble());
        if (!reticle_x.isEmpty())
            Motion2DModel::getInstance(ACS2DReticle)->SetSpeedX(reticle_x.toDouble());
        int speed = Motion2DModel::getInstance(ACS2DReticle)->GetSpeed();

    }
    //OrientalMotorControl::getInstance()->SetSpeed(dut_dxdydz);

    return BT::NodeStatus::SUCCESS;
}

// ------------------------------------ Testing ------------------------------------
NodeStatus DutAA::DutAA4Recipe::AA_MVCameraTest(BT::TreeNode& node)
{
    QString et = getNodeValueByName(node, "et");

    std::string message;
    Result ret;
    message = MotionProcess::getInstance().ConnectMVCamera();
    if (!message.empty())
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(message);
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    ret = CameraModel::GetInstance()->SetExposureTime(et.toDouble() * 1000);
    if (!ret.success)
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    ret = CameraModel::GetInstance()->StartGrabbing();
    if (!ret.success)
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    ret = CameraModel::GetInstance()->StopGrabbing();
    if (!ret.success)
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    ret = CameraModel::GetInstance()->GrabOne();
    if (!ret.success)
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    cv::Mat imgFid = CameraModel::GetInstance()->GetImage();
    if (imgFid.empty())
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    ret = CameraModel::GetInstance()->StartGrabbing();
    if (!ret.success)
    {
        QString message = QString("Recipe Node [ AA_MVCameraTest ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return NodeStatus::SUCCESS;
}