#include "PImotion4Recipe.h"
#include <opencv2/opencv.hpp>
#include <Core\loggingwrapper.h>
#include "LimitMove.h"
#include "piMotionActor/Motion2DModel.h"
using namespace PImotion;
using namespace AAProcess;
PImotion4Recipe* PImotion4Recipe::self = nullptr;
PImotion4Recipe* PImotion4Recipe::getInstance() {

	if (!self)
	{
		self = new PImotion4Recipe();
	}
	return self;
}

PImotion4Recipe::PImotion4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("PImotion4Recipe", static_cast<void*>(this));
}

PImotion4Recipe::~PImotion4Recipe()
{
}

QString PImotion4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus PImotion4Recipe::PI_2D_AbsProTilt(BT::TreeNode& node)
{
    QString x_position = getNodeValueByName(node, "x_degree");
    QString y_position = getNodeValueByName(node, "y_degree");

    double x = x_position.toDouble();
    double y = y_position.toDouble();

    Motion2DModel* m_Model = Motion2DModel::getInstance(ACS2DPro);
    Result res = m_Model->Motion2DMoveAbs(x, y);
    //std::string msg = LimitMove::getInstance()->motion2DMoveAbs(cv::Point2f(x, y), ModuleName::ProjectionDxDy);

    //if (msg != "")
    //{
    //    QString errMsg = QString::fromStdString(msg);
    //    errMsg = "With Projector tilt " + errMsg;
    //    
    //    QString message = QString("Recipe Node [ PI_2D_AbsProTilt ] run error, %1").arg(errMsg);
    //    LoggingWrapper::instance()->error(message);
    //    return BT::NodeStatus::FAILURE;
    //}
     
    return BT::NodeStatus::SUCCESS;
}

NodeStatus PImotion::PImotion4Recipe::PI_2D_AbsMeasureCameraTilt(BT::TreeNode& node)
{
    QString x_position = getNodeValueByName(node, "x_degree");
    QString y_position = getNodeValueByName(node, "y_degree");

    double x = x_position.toDouble();
    double y = y_position.toDouble();

    Motion2DModel* m_Model = Motion2DModel::getInstance(ACS2DCameraTilt);
    Result res = m_Model->Motion2DMoveAbs(x, y);
    //std::string msg = LimitMove::getInstance()->motion2DMoveAbs(cv::Point2f(x, y), ModuleName::ImagingModuleDxDy);

    //if (msg != "")
    //{
    //    QString errMsg = QString::fromStdString(msg);
    //    errMsg = "With measure camera tilt " + errMsg;

    //    QString message = QString("Recipe Node [ PI_2D_AbsMeasureCameraTilt ] run error, %1").arg(errMsg);
    //    LoggingWrapper::instance()->error(message);
    //    return BT::NodeStatus::FAILURE;
    //}

    return BT::NodeStatus::SUCCESS;
}

NodeStatus PImotion4Recipe::PI_3D_AbsDutMove(BT::TreeNode& node)
{
    QString x_position = getNodeValueByName(node, "x_position");
    QString y_position = getNodeValueByName(node, "y_position");
    QString z_position = getNodeValueByName(node, "z_position");

    cv::Point3f coord;
    coord.x = x_position.toDouble();
    coord.y = y_position.toDouble();
    coord.z = z_position.toDouble();

    std::string msg = LimitMove::getInstance()->motion3DMoveAbsSync(coord, withDUT);
    //res = MoveLimit::moveAbs(coord, withDUT);
    if (msg != "")
    {
        QString message = QString("Recipe Node [ PI_3D_AbsDutMove ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus PImotion4Recipe::PI_3D_AbsMeasureCameraMove(BT::TreeNode& node)
{
    QString x_position = getNodeValueByName(node, "x_position");
    QString y_position = getNodeValueByName(node, "y_position");
    QString z_position = getNodeValueByName(node, "z_position");

    cv::Point3f coord;
    coord.x = x_position.toDouble();
    coord.y = y_position.toDouble();
    coord.z = z_position.toDouble();

   // res = MoveLimit::moveAbs(coord, withCamera);
    std::string msg = LimitMove::getInstance()->motion3DMoveAbsSync(coord, withCamera);
    if (msg != "")
    {
        QString message = QString("Recipe Node [ PI_3D_AbsMeasureCameraMove ] run error, %1").arg(QString::fromStdString(msg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}
