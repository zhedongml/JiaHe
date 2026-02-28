#include "DecoderModel.h"

DecoderModel::DecoderModel(QObject *parent)
	: QObject(parent)
{
	m_tcp.setCallback(this);
	loadFile();

	string ip;
	int port;
	getIpPort(ip, port);

	m_tcp.setIpPort(ip, port);
}

DecoderModel::~DecoderModel()
{}

DecoderModel& DecoderModel::instance(QObject * parent)
{
	static DecoderModel self;
	return self;
}

Result DecoderModel::connect()
{
	string ip;
	int port;
	getIpPort(ip, port);

	Result ret = m_tcp.start(ip, port, false);
	return ret;
}

void DecoderModel::disconnect()
{
	m_tcp.close();
	return;
}

Result DecoderModel::queryBarcode(QString& barcode)
{
	string recvInfo;
	Result ret = m_tcp.queryRecv(recvInfo, "LON");
	if(ret.success){
		barcode = QString::fromStdString(recvInfo);

		if (barcode.toUpper() == "ERROR") {
			return Result(false, "Read DUT SN is ERROR, Please check if you have placed a DUT?");
		}
	}
	return ret;
}

Result DecoderModel::captureImage(QString& imagePath)
{
	string recvInfo;
	Result ret = m_tcp.queryRecv(recvInfo, "SHOT,b");
	if (ret.success) {
		imagePath = QString::fromStdString(recvInfo);
	}
	return ret;
}

void DecoderModel::NotifyMotionStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state)
{
	if(new_state == CORE::MLMotionState::kMLStatusFailed)
	{
		emit connectStatus(true, "Connection failed.");
	}
	else if(new_state == CORE::MLMotionState::kMLStatusConnected)
	{
		emit connectStatus(true, "Connected.");
	}
	else
	{
		emit connectStatus(true, "Disconnected.");
	}
}

void DecoderModel::getIpPort(string& ip, int& port)
{
	ip = m_json["IP"].get<string>();
	port = m_json["Port"].get<int>();
}

Result DecoderModel::loadFile()
{
    std::ifstream file(FILE_NAME.toStdString());
    if (file.is_open())
    {
        std::string contents = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();
        m_json = Json::parse(contents);
    }
    else
    {
		return Result(false, QString("Load file error, %1").arg(FILE_NAME).toStdString());
    }
    return Result();
}
