#pragma once

#include <QObject>
#include "DecoderTcp.h"
#include "PrjCommon/service/ml_motion.h"
#include "json.hpp"
#include <fstream>

using Json = nlohmann::json;
using namespace std;

class DecoderModel  : public QObject, public CoreMotionCallback
{
	Q_OBJECT

public:
	static DecoderModel& instance(QObject* parent = nullptr);
	~DecoderModel();

	Result connect();
	void disconnect();

	Result queryBarcode(QString &barcode);
	Result captureImage(QString & imagePath);

	virtual void NotifyMotionStateChanged(CORE::MLMotionState old_state, CORE::MLMotionState new_state);
	virtual void NotifyMotionPosition(int position){}

signals:
	void connectStatus(bool isOpened, QString msg);

private:
	DecoderModel(QObject* parent = nullptr);
	void getIpPort(string &ip, int &port);
	Result loadFile();

private:
	DecoderTcp m_tcp;

	const QString FILE_NAME= "./config/device/barcodeReader.json";
	Json m_json;
};
