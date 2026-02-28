#include "DUTMotion4Recipe.h"
#include "OrientalMotorControl.h"
#include "loggingwrapper.h"

using namespace DUTMotion;

DUTMotion4Recipe* DUTMotion4Recipe::self = nullptr;
DUTMotion4Recipe* DUTMotion4Recipe::getInstance()
{
	if (!self)
	{
		self = new DUTMotion4Recipe();
	}
	return self;
}

DUTMotion4Recipe::DUTMotion4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("DUTMotion4Recipe", static_cast<void*>(this));
}

DUTMotion4Recipe::~DUTMotion4Recipe()
{
}

QString DUTMotion4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [", name, "]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus DUTMotion::DUTMotion4Recipe::OM_Open(BT::TreeNode& node)
{
	QString module = getNodeValueByName(node, "module");

	int nType = OrientalAxle::DX;
	if (module == "X")
		nType = OrientalAxle::DX;
	else if (module == "Y")
		nType = OrientalAxle::DY;
	else if (module == "Z")
		nType = OrientalAxle::DZ;

	OrientalMotorControl* montor = OrientalMotorControl::getInstance();
	Result ret;
	if (nullptr != montor)
	{
		ret = montor->Connect();
	}
	else
	{
		ret = Result(false, "can not get montor");
	}
	if (!ret.success)
	{
		QString strerr = QString("Recipe [OrientalMontor: OM_Open] run error,%1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(strerr);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus DUTMotion::DUTMotion4Recipe::OM_Close(BT::TreeNode& node)
{
	QString module = getNodeValueByName(node, "module");

	int nType = OrientalAxle::DX;
	if (module == "X")
		nType = OrientalAxle::DX;
	else if (module == "Y")
		nType = OrientalAxle::DY;
	else if (module == "Z")
		nType = OrientalAxle::DZ;

	OrientalMotorControl* montor = OrientalMotorControl::getInstance();
	Result ret;
	if (nullptr != montor)
	{
		ret = montor->Disconnect();
	}
	else
	{
		ret = Result(false, "can not get montor");
	}
	if (!ret.success)
	{
		QString strerr = QString("Recipe [OrientalMontor: OM_Open] run error,%1").arg(QString::fromStdString(ret.errorMsg));
		LoggingWrapper::instance()->error(strerr);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus DUTMotion4Recipe::OM_Degree_Async(BT::TreeNode& node)
{
	QString module = getNodeValueByName(node, "module");
	QString degree = getNodeValueByName(node, "degree");

	int nType = OrientalAxle::DX;
	if (module == "X")
		nType = OrientalAxle::DX;
	else if (module == "Y")
		nType = OrientalAxle::DY;
	else if (module == "Z")
		nType = OrientalAxle::DZ;

	OrientalMotorControl* motorControl = OrientalMotorControl::getInstance();
	Result res = motorControl->MoveByDegreeAsync(nType, degree.toDouble());
	if (!res.success)
	{
		QString message = QString("Recipe [ OrientalMotor : OM_Degree_Async ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}

NodeStatus DUTMotion4Recipe::OM_Degree_Sync(BT::TreeNode& node)
{
	QString module = getNodeValueByName(node, "module");
	QString degree = getNodeValueByName(node, "degree");

	int nType = OrientalAxle::DX;
	if (module == "X")
		nType = OrientalAxle::DX;
	else if (module == "Y")
		nType = OrientalAxle::DY;
	else if (module == "Z")
		nType = OrientalAxle::DZ;

	OrientalMotorControl* motorControl = OrientalMotorControl::getInstance();
	Result res = motorControl->MoveByDegreeSync(nType, degree.toDouble());
	if (!res.success)
	{
		QString message = QString("Recipe [ OrientalMotor : OM_Degree_Sync ] run error, %1").arg(QString::fromStdString(res.errorMsg));
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	return BT::NodeStatus::SUCCESS;
}