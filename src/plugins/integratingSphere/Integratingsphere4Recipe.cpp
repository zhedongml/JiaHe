#include "Integratingsphere4Recipe.h"
#include "integratingspheretcpmodel.h"
#include "loggingwrapper.h"
#include <PrjCommon\metricsdata.h>
#include <QMessageBox>
#include <QApplication>
#include <QtWidgets\qinputdialog.h>

using namespace IntegratingSphere;
IntegratingSphere4Recipe* IntegratingSphere4Recipe::self = nullptr;
IntegratingSphere4Recipe* IntegratingSphere4Recipe::getInstance() {

	if (!self) 
	{
		self = new IntegratingSphere4Recipe();
	}
	return self;
}

IntegratingSphere4Recipe::IntegratingSphere4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("IntegratingSphere4Recipe", static_cast<void*>(this));
}

IntegratingSphere4Recipe::~IntegratingSphere4Recipe()
{
}

QString IntegratingSphere4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
    auto f_value = node.getInput<std::string>(name);
    if (!f_value)
    {
        throw BT::RuntimeError("Missing input [force]: ", f_value.error());
    }
    return QString::fromStdString(f_value.value());
}

NodeStatus IntegratingSphere4Recipe::Connect()
{
	Result res = IntegratingSphereTCPModel::getInstance()->Connect();
	if (!res.success)
	{
        QString message = QString("Recipe Node [ IS_Connect ] run error, %1").arg(QString::fromStdString(res.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
	}

    return BT::NodeStatus::SUCCESS;
}

NodeStatus IntegratingSphere4Recipe::Disconnect()
{
	IntegratingSphereTCPModel::getInstance()->Disconnect();
	return BT::NodeStatus::SUCCESS;
}

NodeStatus IntegratingSphere4Recipe::SetSolution(BT::TreeNode& node)
{
    QString light_source = getNodeValueByName(node, "light_source");
    QString isphere_scale = getNodeValueByName(node, "isphere_scale");

    QString ISColor = light_source.toUpper();
    QList<QString> colors;
    colors << "R" << "G" << "B" << "W";
    while (!colors.contains(ISColor)) {
        QString input;
        bool ok = false;

        QMetaObject::invokeMethod(qApp, [&]() {
            QString tip = QString("Invalid color \"%1\". Please enter one of R, G, B, or W:")
                .arg(ISColor);

            input = QInputDialog::getText(
                nullptr,
                "Invalid Color",
                tip,
                QLineEdit::Normal,
                "",
                &ok
            );
            }, Qt::BlockingQueuedConnection);

        if (!ok || input.isEmpty()) {
            return NodeStatus::FAILURE;
        }

        ISColor = input.toUpper();
    }

    QList<QString> ISValues = isphere_scale.split(",");
    if (isphere_scale.contains('&')) {
        ISValues = isphere_scale.split("&");
    }
    Result res;
    {
        if (ISValues.size() == 0)
        {
            QString message = QString("Recipe Node [ IS_SetSolution ] run error, Labsphere scale is missing");
            LoggingWrapper::instance()->error(message);
            return BT::NodeStatus::FAILURE;

        }

        if (ISColor == "W" && ISValues.size() < 3)
        {
            QString message = QString("Recipe Node [ IS_SetSolution ] run error, Labsphere W color scale is incomplete, size of 3 is required");
            LoggingWrapper::instance()->error(message);
            return BT::NodeStatus::FAILURE;
        }

        QMap<QString, float> ISValueMap;
        if (ISColor == "R")
        {
            ISValueMap["R"] = ISValues.at(0).toFloat();
        }
        else if (ISColor == "G")
        {
            ISValueMap["G"] = ISValues.at(0).toFloat();
        }
        else if (ISColor == "B")
        {
            ISValueMap["B"] = ISValues.at(0).toFloat();
        }
        else if (ISColor == "W")
        {
            ISValueMap["R"] = ISValues.at(0).toFloat();
            ISValueMap["G"] = ISValues.at(1).toFloat();
            ISValueMap["B"] = ISValues.at(2).toFloat();
        }

        res = IntegratingSphereTCPModel::getInstance()->setCurrentOutput(ISColor, ISValueMap);
        if (!res.success)
        {
            QString message = QString("Recipe Node [ IS_SetSolution ] run error, %1").arg(QString::fromStdString(res.errorMsg));
            LoggingWrapper::instance()->error(message);
            return BT::NodeStatus::FAILURE;
        }
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus IntegratingSphere::IntegratingSphere4Recipe::CloseAllShutter()
{
    Result res;
    res = IntegratingSphereTCPModel::getInstance()->setShutterTurn("W", false, true);
    if (!res.success)
    {
        QString message = QString("Recipe Node [ IS_CloseAllShutter ] run error, %1").arg(QString::fromStdString(res.errorMsg));
        LoggingWrapper::instance()->error(message);
        return BT::NodeStatus::FAILURE;
    }
    return BT::NodeStatus::SUCCESS;
}

NodeStatus IntegratingSphere4Recipe::SetCurLuminance(BT::TreeNode& node)
{
    QString color = getNodeValueByName(node, "color_name");
    double luminance = getNodeValueByName(node, "lumin").toDouble();

    MetricsData::instance()->setISPluminanceData(color.toLower(), luminance);
    return BT::NodeStatus::SUCCESS;
}