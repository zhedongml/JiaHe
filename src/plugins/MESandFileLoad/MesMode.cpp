#include "MesMode.h"
#include "PrjCommon/MetricsCheck.h"
#include "PrjCommon/CsvDequeRecorder.h"
#include "MESandFileLoad/TcpModel.h"
#include "PrjCommon/compressFile/CompressFile.h"
#include "MesConfig.h"
#include <QDebug>
#include "MesTaskAsync.h"
#include "MyHTTPClient.h"
#include "Core/loggingwrapper.h"

using namespace Core;

MesMode& MesMode::instance()
{
	static MesMode self;
	return self;
}

MesMode::MesMode()
{
	m_dataUpload = new TcpModel();
	m_zipStop.store(false);
}

MesMode::~MesMode()
{
	if(m_dataUpload){
		delete m_dataUpload;
		m_dataUpload = nullptr;
	}
}

Result MesMode::singleLensLoad()
{
	if(!MesConfig::instance().getMesOnline()){
		m_judgment = "1";
		return Result();
	}

	SingleLensUpRequestData data;
	data.id = QUuid::createUuid().toString();
	data.lenscode = "";  // waveguide code, not filled in
	data.WGcode = MetricsData::instance()->getDutSN(); 
	data.mac = MesConfig::instance().getMac();
	data.ADcode = MesConfig::instance().getADcode();

	LoggingWrapper::instance()->debug("Single Lens Load post, " + data.toStr());

	SingleLensUpResponseData responseDatas;
	MyHTTPClient httpClient;
	Result ret = httpClient.sendPostRequest(responseDatas, data);
	LoggingWrapper::instance()->debug("Single Lens Load response, " + responseDatas.toStr());

	if(!ret.success){
		return Result(false, "" + ret.errorMsg);
	}

	if(!responseDatas.success){
		return Result(false, QString("Pending operator confirmation, clear the abnormalities, and then re-execute the test. ERROR is %1.").arg(responseDatas.message).toStdString());
	}

	m_judgment = responseDatas.judgment;
	return Result();
}

Result MesMode::singleLensUnload()
{
	//TODO: test
	bool success = true;
	return singleLensUnload(success);
}

Result MesMode::singleLensUnload(bool success)
{
	if (!MesConfig::instance().getMesOnline()) {
		m_judgment = "1";
		return Result();
	}

	SingleLensDownRequestData data;
	data.id = QUuid::createUuid().toString();
	data.qualifiedtype = "0";// 0: Normal product 1: Release with concession
	data.rbfxcode = "null"; // Release with concession item,  defective item number
	data.rbfxvalue = "null"; // Concession release sample value
	data.lenscode = ""; 
	data.WGcode = MetricsData::instance()->getDutSN();   // WG(Product Code)
	data.judgment = QString::number(getTestCount());

	data.mac = MesConfig::instance().getMac();
	data.ADcode = MesConfig::instance().getADcode();

	if (success) {
		data.cbpcode.append("null");
		data.cstatus = "OK";
		data.dvehiclecode = MesConfig::instance().getTrayCodeOK();
	}
	else {
		data.cbpcode.append(MesConfig::instance().getCbpcodeNG());
		data.cstatus = "NG";
		data.dvehiclecode = MesConfig::instance().getTrayCodeNG();
	}

	LoggingWrapper::instance()->debug("Single Lens Unload post, " + data.toStr());

	SingleLensDownResponseData responseDatas;
	MyHTTPClient httpClient;
	Result ret = httpClient.sendPostRequest(responseDatas, data);
	LoggingWrapper::instance()->debug("Single Lens Unload response, " + responseDatas.toStr());
	if (!ret.success) {
		return Result(false, "" + ret.errorMsg);
	}

	if (!responseDatas.success) {
		return Result(false, QString("Pending operator confirmation, clear the abnormalities, and then re-execute the test. ERROR is %1.").arg(responseDatas.message).toStdString());
	}
	return Result();
}

Result MesMode::setCuttingPrevent(QString trayCode)
{
	if (!MesConfig::instance().getMesOnline()) {
		return Result();
	}

	VehicledesignRequestData data;
	data.id = QUuid::createUuid().toString();
	data.mac = MesConfig::instance().getMac();
	data.dvehiclecode = trayCode;

	LoggingWrapper::instance()->debug("Cutting Prevent post, " + data.toStr());

	SingleLensDownResponseData responseDatas;
	MyHTTPClient httpClient;
	Result ret = httpClient.sendPostRequest(responseDatas, data);
	LoggingWrapper::instance()->debug("Cutting Prevent response, " + responseDatas.toStr());

	if (!ret.success) {
		return Result(false, "" + ret.errorMsg);
	}

	if (!responseDatas.success) {
		return Result(false, QString("Pending operator confirmation, clear the abnormalities, and then re-execute the test. ERROR is %1.").arg(responseDatas.message).toStdString());
	}

	return Result();
}

Result MesMode::sendTcp(ExternalTOSPathbody data)
{
	//data.csvPath = "E:\\pcdata\\AutoDPResult\\WGZS9-R-A2C1_20251126T162746_MetricsTest\\sunny_omnilight_Plaid_Cary7K-01-WG_1_WGZS9-R-A2C1_FAIL_20251126163407.qdf.csv";
	//data.zipPath.push_back("E:\\pcdata\\AutoDPResult\\WGZS9-R-A2C1_20251126T162746_MetricsTest.zip");

	data.use = getUse(true);
	//data.csvPath = MetricsData::instance()->getAllCsvDir().toStdString();
	//data.zipPath = zipPaths;

	Result ret = m_dataUpload->sendData(data);
	if (!ret.success) {
		return Result(false, "Upload ZIP error, " + ret.errorMsg);
	}

	qWarning() << QString("Upload ZIP, send data success.");
	return Result();
}

Result MesMode::sendTcpRD(ExternalTOSPathToRDbody data)
{
	data.use = getUse(false);
	//data.lensId = MetricsData::instance()->getDutSN().toStdString();
	if (data.lensId.empty()) {
		data.lensId = QString::number(QDateTime::currentMSecsSinceEpoch()).toStdString();
	}
	//data.filePath = filePathList;

	Result ret = m_dataUpload->sendData(data);
	if (!ret.success) {
		return Result(false, "Upload file error, " + ret.errorMsg);
	}

	qWarning() << QString("Upload file, send data success.");
	return Result();
}

Result MesMode::zipDataUpload()
{
	//TODO: test
	//string imgDir = "D:/WG/20250925/SingleEyebox/LG3HL_20250925T105745_MetricsTest/LG3HL_EB05_20250925T105849/IQ";
	//string imgDir2 = "D:/WG/20250925/SingleEyebox/LG3HL_20250925T110452_MetricsTest/LG3HL_EB05_20250925T110555/IQ";

	QString imgDir = MetricsData::instance()->getMTFImgsDir();

	std::vector<std::string> imgDirs;
	imgDirs.push_back(imgDir.toStdString());
	//imgDirs.push_back(imgDir2);

	std::vector<std::string> dirPaths;
	std::vector<std::string> zipPaths;
	Result ret = getZipPaths(imgDirs, dirPaths, zipPaths);
	if(!ret.success){
		return ret;
	}

	ExternalTOSPathbody data;
	data.use = getUse(true);
	data.csvPath = MetricsData::instance()->getAllCsvDir().toStdString();
	data.zipPath = zipPaths;

	ret = MesTaskAsync::instance().uploadZip(data, dirPaths, zipPaths);
	return ret;
}

Result MesMode::fileDataUpload()
{
	QString dirPath = MetricsData::instance()->getMTFImgsDir();
	QDir dir(dirPath);
	dir.setNameFilters(QStringList() << "*.tif");
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QFileInfoList fileList = dir.entryInfoList();

	//TODO: IQ image
	std::vector<std::string> filePathList;
	for (const QFileInfo& fileInfo : fileList) {
		filePathList.push_back(fileInfo.absoluteFilePath().toStdString());
	}

	ExternalTOSPathToRDbody data;
	data.use = getUse(false);
	data.lensId = MetricsData::instance()->getDutSN().toStdString();
	if(data.lensId.empty()){
		data.lensId = QString::number(QDateTime::currentMSecsSinceEpoch()).toStdString();
	}
	data.filePath = filePathList;

	Result ret = m_dataUpload->sendData(data);
	if (!ret.success) {
		return Result(false, "Upload data ZIP error, " + ret.errorMsg);
	}

	return Result();
}

Result MesMode::zipUploadAsync(const ExternalTOSPathbody &data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths)
{
	if(dirPaths.size() != zipPaths.size()){
		return Result(false, "Zip upload error, dir paths number is not equal zip paths number.");
	}

	if(dirPaths.size() == 0){
		return Result(false, "Zip upload error, dir paths number is 0.");
	}

	int startTime = QDateTime::currentMSecsSinceEpoch();
	Result ret;
	for(int i = 0; i < dirPaths.size(); ++i){
		if(m_zipStop.load()){
			return Result(false, "Zip upload error, manual stop.");
		}

		QString dirPath = QString::fromStdString(dirPaths[i]);
		QString zipPath = QString::fromStdString(zipPaths[i]);
		ret = CompressFile().compressedFile(dirPath, zipPath);
		if (!ret.success) {
			return ret;
		}
	}

	int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
	qWarning() << QString("ZIP data upload, compress file time: %1 ms, ZIP file number is %2, First fileis %3.")
		.arg(takeTime)
		.arg(zipPaths.size())
		.arg(QString::fromStdString(zipPaths[0]));

	qWarning() << "Send zip upload, " + data.toStr();
	ret = m_dataUpload->sendData(data);
	if (!ret.success) {
		return Result(false, "Upload data ZIP error, " + ret.errorMsg);
	}

	qWarning() << QString("ZIP data upload, send data success.");
	return Result();
}

Result MesMode::loadAndRunAdpTask()
{
	const std::deque<DutMeasureInfo>& dutQueue = CsvDequeRecorder::GetInstance()->getDeque();
	
	for (DutMeasureInfo item : dutQueue) {
		runAdpTask(item);
	}

	//if (!ret.success)
	//	return Result(false, "MesTaskAsync: runQdp error.");

	return Result();
}

Result MesMode::runAdpTask(DutMeasureInfo info)
{
	bool checkEnabled = info.CheckEnabled;
	QString modelName = info.ModelName;
	bool liveRefresh  = info.LiveRefresh;

	//AutoDP_WG
	MetricsCheck::instance().setCheckEnabled_autoDP(checkEnabled);
	Result ret = MetricsCheck::instance().setModelName_autoDP(modelName, liveRefresh);
	if (!ret.success)
		return Result(false, "set model name autoDP error.");

	QString sourceImageDir = info.DutDir;
	QString resultSaveDir = info.AdpResultDir;
	QString tempFolder = QString::fromStdString(ImageProcessor::instance().getTempDirPath(sourceImageDir.toStdString()));

	if (false == QFileInfo(sourceImageDir).exists())
	{
		if (false == QFileInfo(tempFolder).exists())
		{
			return Result(false, "The DUT folder sourceImageDir or tempFolder is not existed");
		}
		else
		{
			sourceImageDir = tempFolder;
		}
	}

	QString msg = QString("sourceImageDir: %1  resultSaveDir: %2").arg(sourceImageDir).arg(resultSaveDir);
	LoggingWrapper::instance()->info(msg);

	ret = MesTaskAsync::instance().runQdp(sourceImageDir.toStdString(),
		resultSaveDir.toStdString());

	if (!ret.success)
		return Result(false, "MesTaskAsync: runQdp error.");


	return Result();
}

Result MesMode::zipUpload(const ExternalTOSPathbody& data, const std::vector<std::string>& dirPaths, const std::vector<std::string>& zipPaths)
{
	if (dirPaths.size() != zipPaths.size()) {
		return Result(false, "Zip upload error, dir paths number is not equal zip paths number.");
	}

	if (dirPaths.size() == 0) {
		return Result(false, "Zip upload error, dir paths number is 0.");
	}

	int startTime = QDateTime::currentMSecsSinceEpoch();
	Result ret;

	//TODO: Async
	{
		QList<std::shared_ptr<CompressFile>> compressFileList;
		for (int i = 0; i < dirPaths.size(); ++i) {
			compressFileList.push_back(std::make_shared<CompressFile>());
		}

		QList<QFuture<Result>> futures;
		for (int i = 0; i < dirPaths.size(); ++i) {
			if (m_zipStop.load()) {
				return Result(false, "Zip upload error, manual stop.");
			}

			QString dirPath = QString::fromStdString(dirPaths[i]);
			QString zipPath = QString::fromStdString(zipPaths[i]);
			QFuture<Result> future = QtConcurrent::run(compressFileList[i].get(), &CompressFile::compressedFile, dirPath, zipPath);
			futures.append(future);
		}

		QFutureSynchronizer<Result> synchronizer;
		for (QFuture<Result>& future : futures) {
			synchronizer.addFuture(future);
		}
		synchronizer.waitForFinished();

		std::string errorMsg;
		int number = 0;
		for (std::string zip : zipPaths) {
			Result ret = futures[number++].result();
			if (!ret.success) {
				errorMsg = errorMsg.empty() ? ret.errorMsg : errorMsg + "\n" + ret.errorMsg;
			}
		}

		if (!errorMsg.empty()) {
			return Result(false, errorMsg);
		}
	}

	int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
	qWarning() << QString("ZIP data upload, compress file time: %1 ms, ZIP file number is %2, First fileis %3.")
		.arg(takeTime)
		.arg(zipPaths.size())
		.arg(QString::fromStdString(zipPaths[0]));

	qWarning() << "Send zip upload, " + data.toStr();
	ret = m_dataUpload->sendData(data);
	if (!ret.success) {
		return Result(false, "Upload data ZIP error, " + ret.errorMsg);
	}

	qWarning() << QString("ZIP data upload, send data success.");
	return Result();
}

void MesMode::notifyStop(bool isstop)
{
	m_zipStop.store(isstop);
}

void MesMode::setMesBaseInfo(const ControlData& data)
{
	m_mesBaseInfo = data;
}

int MesMode::getTestCount()
{
	if(!MesConfig::instance().getMesOnline()){
		return 1;
	}

	if(m_judgment.trimmed().isEmpty()){
		return 1;
	}

	int judgement = m_judgment.trimmed().toInt();
	if(judgement == 0){
		return 1;
	}

	return judgement;
}

string MesMode::getUse(bool isZip)
{
	bool swDebug = MesConfig::instance().getSWDebug();
	if(swDebug){
		if(isZip){
			return "SWDebug";
		}else{
			return "SWDebug/toRD";
		}
	}else{
		if (isZip) {
			return "ToClient";
		}
		else {
			return "ToClient/toRD";
		}
	}

	return string();
}

Result MesMode::compressedFiles()
{
	return Result();
}

Result MesMode::getZipPaths(const std::vector<std::string>& imgPathsRelative, std::vector<std::string>& dirPaths, std::vector<std::string>& zipPaths)
{
	for(string imgDir: imgPathsRelative){
		if(imgDir.empty()){
			return Result(false, QString("MES zip path is empty.").toStdString());
		}

		QDir dir(QString::fromStdString(imgDir));
		if(!dir.exists()){
			return Result(false, QString("MES zip path is not exists, ").arg("imgDir").toStdString());
		}

		dir.cdUp();
		dir.cdUp();
		QString dirPath = dir.absolutePath();
		dirPaths.push_back(dirPath.toStdString());

		QString folderName = dir.dirName();
		dir.cdUp();
		QString upPath = dir.absolutePath();
		QString zipPath = upPath + "/" + folderName + ".zip";
		zipPaths.push_back(zipPath.toStdString());

		QFile zipFile(zipPath);
		if (zipFile.exists()) {
			qWarning() << QString("ZIP data upload, compressed zip file already exists, file is %1.").arg(zipPath);
			//return Result(false, QString("ZIP data upload, compressed zip file already exists, file is %1.").arg(zipPath).toStdString());
		}
	}

	return Result();
}

void MesMode::DetectIncompleteAdpTask()
{
	int size = CsvDequeRecorder::GetInstance()->size();

	if (size > 0)
	{
		QString info = QString("The application detected that there were %1 unfinished AutoDP tasks after the last exiting. Do you want to continue processing them?\n\n"
			"Click [Confirm] to continue processing.\n"
			"Click [Ignore] to ignore and clear the unfinished AutoDP task.\n").arg(size);

		bool ok = MetricsData::instance()->popTipsDialog("Warning", info, "Confirm", "Ignore");

		if (ok)
			MesMode::instance().loadAndRunAdpTask();
		else
			MetricsData::instance()->resetAdpHistoryQueue();
	}
}