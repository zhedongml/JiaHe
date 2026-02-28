#include "System4Recipe.h"
#include "loggingwrapper.h"
#include <QtConcurrent/QtConcurrent>
#include <PrjCommon\Session.h>
#include <QtWidgets\qmessagebox.h>
#include "PrjCommon\metricsdata.h"
#include <filesystem>
#include <QInputDialog>
#include <PrjCommon/MultiLineInputDialog.h>

using namespace System;
System4Recipe* System4Recipe::self = nullptr;
System4Recipe* System4Recipe::getInstance() {

	if (!self)
	{
		self = new System4Recipe();
	}
	return self;
}

System4Recipe::System4Recipe(QObject* parent)
	: QObject(parent)
{
	ObjectManager::getInstance()->registerObject("System4Recipe", static_cast<void*>(this));
}

System4Recipe::~System4Recipe()
{
}

QString System4Recipe::getNodeValueByName(BT::TreeNode& node, std::string name)
{
	auto f_value = node.getInput<std::string>(name);
	if (!f_value)
	{
		throw BT::RuntimeError("Missing input [force]: ", f_value.error());
	}
	return QString::fromStdString(f_value.value());
}

NodeStatus System4Recipe::System_Sleep(BT::TreeNode& node)
{
	QString sleep_time = getNodeValueByName(node, "sleep_time");
	//QThread::msleep(sleep_time.toInt());
	_sleep(sleep_time.toInt() * 1000);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_Date(BT::TreeNode& node)
{
	QDate current_date = QDate::currentDate();
	std::string currentDate = current_date.toString("yyyyMMdd").toStdString();
	node.setOutput("date_key", currentDate);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_DateTime(BT::TreeNode& node)
{
	QDateTime time = QDateTime().currentDateTime();
	std::string currentDateTime = time.toString("yyyyMMddTHHmmss").toStdString();
	node.setOutput("date_time_key", currentDateTime);
	MetricsData::instance()->setStartRecipeTime(time);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_InitValue(BT::TreeNode& node)
{
	QString value = getNodeValueByName(node, "value");
	node.setOutput("value_key", value.toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_GetForNodeValue1(BT::TreeNode& node)
{
	std::string param;
	node.config().blackboard->get("For",param);
	node.setOutput("value_key", param);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_GetForNodeValue2(BT::TreeNode& node)
{
	std::string param;
	node.config().blackboard->get("For", param);
	QStringList p_list = QString::fromStdString(param).split(",");
	if (p_list.size() != 2)
	{
		QString message = QString("Recipe Node [ System_GetForNodeValue2 ] run error, loop variable size of the for loop is incorrect");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	node.setOutput("value_key1", p_list[0].toStdString());
	node.setOutput("value_key2", p_list[1].toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_GetForNodeValue3(BT::TreeNode& node)
{
	std::string param;
	node.config().blackboard->get("For", param);
	QStringList p_list = QString::fromStdString(param).split(",");
	if (p_list.size() != 3)
	{
		QString message = QString("Recipe Node [ System_GetForNodeValue3 ] run error, loop variable size of the for loop is incorrect");
		LoggingWrapper::instance()->error(message);
		return BT::NodeStatus::FAILURE;
	}
	node.setOutput("value_key1", p_list[0].toStdString());
	node.setOutput("value_key2", p_list[1].toStdString());
	node.setOutput("value_key3", p_list[2].toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_CatStr2(BT::TreeNode& node)
{
	QString str1 = getNodeValueByName(node, "str1");
	QString str2 = getNodeValueByName(node, "str2");
	std::string res = (str1 + "_" + str2).toStdString();
	node.setOutput("value_key", res);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_CatStr3(BT::TreeNode& node)
{
	QString str1 = getNodeValueByName(node, "str1");
	QString str2 = getNodeValueByName(node, "str2");
	QString str3 = getNodeValueByName(node, "str3");
	std::string res = (str1 + "_" + str2 + "_" + str3).toStdString();
	node.setOutput("value_key", res);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_CatStr4(BT::TreeNode& node)
{
	QString str1 = getNodeValueByName(node, "str1");
	QString str2 = getNodeValueByName(node, "str2");
	QString str3 = getNodeValueByName(node, "str3");
	QString str4 = getNodeValueByName(node, "str4");
	std::string res = (str1 + "_" + str2 + "_" + str3 + "_" + str4).toStdString();
	node.setOutput("value_key", res);
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_ParseStr3(BT::TreeNode& node)
{
	QString str = getNodeValueByName(node, "str");
	QStringList list = str.split("_");
	if (list.size() != 3)
	{
		LoggingWrapper::instance()->error("Recipe Node [ System_ParseStr3 ] run error, list size is error!");
		return NodeStatus::FAILURE;
	}
	node.setOutput("value_key1", list[0].toStdString());
	node.setOutput("value_key2", list[1].toStdString());
	node.setOutput("value_key3", list[2].toStdString());
	return BT::NodeStatus::SUCCESS;
}

NodeStatus System4Recipe::System_Tips(BT::TreeNode& node)
{
	QString tip = getNodeValueByName(node, "tip");

	QMessageBox::StandardButton result = QMessageBox::No;
	QMetaObject::invokeMethod(qApp, [&]() {
		result = QMessageBox::question(
			nullptr,
			"Tips",
			tip,
			QMessageBox::Yes | QMessageBox::No
		);
		}, Qt::BlockingQueuedConnection); 

	if (result == QMessageBox::Yes) {
		return NodeStatus::SUCCESS;
	}
	else {
		LoggingWrapper::instance()->error("Not meeting the current prompt information conditions!");
		return NodeStatus::FAILURE;
	}
}

NodeStatus System4Recipe::System_InputTips(BT::TreeNode& node)
{
	QString tip = getNodeValueByName(node, "tip");
	bool is_lable_visual = getNodeValueByName(node, "is_lable_visual").toInt() > 0;
	QString param = getNodeValueByName(node, "param");

	QStringList labels;
	if (param.contains(",")) {
		labels = param.split(",", Qt::SkipEmptyParts);
	}
	else {
		bool ok = false;
		int count = param.toInt(&ok);
		if (ok && count > 0) {
			for (int i = 1; i <= count; ++i)
				labels << QString("input item %1").arg(i);
		}
		else {
			labels << param;
		}
	}

	QStringList results;
	bool ok = false;

	QMetaObject::invokeMethod(qApp, [&]() {
		MultiLineInputDialog dlg(tip, labels, !is_lable_visual);
		if (dlg.exec() == QDialog::Accepted) {
			results = dlg.texts();
			ok = true;
		}
		}, Qt::BlockingQueuedConnection);

	if (ok) {
		QString combined = results.join("_");
		node.setOutput("text_key", combined.toStdString());

		QStringList parts;
		for (int i = 0; i < labels.size() && i < results.size(); ++i) {
			parts << QString("%1: %2").arg(labels[i], results[i]);
		}
		QString output = parts.join(" | ");
		LoggingWrapper::instance()->info(QString("----------------------------- %1 -----------------------------").arg(output));
		return NodeStatus::SUCCESS;
	}
	else {
		LoggingWrapper::instance()->error("Recipe Node [ System_Split ] run error,operation canceled or invalid input!");
		return NodeStatus::FAILURE;
	}
}

NodeStatus System::System4Recipe::System_Split(BT::TreeNode& node)
{
	QString str = getNodeValueByName(node, "input_str");
	QString separator = getNodeValueByName(node, "separator");
	QString pos = getNodeValueByName(node, "pos_for_return");
	QStringList parts = str.split(separator, Qt::SkipEmptyParts);
	bool ok;
	std::string splitRes;
	int num = pos.toInt(&ok);
	if (!ok)
	{
		QString message = QString("Recipe Node [ System_Split ] run error,the input position is not a int number");
		LoggingWrapper::instance()->error(message);
		return NodeStatus::FAILURE;
	}
	else
	{
		if (num < 0 || num >= parts.size())
		{
			QString message = QString("Recipe Node [ System_Split ] run error,the input position out of range,input position = %1,size = %2").arg(num).arg(parts.size());
			LoggingWrapper::instance()->error(message);
			return NodeStatus::FAILURE;
		}
		else
		{
			splitRes = parts[num].toStdString();
			node.setOutput("split_result_key", splitRes);
			return NodeStatus::SUCCESS;
		}
	}
}

NodeStatus System::System4Recipe::System_Subfolders(BT::TreeNode& node)
{
	QString parent_dir = getNodeValueByName(node, "parent_dir");
	std::vector<string> subfolders;

	std::ostringstream oss;
	bool first = true;

	try {
		for (const auto& entry : std::filesystem::directory_iterator(parent_dir.toStdString())) {
			if (entry.is_directory()) {
				std::string subdir = entry.path().filename().string();
				{
					if (!first) {
						oss << ","; 
					}
					oss << subdir;
					first = false;
				}
			}
		}

		node.setOutput("subfolders_key", oss.str());
		return NodeStatus::SUCCESS;
	}
	catch (const std::exception& e) {
		QString message = QString("Recipe Node [ System_Subfolders ] run error,%1").arg(e.what());
		LoggingWrapper::instance()->error(message);
		return NodeStatus::FAILURE;
	}
}

NodeStatus System::System4Recipe::System_SequenceConversion(BT::TreeNode& node)
{
	QString cycles_num = getNodeValueByName(node, "cycles_num");

	int num = cycles_num.toInt();
	QStringList list;
	list.reserve(cycles_num.toInt());
	for (int i = 1; i <= num; ++i) {
		list.append(QString::number(i));
	}
	std::string cycle_list = list.join(",").toStdString();
	node.setOutput("value_key", cycle_list);
	return NodeStatus::SUCCESS;
}

NodeStatus System::System4Recipe::System_CompareStr(BT::TreeNode& node)
{
	QString origin_str = getNodeValueByName(node, "origin_str");
	QString target_str = getNodeValueByName(node, "target_str");

	if (origin_str == target_str)
		return NodeStatus::SUCCESS;
	else
		return NodeStatus::FAILURE;
}