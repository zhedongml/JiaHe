#include <PrjCommon\Session.h>
#include "IQT4Recipe.h"
#include "loggingwrapper.h"
#include "GenCSVHeader.h"
#include "iqtmodel.h"
#include "EtManage.h"

#include "MLColorimeterMode.h"
#include "MLColorimeterHelp.h"
#include "MLColorimeterCommon.h"
#include "Focus/AutoFocusModel.h"
#include "Parallel/IQ_BasicDataType.h"
#include "Parallel/MetricsProcessorProxy.h"
#include "Parallel/CameraSimulator.h"
#include "iqmetricconfig.h"
#include "ImageRotationConfig.h"
#include "ML_NineCrossDetection.h"
#include "MLSolidDetection.h"
#include "PolarizerControl/ThorlabsMode.h"
#include "MetricsDataBase.h"
#include <QMessageBox>

using namespace IQT;
using namespace IQ_Parallel_NS;
IQT4Recipe* IQT4Recipe::self = nullptr;
IQT4Recipe* IQT4Recipe::getInstance() {

	if (!self)
	{
		self = new IQT4Recipe();
	}
	return self;
}

IQT4Recipe::IQT4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("IQT4Recipe", static_cast<void*>(this));
}

IQT4Recipe::~IQT4Recipe()
{
}

QString IQT4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
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
			AutoFocusModel::instance()->StopTreeSystemCallback(isStopTreeSystem);
		}
	);
}

void IQT4Recipe::loadConfig()
{
	std::ifstream jsonFile(m_configPath);
	std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
	jsonFile.close();
	settingJsonObj = Json::parse(contents);
}

void IQT4Recipe::saveMetricsInfo(QString info)
{
	QString imgPaths = MetricsData::instance()->getMTFImgsDir();
	QFile infoFile(imgPaths + "info.txt");
	if (!infoFile.open(QIODevice::Append))
	{
		LoggingWrapper::instance()->error("Save info.txt failed, open file error.");
		return;
	}
	infoFile.write(info.toUtf8());
	infoFile.write("\n");
	infoFile.close();
}

NodeStatus IQT4Recipe::IQ_Connect()
{
	Result res = MLColorimeterMode::Instance()->Connect();
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Connect_All ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Connect_Async() {
	Result res = MLColorimeterMode::Instance()->ConnectAsync();
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Connect_All_Async ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_StopGrabbing()
{

	Result res = MLColorimeterMode::Instance()->StopGrabbing(true);
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Camera_StopGrabbing ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_StartGrabbing()
{
	Result res = MLColorimeterMode::Instance()->StopGrabbing(false);
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Camera_StartGrabbing ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetBinning(BT::TreeNode& node)
{
	QString binning = getNodeValueByName(node, "binning");

	Result res;
	res = MLColorimeterMode::Instance()->SetBinning(binning.toInt());
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetBinning ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetBinningMode(BT::TreeNode& node)
{
	QString binning_mode = getNodeValueByName(node, "binning_mode");

	Result res;
	res = MLColorimeterMode::Instance()->SetBinningMode(binning_mode);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetBinningMode ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetBinningSelector(BT::TreeNode& node)
{
	QString binning_selector = getNodeValueByName(node, "binning_selector");
	try
	{
		Result res = MLColorimeterMode::Instance()->SetBinningSelector(BinningSelector::Sensor);
		if (!res.success)throw std::runtime_error(res.errorMsg);
		if (binning_selector.toInt() > 0)
		{
			res = MLColorimeterMode::Instance()->SetBinning(ML::CameraV2::Binning::ONE_BY_ONE);
			if (!res.success) throw std::runtime_error(res.errorMsg);
			res = MLColorimeterMode::Instance()->SetBinningSelector(BinningSelector::Logic);
			if (!res.success) throw std::runtime_error(res.errorMsg);
		}
		return BT::NodeStatus::SUCCESS;
	}
	catch (const std::exception& ex)
	{
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetBinningSelector ] run error, %1")
			.arg(QString::fromStdString(ex.what()));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
}

NodeStatus IQT4Recipe::IQ_Camera_SetBitDepth(BT::TreeNode& node)
{
	QString bit_depth = getNodeValueByName(node, "bit_depth");

	Result res;
	res = MLColorimeterMode::Instance()->SetBitDepth(bit_depth.toInt());
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetBitDepth ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetExposureByAuto(BT::TreeNode& node)
{
	QString init_time = getNodeValueByName(node, "init_time");

	ExposureSetting exposure = { ExposureMode::Auto, init_time.toFloat() };
	Result res = MLColorimeterMode::Instance()->SetExposureTime(exposure);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetExposureByAuto ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetExposureByFix(BT::TreeNode& node)
{
	QString exposure_time = getNodeValueByName(node, "exposure_time");

	ExposureSetting exposure = { ExposureMode::Fixed, exposure_time.toFloat() };
	Result res = MLColorimeterMode::Instance()->SetExposureTime(exposure);
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetExposureByFix ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Camera_SetOverExposure(BT::TreeNode& node)
{
	QString over_exposure_factor = getNodeValueByName(node, "over_exposure_factor");

	double expo = MLColorimeterMode::Instance()->GetExposureTime();
	ExposureSetting exposure = { ExposureMode::Fixed, expo * over_exposure_factor.toFloat() };
	Result res = MLColorimeterMode::Instance()->SetExposureTime(exposure);
	if (!res.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Camera_SetOverExposure ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_RX_SetRXSync(BT::TreeNode& node)
{
	QString rx_sphere = getNodeValueByName(node, "RX_Sphere");
	QString rx_cylinder = getNodeValueByName(node, "RX_Cylinder");
	QString rx_axis = getNodeValueByName(node, "RX_Axis");

	RXCombination RXParams; 
	RXParams.Sphere = rx_sphere.toDouble();
	RXParams.Cylinder = rx_cylinder.toDouble();
	RXParams.Axis = rx_axis.toInt();
	Result res = MLColorimeterMode::Instance()->SetRXSync(RXParams);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_RX_SetRXSync ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_RX_SetRXASync(BT::TreeNode& node)
{
	QString rx_sphere = getNodeValueByName(node, "RX_Sphere");
	QString rx_cylinder = getNodeValueByName(node, "RX_Cylinder");
	QString rx_axis = getNodeValueByName(node, "RX_Axis");

	RXCombination RXParams;
	RXParams.Sphere = rx_sphere.toDouble();
	RXParams.Cylinder = rx_cylinder.toDouble();
	RXParams.Axis = rx_axis.toInt();
	Result res = MLColorimeterMode::Instance()->SetRXSync(RXParams);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_RX_SetRXASync ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_RX_SetCylinder(BT::TreeNode& node)
{
	QString rx_cylinder = getNodeValueByName(node, "RX_Cylinder");
	QString rx_axis = getNodeValueByName(node, "RX_Axis");

	Result res = MLColorimeterMode::Instance()->SetCylinder(rx_cylinder,rx_axis.toInt());
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_RX_SetCylinder ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SwitchColorFilter(BT::TreeNode& node)
{
	QString color_filter = getNodeValueByName(node, "color_filter");

	Result res = MLColorimeterMode::Instance()->SetColorFilter(color_filter);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Filter_ColorSwitch ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SwitchNDFilter(BT::TreeNode& node)
{
	QString nd_filter = getNodeValueByName(node, "nd_filter");

	Result res = MLColorimeterMode::Instance()->SetNDFilter(nd_filter);
	if (!res.success) {
		QString message = QString("Recipe Node [ Colorimeter_Filter_NDSwitch ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetVID(BT::TreeNode& node)
{
	QString vid = getNodeValueByName(node, "vid");
	Result ret = MLColorimeterMode::Instance()->SetVidSync(vid.toDouble());
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Motion_SetVID ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetSphere(BT::TreeNode& node)
{
	QString sphere = getNodeValueByName(node, "sphere");
	Result ret = MLColorimeterMode::Instance()->SetSphereSync(sphere.toDouble());
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Motion_SetSphere ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetFocusMotion(BT::TreeNode& node)
{
	QString focus = getNodeValueByName(node, "focus");
	Result ret = MLColorimeterMode::Instance()->SetFocusMotionPosSync(focus.toDouble());
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Motion_SetFocus ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetLightFocusMotion(BT::TreeNode& node)
{
	QString color = getNodeValueByName(node, "color").toUpper();
	Result ret = AutoFocusModel::instance()->SetFocusMotionByLight(color);
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Motion_SetLightFocus ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_LoadCalibrationData(BT::TreeNode& node)
{
	// CalibrationConfig2
	QString aperture = getNodeValueByName(node, "aperture");
	QString binning = getNodeValueByName(node, "binning");
	QString colorfilter_list = getNodeValueByName(node, "colorfilter_list");
	QString lightsource_list = getNodeValueByName(node, "lightsource_list");
	QString ndfliter_list = getNodeValueByName(node, "ndfliter_list");
	// CalibrationFlag
	QString is_load_dark = getNodeValueByName(node, "is_load_dark");
	QString is_load_colorshift = getNodeValueByName(node, "is_load_colorshift");
	QString is_load_ffc = getNodeValueByName(node, "is_load_ffc");
	QString is_load_distortion = getNodeValueByName(node, "is_load_distortion");
	QString is_load_exposure = getNodeValueByName(node, "is_load_exposure");
	QString is_load_fliprotation = getNodeValueByName(node, "is_load_fliprotation");
	QString is_load_luminance = getNodeValueByName(node, "is_load_luminance");
	QString is_load_fourcolor = getNodeValueByName(node, "is_load_fourcolor");

	CalibrationConfig2 c_config;
	c_config.Aperture = aperture.toStdString() + "mm";
	c_config.Bin = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	c_config.ColorFilterList.clear();
	QStringList c_list = colorfilter_list.split(",");
	for (const QString& item : c_list)
	{
		c_config.ColorFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	//c_config.InputPath;
	c_config.LightSourceList.clear();
	QStringList l_list = lightsource_list.split(",");
	for (const QString& item : l_list)
	{
		c_config.LightSourceList.push_back(item.toStdString());
	}
	c_config.NDFilterList.clear();
	QStringList nd_list = ndfliter_list.split(",");
	for (const QString& item : nd_list)
	{
		c_config.NDFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	c_config.RX = { 0,0,0 };
	CalibrationFlag c_flag;
	c_flag.Dark_Flag = is_load_dark.toInt() == 1;
	c_flag.ColorShift_Flag = is_load_colorshift.toInt() == 1;
	c_flag.FFC_Flag = is_load_ffc.toInt() == 1;
	c_flag.Distortion_Flag = is_load_distortion.toInt() == 1;
	c_flag.Exposure_Flag = is_load_exposure.toInt() == 1;
	c_flag.Flip_Rotate_Flag = is_load_fliprotation.toInt() == 1;
	c_flag.Luminance_Flag = is_load_luminance.toInt() == 1;
	c_flag.FourColor_Flag = is_load_fourcolor.toInt() == 1;
	Result ret = MLColorimeterMode::Instance()->LoadCalibrationData(c_config, c_flag);
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_LoadCalibrationData ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_LoadDarkCalibrationData(BT::TreeNode& node)
{
	// CalibrationConfig2
	QString aperture = getNodeValueByName(node, "aperture");
	QString binning = getNodeValueByName(node, "binning");
	QString colorfilter_list = getNodeValueByName(node, "colorfilter_list");
	QString lightsource_list = getNodeValueByName(node, "lightsource_list");
	QString ndfliter_list = getNodeValueByName(node, "ndfliter_list");

	CalibrationConfig2 c_config;
	c_config.Aperture = aperture.toStdString() + "mm";
	c_config.Bin = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	c_config.ColorFilterList.clear();
	QStringList c_list = colorfilter_list.split(",");
	for (const QString& item : c_list)
	{
		c_config.ColorFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	//c_config.InputPath;
	c_config.LightSourceList.clear();
	QStringList l_list = lightsource_list.split(",");
	for (const QString& item : l_list)
	{
		c_config.LightSourceList.push_back(item.toStdString());
	}
	c_config.NDFilterList.clear();
	QStringList nd_list = ndfliter_list.split(",");
	for (const QString& item : nd_list)
	{
		c_config.NDFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	c_config.RX = { 0,0,0 };
	CalibrationFlag c_flag;
	c_flag.Dark_Flag = true;
	Result ret = MLColorimeterMode::Instance()->LoadCalibrationData(c_config, c_flag);
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_LoadDarkCalibrationData ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_LoadFFCCalibrationData(BT::TreeNode& node)
{
	// CalibrationConfig2
	QString aperture = getNodeValueByName(node, "aperture");
	QString binning = getNodeValueByName(node, "binning");
	QString colorfilter_list = getNodeValueByName(node, "colorfilter_list");
	QString lightsource_list = getNodeValueByName(node, "lightsource_list");
	QString ndfliter_list = getNodeValueByName(node, "ndfliter_list");

	CalibrationConfig2 c_config;
	c_config.Aperture = aperture.toStdString() + "mm";
	c_config.Bin = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	c_config.ColorFilterList.clear();
	QStringList c_list = colorfilter_list.split(",");
	for (const QString& item : c_list)
	{
		c_config.ColorFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	//c_config.InputPath;
	c_config.LightSourceList.clear();
	QStringList l_list = lightsource_list.split(",");
	for (const QString& item : l_list)
	{
		c_config.LightSourceList.push_back(item.toStdString());
	}
	c_config.NDFilterList.clear();
	QStringList nd_list = ndfliter_list.split(",");
	for (const QString& item : nd_list)
	{
		c_config.NDFilterList.push_back(MLColorimeterHelp::instance()->TransStrToFilterEnum(item.toStdString()));
	}
	c_config.RX = { 0,0,0 };
	CalibrationFlag c_flag;
	c_flag.FFC_Flag = true;
	Result ret = MLColorimeterMode::Instance()->LoadCalibrationData(c_config, c_flag);
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_LoadFFCCalibrationData ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_XYZImage_Sync(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString x_exposure_time = getNodeValueByName(node, "x_exposure_time");
	QString y_exposure_time = getNodeValueByName(node, "y_exposure_time");
	QString z_exposure_time = getNodeValueByName(node, "z_exposure_time");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString binning = getNodeValueByName(node, "binning");
	QString ndfilter = getNodeValueByName(node, "nd_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting et_setting_x = { mode, x_exposure_time.toDouble() };
	ExposureSetting et_setting_y = { mode, y_exposure_time.toDouble() };
	ExposureSetting et_setting_z = { mode, z_exposure_time.toDouble() };
	config.colorFilterToExposureMap = { {MLFilterEnum::X, et_setting_x}, {MLFilterEnum::Y, et_setting_y}, {MLFilterEnum::Z, et_setting_z} };
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(ndfilter.toStdString());

	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.is_undistort_method = true;
	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	p_flag.Exposure_Flag = true;
	p_flag.FourColor_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageSyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_XYZImage_Sync ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_GrayImage_Sync(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString exposure_time = getNodeValueByName(node, "exposure_time");
	QString et_factor = getNodeValueByName(node, "et_factor");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString is_hdr = getNodeValueByName(node, "is_hdr");
	QString max_exposure_time = getNodeValueByName(node, "max_exposure_time");
	QString binning = getNodeValueByName(node, "binning");
	QString nd_filter = getNodeValueByName(node, "nd_filter");
	QString color_filter = getNodeValueByName(node, "color_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd_filter.toStdString());
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.hdrConfig.isHDR = is_hdr.toInt() > 0;
	config.hdrConfig.maxExposure = max_exposure_time.toDouble();
	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.is_undistort_method = true;

	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting setting{ mode, exposure_time.toDouble() };

	// judge et factor to get new exposure setting
	if (!qFuzzyCompare(et_factor.toDouble(), 1.0))
	{
		Result ret = MLColorimeterMode::Instance()->TransExposureSetting(mode, et_factor.toDouble(), setting);
		if (!ret.success)
		{
			QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Sync ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	else
	{
		// Fixed mode + no exposure time -> search for the exposure time in the configuration by name
		if (mode == ExposureMode::Fixed && exposure_time.isEmpty())
		{
			double time = 0.0;
			QString name = MLColorimeterMode::Instance()->AnalyzeImageName(nameRule, nd_filter, light_source, image_type, eyeboxid, color_filter);
			Result ret = MLColorimeterMode::Instance()->GetFixExposureTime(name.toStdString(), eyeboxid.toStdString(), time);
			if (!ret.success)
			{
				QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Sync ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}
			setting.ExposureTime = time;
		}
	}

	config.colorFilterToExposureMap = {
	{MLColorimeterHelp::instance()->TransStrToFilterEnum(color_filter.toStdString()), setting}
	};

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageSyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Sync ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_LuminanceImage_Sync(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString exposure_time = getNodeValueByName(node, "exposure_time");
	QString et_factor = getNodeValueByName(node, "et_factor");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString is_hdr = getNodeValueByName(node, "is_hdr");
	QString max_exposure_time = getNodeValueByName(node, "max_exposure_time");
	QString binning = getNodeValueByName(node, "binning");
	QString nd_filter = getNodeValueByName(node, "nd_filter");
	QString color_filter = getNodeValueByName(node, "color_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd_filter.toStdString());
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.hdrConfig.isHDR = is_hdr.toInt() > 0;
	config.hdrConfig.maxExposure = max_exposure_time.toDouble();
	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.is_undistort_method = true;

	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Exposure_Flag = true;
	p_flag.Luminance_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting setting{ mode, exposure_time.toDouble() };

	// judge et factor to get new exposure setting
	if (!qFuzzyCompare(et_factor.toDouble(), 1.0))
	{
		Result ret = MLColorimeterMode::Instance()->TransExposureSetting(mode, et_factor.toDouble(), setting);
		if (!ret.success)
		{
			QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Sync ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	else
	{
		// Fixed mode + no exposure time -> search for the exposure time in the configuration by name
		if (mode == ExposureMode::Fixed && exposure_time.isEmpty())
		{
			double time = 0.0;
			QString name = MLColorimeterMode::Instance()->AnalyzeImageName(nameRule, nd_filter, light_source, image_type, eyeboxid, color_filter);
			Result ret = MLColorimeterMode::Instance()->GetFixExposureTime(name.toStdString(), eyeboxid.toStdString(), time);
			if (!ret.success)
			{
				QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Sync ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}
			setting.ExposureTime = time;
		}
	}

	config.colorFilterToExposureMap = {
	{MLColorimeterHelp::instance()->TransStrToFilterEnum(color_filter.toStdString()), setting}
	};

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageSyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Sync ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_XYZImage_Async(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString x_exposure_time = getNodeValueByName(node, "x_exposure_time");
	QString y_exposure_time = getNodeValueByName(node, "y_exposure_time");
	QString z_exposure_time = getNodeValueByName(node, "z_exposure_time");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString binning = getNodeValueByName(node, "binning");
	QString ndfilter = getNodeValueByName(node, "nd_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting et_setting_x = { mode, x_exposure_time.toDouble() };
	ExposureSetting et_setting_y = { mode, y_exposure_time.toDouble() };
	ExposureSetting et_setting_z = { mode, z_exposure_time.toDouble() };
	config.colorFilterToExposureMap = { {MLFilterEnum::X, et_setting_x}, {MLFilterEnum::Y, et_setting_y}, {MLFilterEnum::Z, et_setting_z} };
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(ndfilter.toStdString());
	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.is_undistort_method = true;
	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	p_flag.Exposure_Flag = true;
	p_flag.FourColor_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageAsyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_XYZImage_Async ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_GrayImage_Async(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString exposure_time = getNodeValueByName(node, "exposure_time");
	QString et_factor = getNodeValueByName(node, "et_factor");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString is_hdr = getNodeValueByName(node, "is_hdr");
	QString max_exposure_time = getNodeValueByName(node, "max_exposure_time");
	QString binning = getNodeValueByName(node, "binning");
	QString nd_filter = getNodeValueByName(node, "nd_filter");
	QString color_filter = getNodeValueByName(node, "color_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd_filter.toStdString());
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.hdrConfig.isHDR = is_hdr.toInt() > 0;
	config.hdrConfig.maxExposure = max_exposure_time.toDouble();
	config.is_undistort_method = true;

	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting setting{ mode, exposure_time.toDouble() };

	// judge et factor to get new exposure setting
	if (!qFuzzyCompare(et_factor.toDouble(), 1.0))
	{
		Result ret = MLColorimeterMode::Instance()->TransExposureSetting(mode, et_factor.toDouble(), setting);
		if (!ret.success)
		{
			QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Async ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	else
	{
		// Fixed mode + no exposure time -> search for the exposure time in the configuration by name
		if (mode == ExposureMode::Fixed && exposure_time.isEmpty())
		{
			double time = 0.0;
			QString name = MLColorimeterMode::Instance()->AnalyzeImageName(nameRule, nd_filter, light_source, image_type, eyeboxid, color_filter);
			Result ret = MLColorimeterMode::Instance()->GetFixExposureTime(name.toStdString(), eyeboxid.toStdString(), time);
			if (!ret.success)
			{
				QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Async ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}
			setting.ExposureTime = time;
		}
	}

	config.colorFilterToExposureMap = {
	{MLColorimeterHelp::instance()->TransStrToFilterEnum(color_filter.toStdString()), setting}
	};

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageAsyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_GrayImage_Async ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Capture_LuminanceImage_Async(BT::TreeNode& node)
{
	QString aperture = getNodeValueByName(node, "aperture");
	QString dutid = getNodeValueByName(node, "dutid");
	QString eyeboxid = getNodeValueByName(node, "eyeboxid");
	QString is_auto_et = getNodeValueByName(node, "is_auto_et");
	QString exposure_time = getNodeValueByName(node, "exposure_time");
	QString et_factor = getNodeValueByName(node, "et_factor");
	QString is_convert_16bit = getNodeValueByName(node, "is_convert_16bit");
	QString convert_method = getNodeValueByName(node, "convert_method");
	QString save_raw_image = getNodeValueByName(node, "save_raw_image");
	QString save_calibration_image = getNodeValueByName(node, "save_calibration_image");
	QString is_hdr = getNodeValueByName(node, "is_hdr");
	QString max_exposure_time = getNodeValueByName(node, "max_exposure_time");
	QString binning = getNodeValueByName(node, "binning");
	QString nd_filter = getNodeValueByName(node, "nd_filter");
	QString color_filter = getNodeValueByName(node, "color_filter");
	QString light_source = getNodeValueByName(node, "light_source");
	QString image_type = getNodeValueByName(node, "image_type");
	QString nameRule = getNodeValueByName(node, "nameRule");

	ImageCaptureConfig config;
	config.binning = MLColorimeterMode::Instance()->TransIntToBinning(binning.toInt());
	//norx
	//config.rx = MLColorimeterMode::Instance()->GetRX();
	config.isSaveCali = save_calibration_image.toInt() > 0;
	config.isSaveRaw = save_raw_image.toInt() > 0;
	config.ndFilter = MLColorimeterHelp::instance()->TransStrToFilterEnum(nd_filter.toStdString());
	config.lightSource = light_source.toStdString();
	config.aperture = aperture.toStdString() + "mm";
	config.hdrConfig.isHDR = is_hdr.toInt() > 0;
	config.hdrConfig.maxExposure = max_exposure_time.toDouble();
	config.is_undistort_method = true;

	CalibrationFlag p_flag;
	p_flag.Dark_Flag = true;
	p_flag.ColorShift_Flag = true;
	p_flag.Distortion_Flag = true;
	p_flag.FFC_Flag = true;
	p_flag.Exposure_Flag = true;
	p_flag.Luminance_Flag = true;
	p_flag.Flip_Rotate_Flag = true;
	config.calibrationFlag = p_flag;

	SaveDataMeta s_config;
	s_config.DutID = dutid.toStdString();
	s_config.EyeboxID = eyeboxid.toStdString();
	s_config.ImageType = image_type.toStdString();
	s_config.SavePath = MetricsData::instance()->getMTFImgsDir().toStdString();
	s_config.Is_Convert = is_convert_16bit.toInt() > 0;
	s_config.ConvertMethod = convert_method.toInt();
	s_config.ImageNameRule = nameRule.toStdString();
	config.saveDataMeta = s_config;

	ExposureMode mode = is_auto_et.toInt() == 1 ? ExposureMode::Auto : ExposureMode::Fixed;
	ExposureSetting setting{ mode, exposure_time.toDouble() };

	// judge et factor to get new exposure setting
	if (!qFuzzyCompare(et_factor.toDouble(), 1.0))
	{
		Result ret = MLColorimeterMode::Instance()->TransExposureSetting(mode, et_factor.toDouble(), setting);
		if (!ret.success)
		{
			QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Async ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	else
	{
		// Fixed mode + no exposure time -> search for the exposure time in the configuration by name
		if (mode == ExposureMode::Fixed && exposure_time.isEmpty())
		{
			double time = 0.0;
			QString name = MLColorimeterMode::Instance()->AnalyzeImageName(nameRule, nd_filter, light_source, image_type, eyeboxid, color_filter);
			Result ret = MLColorimeterMode::Instance()->GetFixExposureTime(name.toStdString(), eyeboxid.toStdString(), time);
			if (!ret.success)
			{
				QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Async ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}
			setting.ExposureTime = time;
		}
	}

	config.colorFilterToExposureMap = {
	{MLColorimeterHelp::instance()->TransStrToFilterEnum(color_filter.toStdString()), setting}
	};

	Result result = MLColorimeterMode::Instance()->GetCaptureTaskManager()->CaptureImageAsyncTask(config);
	if (!result.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_Capture_LuminanceImage_Async ] run error, %1").arg(QString::fromStdString(result.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_WaitForCaptureEnd(BT::TreeNode& node)
{
	QString timeout = getNodeValueByName(node, "timeout");

	Result ret1 = MLColorimeterMode::Instance()->GetCaptureTaskManager()->GetNormalTasksResult(timeout.toInt());
	if (!ret1.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_WaitForCaptureEnd ] run error, %1").arg(QString::fromStdString(ret1.errorMsg));
		LoggingWrapper::instance()->error(message);
	}
	Result ret2 = MLColorimeterMode::Instance()->GetCaptureTaskManager()->GetFourcolorTasksResult(timeout.toInt());
	if (!ret2.success)
	{
		QString message = QString("Recipe Node [ Colorimeter_WaitForCaptureEnd ] run error, %1").arg(QString::fromStdString(ret2.errorMsg));
		LoggingWrapper::instance()->error(message);
	}
	if (!ret1.success || !ret2.success) {
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetTestState(BT::TreeNode& node)
{
	QString is_dut = getNodeValueByName(node, "is_dut");
	QString size = getNodeValueByName(node, "size");
	QString is_update_slb = getNodeValueByName(node, "is_update_slb");
	MLUtils::TestState state;
	state.IsDut = is_dut.toInt() > 0;
	state.IsUpdateSLB = is_update_slb.toInt() > 0;
	state.size = MLUtils::MLUtilCommon::instance()->TransIntToSize(size.toInt());
	MetricsData::instance()->SetTestState(state);
	std::string type = MLUtils::MLUtilCommon::instance()->TransSizeToStr(state.size);
	node.setOutput("size_key", type);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Metrics_CreateDBTable()
{
	QString eyeboxs = MetricsData::instance()->getEyeboxQueue();
	QStringList eyeboxlist = eyeboxs.split(",", Qt::SkipEmptyParts);
	bool ok = GenCSVHeader::crateMetricsTable(eyeboxlist);
	if (!ok)
	{
		QString message = QString("Recipe Node [ IQ_Metrics_CreateDBTable ] run error, create Metrics table errors");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	Result ret = GenCSVHeader::writeCsvHeadersToDB();
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ IQ_Metrics_CreateDBTable ] run error, Write csv headers to db error");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Metrics_CreateResultFile(BT::TreeNode& node)
{
	QString root_dir = getNodeValueByName(node, "root_dir");
	QString dut_seq = getNodeValueByName(node, "dut_seq");
	QString csvname = getNodeValueByName(node, "csvname").remove(',');
	QString all_csvname = getNodeValueByName(node, "all_csvname").remove(',');

	QString all_csv_path = root_dir + QDir::separator() + all_csvname + ".csv";
	QString csv_path = root_dir + QDir::separator() + dut_seq + QDir::separator() + csvname + ".csv";
	QString iq_dir = root_dir + QDir::separator() + dut_seq + QDir::separator() + "IQ" + QDir::separator();
	//QString throughfocus_dir = root_dir + QDir::separator() + dut_seq + QDir::separator() + "ThroughFocus" + QDir::separator();

	MetricsData::instance()->setCsvPath(csv_path, all_csv_path);
	MetricsData::instance()->setDutTestSeqName(dut_seq);
	MetricsData::instance()->setMTFImgsDir(iq_dir);
	bool ret = MLColorimeterMode::Instance()->isDirExist(iq_dir);
	if (!ret)
	{
		QString message = QString("Recipe Node [ IQ_Metrics_CreateResultFile ] run error, create iq_dir error!");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	QFile file(all_csv_path);
	if (!file.exists())
	{
		bool ok = false;
		ok = IQTModel::instance()->createMetricsResutCSVFile(all_csv_path);
		if (!ok)
		{
			QString message = QString("Recipe Node [ IQ_Metrics_CreateResultFile ] run error, create all csv file error!");
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	QFile file1(csv_path);
	if (!file1.exists())
	{
		bool ok = false;
		ok = IQTModel::instance()->createMetricsResutCSVFile(csv_path);
		if (!ok)
		{
			QString message = QString("Recipe Node [ IQ_Metrics_CreateResultFile ] run error, create csv file error!");
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Metrics_Calculate(BT::TreeNode& node)
{
	QString metrics_name = getNodeValueByName(node, "metrics_name");
	try {
		Result res = IQTModel::instance()->calulateOneMetricsByRecipe(metrics_name);
		if (!res.success)
		{
			QString message = QString("Recipe Node [ IQ_Metrics_Calculate ] run error, Metrics %1 %2").arg(metrics_name).arg(QString::fromStdString(res.errorMsg));
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
	}
	catch (const std::exception e)
	{
		QString message = QString("Recipe Node [ IQ_Metrics_Calculate ] run error, Metrics %1 %2").arg(metrics_name).arg(e.what());
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT::IQT4Recipe::IQ_Metrics_Location_Clear(BT::TreeNode& node)
{
	MLIQMetrics::MLNineCrossDetection cross;
	cross.setDetectionFlag(false, MLIQMetrics::SMALLFOV);
	cross.setDetectionFlag(false, MLIQMetrics::BIGFOV);
	MLIQMetrics::MLSolidDetection solid;
	solid.setDetectionFlag(false, MLIQMetrics::SMALLFOV);
	solid.setDetectionFlag(false, MLIQMetrics::BIGFOV);
	//MLIQMetrics::isNineCrossDetectionFlagBig = false;
	//MLIQMetrics::isNineCrossDetectionFlagSmall = false;
	//MLIQMetrics::isSolidDetectionFlagBig = false;
	//MLIQMetrics::isSolidDetectionFlagSmall = false;
	return NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Metrics_ExportToCSV()
{
	QString csvPath = MetricsData::instance()->getCsvPath();
	QString allcsvPath = MetricsData::instance()->getAllCsvPath();

	Result ret = IQTModel::instance()->saveRecipeMetricsToCsv(csvPath);
	if (!ret.success)
	{
		QString message = QString("Recipe Node [ IQ_Metrics_ExportToCSV ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	QString testresult = QString::fromStdString(ret.errorMsg);
	IQTModel::instance()->saveRecipeMetricsToCsv(allcsvPath);
	IQTModel::instance()->deleteDataBase();

	//transpose csv
	bool res = IQTModel::instance()->transposeCsv(csvPath);
	res = IQTModel::instance()->transposeCsv(allcsvPath);

	if (!testresult.contains("PASS")) {
		QString message = QString("Recipe Node [ IQ_Metrics_ExportToCSV ] run error, %1").arg(testresult);
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_ThroughFocus_Diopter(BT::TreeNode& node) {

	RegisterStopHandler(node);

	QString file_name = getNodeValueByName(node, "file_name");
	QString direction = getNodeValueByName(node, "direction");
	QString diopter = getNodeValueByName(node, "diopter");
	QString scan_range = getNodeValueByName(node, "scan_range");
	QString rough_step_num = getNodeValueByName(node, "rough_step_num");
	QString is_use_fine = getNodeValueByName(node, "is_use_fine");
	QString fine_step_num = getNodeValueByName(node, "fine_step_num");
	QString is_chess_mode = getNodeValueByName(node, "is_chess_mode");
	QString is_lpmm_unit = getNodeValueByName(node, "is_lpmm_unit");
	QString freq = getNodeValueByName(node, "freq");
	QString is_save_images = getNodeValueByName(node, "is_save_images");

	DiopterScanConfig config;
	config.DiopterMax = diopter.toDouble() + scan_range.toDouble() / 2.0;
	config.DiopterMin = diopter.toDouble() - scan_range.toDouble() / 2.0;
	config.RoughStepNumber = rough_step_num.toInt();
	config.IsUseFineAdjust = is_use_fine.toInt() > 0;
	config.FineStepNumber = fine_step_num.toInt();
	config.ChessMode = is_chess_mode.toInt() > 0;
	config.LpmmUnit = is_lpmm_unit.toInt() > 0;
	config.StoreResultImg = is_save_images.toInt() > 0;
	config.Freq = freq.toDouble();

	Result ret = AutoFocusModel::instance()->DiopterScanProcess(config, file_name.toStdString(), direction.toStdString());
	if (!ret.success)
	{
		QString message = QString::fromStdString("Recipe Node [ Colorimeter_Motion_AutoFocus_Diopter ] run error, " + ret.errorMsg);
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT::IQT4Recipe::IQ_StartParallelCalculate(BT::TreeNode& node)
{
	QStringList metricslist = getNodeValueByName(node, "metrics").split(",", Qt::SkipEmptyParts);
	QStringList eyeboxlist = getNodeValueByName(node, "eyeboxlist").split(",", Qt::SkipEmptyParts);

	Result res = MetricsProcessorProxy::GetInstance()->StartParallelCalculate(metricslist, eyeboxlist);
	if (!res.success)
	{
		QString message = QString("Recipe Node [ IQ_StartParallelCalculate ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT::IQT4Recipe::IQ_StopParallelCalculate(BT::TreeNode& node)
{
	Result res = MetricsProcessorProxy::GetInstance()->StopParallelCalculate();
	if (!res.success)
	{
		QString message = QString("Recipe Node [ IQ_StopParallelCalculate ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT::IQT4Recipe::IQ_WaitParallelCalculateEnd(BT::TreeNode& node)
{
	int TimeOut = 100;
	QString sTimeOut = getNodeValueByName(node, "timeout");

	if (!sTimeOut.isEmpty())
		TimeOut = sTimeOut.toInt();

	std::atomic<bool> stop_flag(false);

	std::future<void> result = std::async(std::launch::async, [&stop_flag, &node]()
		{
			while (1)
			{
				if (stop_flag.load() || node.IsStopChildNodes()) {
					break;
				}

				if (IQ_TaskState::Idle == MetricsProcessorProxy::GetInstance()->getRunningStatus())
					break;

				QCoreApplication::processEvents();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		});

	std::future_status status = result.wait_for(std::chrono::seconds(TimeOut));

	if (future_status::timeout == status)
	{
		qInfo() << "future_status::timeout";
		stop_flag = true;
		result.get();
		MetricsProcessorProxy::GetInstance()->StopParallelCalculate();
		return BT::NodeStatus::SUCCESS;
	}
	else if (future_status::ready == status)
	{
		qInfo() << "future_status::ready";
		MetricsProcessorProxy::GetInstance()->ClearCache();
		return BT::NodeStatus::SUCCESS;
	}
	else if (future_status::deferred == status)
	{
		MetricsProcessorProxy::GetInstance()->ClearCache();
		return BT::NodeStatus::FAILURE;
	}

	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_ThroughFocus_MotionPos(BT::TreeNode& node) {

	QString file_name = getNodeValueByName(node, "file_name");
	QString direction = getNodeValueByName(node, "direction");
	QString focus_max = getNodeValueByName(node, "focus_max");
	QString focus_min = getNodeValueByName(node, "focus_min");
	QString rough_step = getNodeValueByName(node, "rough_step");
	QString is_use_fine = getNodeValueByName(node, "is_use_fine");
	QString fine_range = getNodeValueByName(node, "fine_range");
	QString fine_step = getNodeValueByName(node, "fine_step");
	QString is_chess_mode = getNodeValueByName(node, "is_chess_mode");
	QString is_lpmm_unit = getNodeValueByName(node, "is_lpmm_unit");
	QString freq = getNodeValueByName(node, "freq");

	FocusScanConfig config;
	config.FocusMin = focus_max.toDouble();
	config.FocusMax = focus_min.toDouble();
	config.RoughStep = rough_step.toDouble();
	config.IsUseFineAdjust = is_use_fine > 0;
	config.FineRange = fine_range.toDouble();
	config.FineStep = fine_step.toDouble();
	config.ChessMode = is_chess_mode > 0;
	config.LpmmUnit = is_lpmm_unit > 0;
	config.Freq = freq.toDouble();

	Result ret = AutoFocusModel::instance()->FocusScanProcess(config, file_name.toStdString(), direction.toStdString());
	if (!ret.success)
	{
		QString message = QString::fromStdString("Recipe Node [ Colorimeter_Motion_AutoFocus_Position ] run error, " + ret.errorMsg);
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetEyeboxList(BT::TreeNode& node)
{
	QString eyeboxlist = getNodeValueByName(node, "eyeboxlist");
	if (eyeboxlist == "preset")
	{
		QStringList ids = GenCSVHeader::getMetricsEyeboxIds();
		eyeboxlist = ids.join(",");
	}
	MetricsData::instance()->setEyeboxQueue(eyeboxlist);
	node.setOutput("key", eyeboxlist.toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_Metrics_LoadLimitConfig(BT::TreeNode& node)
{
	QString file_name = getNodeValueByName(node, "file_name");
	QString file_path = "./config/limit/" + file_name;
	IQMetricConfig::instance()->loadLimitInfos(file_path.toStdString().c_str());
	QString message = QString("load limit file " + file_path + " finish.");
	LoggingWrapper::instance()->info(message);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_SetDutID(BT::TreeNode& node)
{
	QString dutid = getNodeValueByName(node, "id_value");
	MetricsData::instance()->setDutBarCode(dutid);
	node.setOutput("id_key", dutid.toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_UpdateFlipRotation()
{
	// function is cancled
	return BT::NodeStatus::FAILURE;

	MLUtils::TestState state = MetricsData::instance()->GetTestState();
	if (state.IsDut && state.eyeType == MLUtils::EyeType::EyeType_UnKnown)
	{
		QString message = QString::fromStdString("Recipe Node [ IQ_UpdateFlipRotation ] run error, current eye type is unknown.");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	Flipping flip_x; Flipping flip_y; ML::MLColorimeter::Rotation rotate;
	if (state.IsDut && state.eyeType == MLUtils::EyeType::Right)
		ImageRotationConfig::instance()->GetFilpRotationConfig_DutRightEye(flip_x, flip_y, rotate);
	else if (state.IsDut && state.eyeType == MLUtils::EyeType::Left)
		ImageRotationConfig::instance()->GetFilpRotationConfig_DutLeftEye(flip_x, flip_y, rotate);
	else
		ImageRotationConfig::instance()->GetFilpRotationConfig_SLB(flip_x, flip_y, rotate);
	if (!MLColorimeterMode::Instance()->UpdateFlipRotation(flip_x, flip_y, rotate))
	{
		QString message = QString::fromStdString("Recipe Node [ IQ_UpdateFlipRotation ] run error, update Flip_Rotate.json error.");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_LoadFixExposureTime(BT::TreeNode& node)
{
	QString is_match_eb = getNodeValueByName(node, "is_match_eb");
	MLColorimeterMode::Instance()->SetFixExposureTimeMatchRule(is_match_eb.toInt() > 0);
	if (MLColorimeterMode::Instance()->ReadFixExposureTime())
		return BT::NodeStatus::SUCCESS;
	return BT::NodeStatus::FAILURE;
}

NodeStatus IQT4Recipe::IQ_DeleteFixExposureTime()
{
	if (MLColorimeterMode::Instance()->DeleteFixExposureTimeCsv())
		return BT::NodeStatus::SUCCESS;
	return BT::NodeStatus::FAILURE;
}

NodeStatus IQT4Recipe::IQ_UpdateFixExposureTimeConfig()
{
	Result ret = MLColorimeterMode::Instance()->UpdateFixExposureTimeConfig(MetricsData::instance()->getMTFImgsDir());
	if (!ret.success)
	{
		QString message = QString::fromStdString("Recipe Node [ IQ_UpdateFixExposureTimeConfig ] run error, " + ret.errorMsg);
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IQT4Recipe::IQ_CameraSimulator_Create(BT::TreeNode& node)
{
	int timer_interval = 5;

	QString root_dir = getNodeValueByName(node, "img_folder");
	QString s_timer_interval = getNodeValueByName(node, "timer_interval");

	if (!s_timer_interval.isEmpty())
		timer_interval = s_timer_interval.toInt();

	bool ret = CameraSimulator::getInstance()->isDirExist(root_dir);
	if (!ret)
	{
		QString message = QString("Recipe Node [ IQ_CameraSimulator_Create ] run error, create root_dir error!");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}

	CameraSimulator::getInstance()->initialize(root_dir, timer_interval);
	return BT::NodeStatus::SUCCESS;

}

NodeStatus IQT4Recipe::IQ_CameraSimulator_StartWork(BT::TreeNode& node)
{
	if (CameraSimulator::getInstance()->startWork(node))
		return BT::NodeStatus::SUCCESS;
	else
		return BT::NodeStatus::FAILURE;
}

bool saveToCSV(const std::string& filename, const std::map<double, double>& data)
{
	std::ofstream file(filename);
	if (!file.is_open()) {
		return false;
	}
	file << "Angle,Grayscale\n";

	for (const auto& p : data) {
		file << p.first << "," << p.second << "\n";
	}
	file.close();
	return true;
}

NodeStatus IQT4Recipe::IQ_Polarizer_AutoScan(BT::TreeNode& node)
{
	double max_angle = getNodeValueByName(node, "max_angle").toDouble();
	double min_angle = getNodeValueByName(node, "min_angle").toDouble();
	double step = getNodeValueByName(node, "step").toDouble();
	int roi_x = getNodeValueByName(node, "roi_x").toInt();
	int roi_y = getNodeValueByName(node, "roi_y").toInt();
	int roi_width = getNodeValueByName(node, "roi_width").toInt();
	int roi_height = getNodeValueByName(node, "roi_height").toInt();
	QString save_dir = getNodeValueByName(node, "save_dir");

	try
	{
		QString time = QDateTime::currentDateTime().toString("yyyyMMddTHHmmss");
		if (max_angle <= min_angle || step < 0)
		{
			QString message = QString("Recipe Node [ IQ_Polarizer_AutoScan ] run error, scan range is invalid!");
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
		QString path = save_dir + "\\images_"+ time;
		MLColorimeterMode::Instance()->isDirExist(path);
		std::map<double, double>grayMap;

		double angle = min_angle;
		while (true)
		{
			Result res = ThorlabsMode::instance()->AbsMoveSync(angle);
			if (!res.success)
			{
				QString message = QString("Recipe Node [ IQ_Polarizer_AutoScan ] run error, %1").arg(QString::fromStdString(res.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}
			Sleep(500);

			cv::Mat image;
			res = MLColorimeterMode::Instance()->GetImage(image);
			if (!res.success)
			{
				QString message = QString("Recipe Node [ IQ_Polarizer_AutoScan ] run error, %1").arg(QString::fromStdString(res.errorMsg));
				LoggingWrapper::instance()->error(message);
				return BT::NodeStatus::FAILURE;
			}

			cv::imwrite(path.toStdString() + "\\" + std::to_string(angle) + ".tif", image);
			if (roi_x < 0 || roi_y < 0 ||
				roi_x + roi_width  > image.cols ||
				roi_y + roi_height > image.rows)
			{
				LoggingWrapper::instance()->error("Recipe Node [ IQ_Polarizer_AutoScan ] run error, roi is out of image range!");
				return BT::NodeStatus::FAILURE;
			}

			cv::Mat roi = image(cv::Rect(roi_x, roi_y, roi_width, roi_height)).clone();
			double meanVal = cv::mean(roi)[0];
			grayMap[angle] = meanVal;

			if (angle >= max_angle)
				break;

			double nextAngle = angle + step;
			angle = (nextAngle > max_angle) ? max_angle : nextAngle;
		}
		std::string filename = (save_dir + "\\polarizer_scan_" + time + ".csv").toStdString();
		if (!saveToCSV(filename, grayMap))
		{
			QString message = QString("Recipe Node [ IQ_Polarizer_AutoScan ] run error, save to %1 error!").arg(save_dir);
			LoggingWrapper::instance()->error(message);
			return BT::NodeStatus::FAILURE;
		}
		return BT::NodeStatus::SUCCESS;
	}
	catch (const std::exception& ex)
	{
		QString message = QString("Recipe Node [ IQ_Polarizer_AutoScan ] run error, %1").arg(QString::fromStdString(ex.what()));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
}

NodeStatus IQT4Recipe::IQ_Metrics_Luminance_Dialog(BT::TreeNode& node)
{
	QString color = getNodeValueByName(node, "color");
	QString eyebox = getNodeValueByName(node, "eyebox");

	double lum = MetricsDataBase::getInstance()->queryOneMetricValue("5", QString("Luminance(Nits)_%1_Luminance_0%2Eyebox").arg(color).arg(eyebox),"value");
	double lumUniform = MetricsDataBase::getInstance()->queryOneMetricValue("5", QString("Luminance(Nits)_%1_LuminanceUniformity_0%2Eyebox").arg(color).arg(eyebox), "value");
	double p5 = MetricsDataBase::getInstance()->queryOneMetricValue("5", QString("Luminance(Nits)_%1_P5_0%2Eyebox").arg(color).arg(eyebox), "value");
	double p50 = MetricsDataBase::getInstance()->queryOneMetricValue("5", QString("Luminance(Nits)_%1_P50_0%2Eyebox").arg(color).arg(eyebox), "value");
	double p95 = MetricsDataBase::getInstance()->queryOneMetricValue("5", QString("Luminance(Nits)_%1_P95_0%2Eyebox").arg(color).arg(eyebox), "value");
	QString msg = QString(
		"Luminance: %1\n"
		"Luminance Uniformity: %2\n"
		"P5: %3\n"
		"P50: %4\n"
		"P95: %5")
		.arg(lum)
		.arg(lumUniform)
		.arg(p5)
		.arg(p50)
		.arg(p95);

	QMetaObject::invokeMethod(
		qApp,
		[=]() {
			QMessageBox::information(nullptr, "Metric Values", msg);
		},
		Qt::QueuedConnection
			);
	return BT::NodeStatus::SUCCESS;
}