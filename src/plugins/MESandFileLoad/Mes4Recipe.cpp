#include "Mes4Recipe.h"
#include "core/loggingwrapper.h"
#include "MesMode.h"
#include <PrjCommon\metricsdata.h>
#include "PLCControl/PLCController.h"
#include <QDebug>
#include <QCoreApplication>
#include "MesTaskAsync.h"
#include "PrjCommon/MetricsCheck.h"
#include "PrjCommon/CsvDequeRecorder.h"

using namespace MesNS;

Mes4Recipe* Mes4Recipe::self = nullptr;
Mes4Recipe* Mes4Recipe::getInstance() {

	if (!self)
	{
		self = new Mes4Recipe();
	}																																																																																																																																																																																																																																																																																																																																																																																																																																																														
	return self;
}

Mes4Recipe::Mes4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("Mes4Recipe", static_cast<void*>(this));
}

Mes4Recipe::~Mes4Recipe()
{
}

QString Mes4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus Mes4Recipe::Mes_Lens_Load(BT::TreeNode& node)
{
   Result ret = MesMode::instance().singleLensLoad();

   if (ret.success)
       return BT::NodeStatus::SUCCESS;
   else
       return BT::NodeStatus::FAILURE;
}

//NodeStatus Mes4Recipe::Mes_AutoDPResultPath(BT::TreeNode& node)
//{
//    Result ret;
//    QString resultSaveDir = getNodeValueByName(node, "resultSaveDir");
//
//    QStringList list = param.split("/");
//    for (QString paramstr : list)
//    {
//        QStringList paramPair = paramstr.split("=");
//        if (paramPair.size() == 2)
//        {
//            QString key = paramPair[0].trimmed();
//            QString val = paramPair[1].trimmed();
//            if (key == "ResultSaveDir")
//            {
//                m_autoDPResultSaveDir = val.toStdString();
//            }
//        }
//    }
//
//    if (ret.success)
//        return BT::NodeStatus::SUCCESS;
//    else
//        return BT::NodeStatus::FAILURE;
//}

NodeStatus Mes4Recipe::Mes_SubmitAutoDPTask(BT::TreeNode& node)
{
    QString sourceImageDir = getNodeValueByName(node, "sourceImageDir");
    QString resultSaveDir = getNodeValueByName(node, "resultSaveDir");

	QString msg = QString("sourceImageDir: %1  resultSaveDir: %2").arg(sourceImageDir).arg(resultSaveDir);
	LoggingWrapper::instance()->info(msg);

    Result ret = MesTaskAsync::instance().runQdp(sourceImageDir.toStdString(),
        resultSaveDir.toStdString());

	if (ret.success)
		return BT::NodeStatus::SUCCESS;
	else
		return BT::NodeStatus::FAILURE;
}

NodeStatus Mes4Recipe::Mes_SubmitAdpTaskNew(BT::TreeNode& node)
{
	auto _dut = MetricsData::instance()->popMetricsQueueFront();

	LoggingWrapper::instance()->info(QString::fromStdString(MetricsData::instance()->printAllQueue()));

	if (!_dut.has_value()) {
		return BT::NodeStatus::FAILURE;
	}

	DutMeasureInfo _dutInfo = _dut.value();
	MetricsData::instance()->pushDutAdpHistoryQueue(_dutInfo);
	MesMode::instance().runAdpTask(_dutInfo);

	return BT::NodeStatus::SUCCESS;
}

NodeStatus Mes4Recipe::Mes_MetricsLimitAutoDP(BT::TreeNode& node)
{
    bool checkEnabled = getNodeValueByName(node, "checkEnabled").toInt() > 0;
    QString modelName = getNodeValueByName(node, "modelName");
    bool liveRefresh = getNodeValueByName(node, "liveRefresh").toInt()>0;

    MetricsCheck::instance().setCheckEnabled_autoDP(checkEnabled);
    Result ret = MetricsCheck::instance().setModelName_autoDP(modelName, liveRefresh);
    if (ret.success)
        return BT::NodeStatus::SUCCESS;
    else
        return BT::NodeStatus::FAILURE;
}

NodeStatus Mes4Recipe::Mes_Test_Node(BT::TreeNode& node)
{
	AdpDequeueManager::GetInstance()->Test();
	//MesMode::instance().loadAndRunAdpTask();
	return BT::NodeStatus::SUCCESS;;

	DutMeasureInfo _dut1;
	_dut1.StartTime = QDateTime::currentDateTime();
	_dut1.DutIndex = 1;
	_dut1.SN = "111";
	_dut1.ErrorMsg = "111_ERR";

	DutMeasureInfo _dut2;
	_dut2.StartTime = QDateTime::currentDateTime();
	_dut2.DutIndex = 2;
	_dut2.SN = "222";
	_dut2.ErrorMsg = "222_ERR";

	DutMeasureInfo _dut3;
	_dut3.StartTime = QDateTime::currentDateTime();
	_dut3.DutIndex = 3;
	_dut3.SN = "333";
	_dut3.ErrorMsg = "333_ERR";

	DutMeasureInfo _dut4;
	_dut4.StartTime = QDateTime::currentDateTime();
	_dut4.DutIndex = 4;
	_dut4.SN = "444";
	_dut4.ErrorMsg = "444_ERR";

	//MetricsData::instance()->pushDutAutoDPQueue(_dut1);
	MetricsData::instance()->pushDutMetricsQueue(_dut2);
	MetricsData::instance()->pushDutMetricsQueue(_dut3);
	MetricsData::instance()->pushDutMetricsQueue(_dut4);

	//MetricsData::instance()->popMetricsQueueBackSN("111");


	return BT::NodeStatus::SUCCESS;
}