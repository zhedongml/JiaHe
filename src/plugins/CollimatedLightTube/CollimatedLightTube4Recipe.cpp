#include "CollimatedLightTube4Recipe.h"
#include "CollimatedLightTubeMode.h"
#include "CollimatedConfig.h"
#include "loggingwrapper.h"
#include "PLCController.h"
#include <QDir>


CollimatedLightTube4Recipe* CollimatedLightTube4Recipe::self = nullptr;

CollimatedLightTube4Recipe* CollimatedLightTube4Recipe::getInstance()
{
	if (!self)
	{
		self = new CollimatedLightTube4Recipe();
	}
	return self;
}

CollimatedLightTube4Recipe::CollimatedLightTube4Recipe(QObject* parent)
	:QObject(parent)
{
	ObjectManager::getInstance()->registerObject("CollimatedLightTube4Recipe", static_cast<void*>(this));
}

CollimatedLightTube4Recipe::~CollimatedLightTube4Recipe()
{
}

QString CollimatedLightTube4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus CollimatedLightTube4Recipe::Collimator_Connect()
{
	std::string sn = CollimatedConfig::instance()->GetCollimatorSn();
	Result ret = CollimatedLightTubeMode::GetInstance()->Connect(sn.c_str());
	if (!ret.success)
	{
		QString strerr = QString("Recipe [CollimatedLightTubeMode:Collimator_Connect] run error,%1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(strerr);
		return NodeStatus::FAILURE;
	}
	return NodeStatus::SUCCESS;
}

NodeStatus CollimatedLightTube4Recipe::Collimator_Close()
{
	bool ret = CollimatedLightTubeMode::GetInstance()->DisConnect();
	if (!ret)
	{
		QString strerr = QString("Recipe [CollimatedLightTubeMode:Collimator_Close] run error");
		LoggingWrapper::instance()->error(strerr);
		return NodeStatus::FAILURE;
	}
	return NodeStatus::SUCCESS;
}

NodeStatus CollimatedLightTube4Recipe::Collimator_SetExposure(BT::TreeNode& node)
{
	double dexposure = getNodeValueByName(node, "exposure").toDouble();
	bool bauto = getNodeValueByName(node, "is_auto").toInt();
	if (bauto)
	{
		CollimatedLightTubeMode::GetInstance()->SetMLExposureAuto();
	}
	else
	{
		CollimatedLightTubeMode::GetInstance()->SetExposureTime(dexposure);
	}
	return NodeStatus::SUCCESS;
}

NodeStatus CollimatedLightTube4Recipe::Collimator_SaveAngleToCsv(BT::TreeNode& node)
{
	QString file_name = getNodeValueByName(node, "file_name");

	QDir dir(file_name);
	bool ok = false;
	if (dir.exists())
		ok = true;
	else
		ok = dir.mkpath(file_name);
	if (!ok)
	{
		QString strerr = QString("Recipe Node [ Collimator_SaveAngleToCsv ] run error, %1 is not exist!").arg(file_name);
		LoggingWrapper::instance()->error(strerr);
		return BT::NodeStatus::FAILURE;
	}
	file_name += "\\collimator_angle.csv";
	Result ret = PLCController::instance()->collimatorLight(true);
	if (!ret.success)
	{
		QString strerr = QString("Recipe Node [ Collimator_SaveAngleToCsv ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(strerr);
		return BT::NodeStatus::FAILURE;
	}
	QString angleX = CollimatedLightTubeMode::GetInstance()->GetCollimatorAngleX();
	QString angleY = CollimatedLightTubeMode::GetInstance()->GetCollimatorAngleY();
	PLCController::instance()->collimatorLight(false);
	ret = CollimatedLightTubeMode::GetInstance()->WriteAngleToCSV(file_name, angleX, angleY);
	if (!ret.success)
	{
		QString strerr = QString("Recipe Node [ Collimator_SaveAngleToCsv ] run error, %1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(strerr);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

