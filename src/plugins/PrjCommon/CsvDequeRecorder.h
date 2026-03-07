#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <deque>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <filesystem>
#include "metricsdata.h"
#include <QObject>
#include <QHash>
#include <QString>
#include <QVariant>
#include <mutex>
#include <type_traits>
#include <stdexcept>

namespace fs = std::filesystem;

class CsvDequeRecorder {
private:
	mutable std::mutex mtx_;
	std::deque<DutMeasureInfo> data_deque;
	std::string filename = ".\\config\\AdpCsv\\AdpCsv.csv";;
	char delimiter = ',';
	const std::string m_Dir = ".\\config\\AdpCsv";
	//const std::string m_FilePath = ".\\config\\AdpCsv\\AdpCsv.csv";

	std::string serializeToCSV(const DutMeasureInfo& item) {
		std::ostringstream oss;

		//if constexpr (std::is_same_v<T, DutMeasureInfo>) 
		{
			oss << "\"" << item.SN.toStdString() << "\"" << delimiter
				<< "\"" << item.StartTime.toString("yyyy-MM-dd HH:mm:ss").toStdString() << "\"" << delimiter
				<< "\"" << item.EndTime.toString("yyyy-MM-dd HH:mm:ss").toStdString() << "\"" << delimiter
				<< "\"" << item.DutDir.toStdString() << "\"" << delimiter
				<< "\"" << item.AdpResultDir.toStdString() << "\"" << delimiter
				<< "\"" << item.ErrorMsg.toStdString() << "\"" << delimiter
				<< item.DutIndex << delimiter
				<< item.LayerIndex << delimiter
				<< item.Barcode_OK << delimiter
				<< item.AA_OK << delimiter
				<< item.Capture_OK << delimiter
				<< item.Metrics_OK << delimiter
				<< "\"" << item.ModelName.toStdString() << "\"" << delimiter
				<< item.CheckEnabled << delimiter
				<< item.LiveRefresh;
				/*<< "\"" << item.name << "\"" << delimiter
				<< item.score << delimiter
				<< "\"" << item.timestamp << "\"";*/
		}
		//else {
		//    static_assert(false, "请为你的结构体特化序列化方法");
		//}
		return oss.str();
	}

	// 从 CSV 行反序列化结构体
	DutMeasureInfo deserializeFromCSV(const std::string& csv_line) {
		std::istringstream iss(csv_line);
		std::string token;
		std::vector<std::string> tokens;

		// 解析 CSV 行
		bool in_quotes = false;
		std::string current_token;

		for (char c : csv_line) {
			if (c == '"') {
				in_quotes = !in_quotes;
			}
			else if (c == delimiter && !in_quotes) {
				tokens.push_back(current_token);
				current_token.clear();
			}
			else {
				current_token += c;
			}
		}
		tokens.push_back(current_token);

		// 清理引号
		for (auto& token : tokens) {
			if (!token.empty() && token.front() == '"' && token.back() == '"') {
				token = token.substr(1, token.length() - 2);
			}
		}

		//if constexpr (std::is_same_v<T, DutMeasureInfo>) 
		{
			if (tokens.size() >= 15) {
				DutMeasureInfo data;
				data.SN = QString::fromStdString(tokens[0]);
				data.StartTime = QDateTime::fromString(QString::fromStdString(tokens[1]), "yyyy-MM-dd HH:mm:ss");
				data.EndTime = QDateTime::fromString(QString::fromStdString(tokens[2]), "yyyy-MM-dd HH:mm:ss");
				//data.ParentDir = QString::fromStdString(tokens[3]);
				data.DutDir = QString::fromStdString(tokens[3]);
				data.AdpResultDir = QString::fromStdString(tokens[4]);
				data.ErrorMsg = QString::fromStdString(tokens[5]);

				data.DutIndex = std::stoi(tokens[6]);
				data.LayerIndex = std::stoi(tokens[7]);
				data.Barcode_OK = std::stoi(tokens[8]);
				data.AA_OK = std::stoi(tokens[9]);
				data.Capture_OK = std::stoi(tokens[10]);
				data.Metrics_OK = std::stoi(tokens[11]);

				data.ModelName = QString::fromStdString(tokens[12]);
				data.CheckEnabled = std::stoi(tokens[13]);
				data.LiveRefresh = std::stoi(tokens[14]);

				/*data.id = std::stoi(tokens[0]);
				data.name = tokens[1];
				data.score = std::stod(tokens[2]);
				data.timestamp = tokens[3];*/
				return data;
			}
		}

		return DutMeasureInfo();// T();
	}

	// 保存整个 deque 到 CSV
	void saveToCSV() {
		std::ofstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("无法打开文件: " + filename);
		}

		// 添加 CSV 表头
		//if constexpr (std::is_same_v<T, DutMeasureInfo>) 
		{
			file << "SN,StartTime,EndTime,DutDir,AdpResultDir,ErrorMsg,DutIndex,TrayIndex,Barcode_OK,AA_OK,Capture_OK,Metrics_OK,ModelName,CheckEnab,LiveRefresh\n";
		}

		// 保存数据
		for (const auto& item : data_deque) {
			file << serializeToCSV(item) << "\n";
		}

		file.close();
	}

public:
	// 构造函数
	//CsvDequeRecorder(const std::string& file_path, char delim = ',')
	//	: filename(file_path), delimiter(delim) {
	//	
	//	try {

	//		if (!fs::exists(m_Dir))
	//		{
	//			fs::create_directories(m_Dir);
	//		}

	//		std::ofstream file(m_FilePath, std::ios::trunc);
	//		file.close();
	//	}
	//	catch (const std::exception& e) {
	//		std::cerr << "错误: " << e.what() << std::endl;
	//		return;
	//	}

	//	// 如果文件存在，自动加载
	//	if (fs::exists(filename)) {
	//		loadAll();
	//	}
	//}

	CsvDequeRecorder(char delim = ',')
		: delimiter(delim) {

		try {

			if (!fs::exists(m_Dir))
			{
				fs::create_directories(m_Dir);
			}

			//std::ofstream file(filename, std::ios::trunc);
			//file.close();
		}
		catch (const std::exception& e) {
			std::cerr << "错误: " << e.what() << std::endl;
			return;
		}

		// 如果文件存在，自动加载
		if (fs::exists(filename)) {
			loadAll();
		}
	}

	static CsvDequeRecorder* GetInstance(void)
	{
		static CsvDequeRecorder self;
		return &self;
	}

	// 从 CSV 加载所有数据
	void loadAll() {
		std::unique_lock<std::mutex> lock(mtx_);

		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "警告: 无法打开文件 " << filename << "，将创建新文件" << std::endl;
			return;
		}

		data_deque.clear();
		std::string line;

		// 跳过表头
		std::getline(file, line);

		// 读取数据
		while (std::getline(file, line)) {
			if (!line.empty()) {
				try {
					DutMeasureInfo item = deserializeFromCSV(line);
					data_deque.push_back(item);
				}
				catch (const std::exception& e) {
					std::cerr << "警告: 解析行失败: " << line << " - " << e.what() << std::endl;
				}
			}
		}

		file.close();
	}

	// 在队尾添加数据
	bool push_back(const DutMeasureInfo& item) {
		std::unique_lock<std::mutex> lock(mtx_);

		auto it = std::find(data_deque.begin(), data_deque.end(), item);

		if (it == data_deque.end()) {
			data_deque.push_back(item);
			appendToCSV(item);
		}
		else {
			std::cout << "发现重复项！DutDir " << item.DutDir.toStdString() << " 已存在，插入操作已跳过。\n";
			return false;
		}

		return true;
	}

	// 在队头添加数据
	bool push_front(const DutMeasureInfo& item) {
		std::unique_lock<std::mutex> lock(mtx_);

		auto it = std::find(data_deque.begin(), data_deque.end(), item);

		if (it == data_deque.end()) {
			data_deque.push_front(item);
			saveToCSV();  // 需要在文件开头插入，所以重新保存整个文件
		}
		else {
			std::cout << "发现重复项！DutDir " << item.DutDir.toStdString() << " 已存在，插入操作已跳过。\n";
			return false;
		}

		return true;
	}

	// 删除队尾数据
	std::optional<std::reference_wrapper<DutMeasureInfo>> pop_back() {
		std::unique_lock<std::mutex> lock(mtx_);
		if (!data_deque.empty()) {
			data_deque.pop_back();
			saveToCSV();
		}
	}

	// 删除队头数据
	std::optional<DutMeasureInfo> pop_front() {
		std::unique_lock<std::mutex> lock(mtx_);
		if (data_deque.empty()) {
			return std::nullopt;
		}

		DutMeasureInfo value = data_deque.front();
		data_deque.pop_front();
		saveToCSV();

		/*if (!data_deque.empty()) {
			data_deque.pop_front();
			saveToCSV();
		}*/
		return std::ref(value);
	}

	std::optional<std::reference_wrapper<DutMeasureInfo>> query_front()
	{
		if (data_deque.empty()) {
			return std::nullopt;
		}
		return std::ref(data_deque.front());
	}

	std::optional<std::reference_wrapper<DutMeasureInfo>> query_back()
	{
		if (data_deque.empty()) {
			return std::nullopt;
		}
		return std::ref(data_deque.back());
	}
	//// 在指定位置插入数据
	//void insert(size_t pos, const T& item) {
	//    if (pos <= data_deque.size()) {
	//        auto it = data_deque.begin();
	//        std::advance(it, pos);
	//        data_deque.insert(it, item);
	//        saveToCSV();
	//    }
	//}

	//// 删除指定位置的数据
	//void erase(size_t pos) {
	//    if (pos < data_deque.size()) {
	//        auto it = data_deque.begin();
	//        std::advance(it, pos);
	//        data_deque.erase(it);
	//        saveToCSV();
	//    }
	//}

	// 清空所有数据
	void clear() {
		data_deque.clear();
		saveToCSV();
	}

	// 追加数据到 CSV 文件（不重新写入整个文件）
	void appendToCSV(const DutMeasureInfo& item) {
		std::ofstream file(filename, std::ios::app);
		if (!file.is_open()) {
			throw std::runtime_error("无法打开文件: " + filename);
		}

		// 如果是新文件，添加表头
		if (fs::file_size(filename) == 0)// && std::is_same_v<T, DutMeasureInfo>) {
		{
			file << "SN,StartTime,EndTime,DutDir,AdpResultDir,ErrorMsg,DutIndex,TrayIndex,Barcode_OK,AA_OK,Capture_OK,Metrics_OK,ModelName,CheckEnab,LiveRefresh\n";
		}

		file << serializeToCSV(item) << "\n";
		file.close();
	}

	// 获取 deque 引用
	std::deque<DutMeasureInfo>& getDeque() {
		return data_deque;
	}

	// 获取 const deque 引用
	const std::deque<DutMeasureInfo>& getDeque() const {
		return data_deque;
	}

	// 获取数据数量
	size_t size() const {
		return data_deque.size();
	}

	// 检查是否为空
	bool empty() const {
		return data_deque.empty();
	}

	// 手动保存到 CSV
	void save() {
		saveToCSV();
	}

	// 设置新的文件名
	void setFilename(const std::string& new_filename) {
		filename = new_filename;
	}

	// 获取当前文件名
	std::string getFilename() const {
		return filename;
	}

	// 打印所有数据
	//void printAll() const {
	//    std::cout << "数据记录 (" << data_deque.size() << " 条):\n";
	//    for (const auto& item : data_deque) {
	//        if constexpr (std::is_same_v<T, DutMeasureInfo>) {
	//            std::cout << "ID: " << item.id
	//                << ", Name: " << item.name
	//                << ", Score: " << item.score
	//                << ", Time: " << item.timestamp
	//                << std::endl;
	//        }
	//    }
	//}
};

class AdpDequeueManager : public QObject
{
	//Q_OBJECT

public:

	explicit AdpDequeueManager(QObject* parent = nullptr) : QObject(parent) {
		Initialize();
	}

	static AdpDequeueManager* GetInstance(void)
	{
		static AdpDequeueManager self;
		return &self;
	}

	void Initialize()
	{
		//try {

		//	if (!fs::exists(m_Dir.toStdString()))
		//	{
		//		fs::create_directories(m_Dir.toStdString());
		//	}

		//	std::ofstream file(m_FilePath.toStdString(), std::ios::trunc);
		//	file.close();

		//	// 创建 CSV 记录器
		//	m_CsvDequeRecorder = new CsvDequeRecorder(filename.toStdString());
		//	//CsvDequeRecorder recorder(m_FilePath.toStdString());
		//}
		//catch (const std::exception& e) {
		//	std::cerr << "错误: " << e.what() << std::endl;
		//	return;
		//}
	}

	void Test()
	{
		try {

			//QString dir = QString(".\\config\\AdpCsv");

			//if (!fs::exists(dir.toStdString()))
			//{
			//	fs::create_directories(dir.toStdString());
			//}

			//QString filePath = QString(".\\config\\AdpCsv\\AdpCsv.csv");

			//std::ofstream file(filePath.toStdString(), std::ios::trunc);
			//file.close();

			//recorder.loadAll();

			CsvDequeRecorder::GetInstance()->clear();

			DutMeasureInfo _dut1;
			_dut1.StartTime = QDateTime::currentDateTime();
			_dut1.DutIndex = 1;
			_dut1.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut1.DutDir = "E:\\DutTestResult\\MetricsTest1";
			_dut1.SN = "111";
			_dut1.ModelName = "AutoDP_WG";
			_dut1.CheckEnabled = true;
			_dut1.LiveRefresh = true;
			_dut1.ErrorMsg = "";

			DutMeasureInfo _dut2;
			_dut2.StartTime = QDateTime::currentDateTime();
			_dut2.DutIndex = 2;
			_dut2.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut2.DutDir = "E:\\DutTestResult\\MetricsTest2";
			_dut2.SN = "222";
			_dut2.ModelName = "AutoDP_WG";
			_dut2.CheckEnabled = true;
			_dut2.LiveRefresh = true;
			_dut2.ErrorMsg = "222_ERR";

			DutMeasureInfo _dut3;
			_dut3.StartTime = QDateTime::currentDateTime();
			_dut3.DutIndex = 3;
			_dut3.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut3.DutDir = "E:\\DutTestResult\\MetricsTest3";
			_dut3.SN = "333";
			_dut3.CheckEnabled = true;
			_dut3.ModelName = "AutoDP_WG";
			_dut3.LiveRefresh = true;
			_dut3.ErrorMsg = "";

			DutMeasureInfo _dut4;
			_dut4.StartTime = QDateTime::currentDateTime();
			_dut4.DutIndex = 4;
			_dut4.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut4.DutDir = "E:\\DutTestResult\\MetricsTest4";
			_dut4.SN = "444";
			_dut4.CheckEnabled = true;
			_dut4.ModelName = "AutoDP_WG";
			_dut4.LiveRefresh = true;
			_dut4.ErrorMsg = "444_ERR";

			MetricsData::instance()->pushDutMetricsQueue(_dut1);
			MetricsData::instance()->pushDutMetricsQueue(_dut2);
			MetricsData::instance()->pushDutMetricsQueue(_dut3);
			MetricsData::instance()->pushDutMetricsQueue(_dut4);

			// 添加数据
			//CsvDequeRecorder::GetInstance()->push_back(_dut1);
			//CsvDequeRecorder::GetInstance()->push_back(_dut2);
			//CsvDequeRecorder::GetInstance()->push_back(_dut3);

			// 打印数据
			//recorder.printAll();
			//std::cout << "保存到: " << CsvDequeRecorder::GetInstance()->getFilename() << std::endl;

			// 删除数据
			//CsvDequeRecorder::GetInstance()->pop_front();

			//// 修改数据
			//auto& deque = recorder.getDeque();
			//if (!deque.empty()) {
			//    deque[0].score = 99.0;  // 修改第一个元素的分数
			//    recorder.save();  // 保存修改
			//}

			// 清空并重新加载
			//CsvDequeRecorder::GetInstance()->clear();
			//std::cout << "\n清空后数据量: " << CsvDequeRecorder::GetInstance()->size() << std::endl;

			//CsvDequeRecorder::GetInstance()->loadAll();
			//std::cout << "重新加载后数据量: " << CsvDequeRecorder::GetInstance()->size() << std::endl;

			// 再次添加新数据
			//CsvDequeRecorder::GetInstance()->push_back(_dut4);

		}
		catch (const std::exception& e) {
			std::cerr << "错误: " << e.what() << std::endl;
			return;
		}
	}

	void Test2()
	{
		try {

			//QString dir = QString(".\\config\\AdpCsv");

			//if (!fs::exists(dir.toStdString()))
			//{
			//	fs::create_directories(dir.toStdString());
			//}

			//QString filePath = QString(".\\config\\AdpCsv\\AdpCsv.csv");

			//std::ofstream file(filePath.toStdString(), std::ios::trunc);
			//file.close();

			//recorder.loadAll();

			CsvDequeRecorder::GetInstance()->clear();

			DutMeasureInfo _dut1;
			_dut1.StartTime = QDateTime::currentDateTime();
			_dut1.DutIndex = 1;
			_dut1.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut1.DutDir = "E:\\DutTestResult\\MetricsTest1";
			_dut1.SN = "111";
			_dut1.ModelName = "AutoDP_WG";
			_dut1.CheckEnabled = true;
			_dut1.LiveRefresh = true;
			_dut1.ErrorMsg = "";

			DutMeasureInfo _dut2;
			_dut2.StartTime = QDateTime::currentDateTime();
			_dut2.DutIndex = 2;
			_dut2.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut2.DutDir = "E:\\DutTestResult\\MetricsTest2";
			_dut2.SN = "222";
			_dut2.ModelName = "AutoDP_WG";
			_dut2.CheckEnabled = true;
			_dut2.LiveRefresh = true;
			_dut2.ErrorMsg = "222_ERR";

			DutMeasureInfo _dut3;
			_dut3.StartTime = QDateTime::currentDateTime();
			_dut3.DutIndex = 3;
			_dut3.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut3.DutDir = "E:\\DutTestResult\\MetricsTest3";
			_dut3.SN = "333";
			_dut3.CheckEnabled = true;
			_dut3.ModelName = "AutoDP_WG";
			_dut3.LiveRefresh = true;
			_dut3.ErrorMsg = "";

			DutMeasureInfo _dut4;
			_dut4.StartTime = QDateTime::currentDateTime();
			_dut4.DutIndex = 4;
			_dut4.ParentDir = "E:\\DutTestResult\\ResultFolder";
			_dut4.DutDir = "E:\\DutTestResult\\MetricsTest4";
			_dut4.SN = "444";
			_dut4.CheckEnabled = true;
			_dut4.ModelName = "AutoDP_WG";
			_dut4.LiveRefresh = true;
			_dut4.ErrorMsg = "444_ERR";

			MetricsData::instance()->pushDutAdpHistoryQueue(_dut1);
			MetricsData::instance()->pushDutAdpHistoryQueue(_dut2);
			MetricsData::instance()->pushDutAdpHistoryQueue(_dut3);
			MetricsData::instance()->pushDutAdpHistoryQueue(_dut4);

			// 添加数据
			//CsvDequeRecorder::GetInstance()->push_back(_dut1);
			//CsvDequeRecorder::GetInstance()->push_back(_dut2);
			//CsvDequeRecorder::GetInstance()->push_back(_dut3);

			// 打印数据
			//recorder.printAll();
			//std::cout << "保存到: " << CsvDequeRecorder::GetInstance()->getFilename() << std::endl;

			// 删除数据
			//CsvDequeRecorder::GetInstance()->pop_front();

			//// 修改数据
			//auto& deque = recorder.getDeque();
			//if (!deque.empty()) {
			//    deque[0].score = 99.0;  // 修改第一个元素的分数
			//    recorder.save();  // 保存修改
			//}

			// 清空并重新加载
			//CsvDequeRecorder::GetInstance()->clear();
			//std::cout << "\n清空后数据量: " << CsvDequeRecorder::GetInstance()->size() << std::endl;

			//CsvDequeRecorder::GetInstance()->loadAll();
			//std::cout << "重新加载后数据量: " << CsvDequeRecorder::GetInstance()->size() << std::endl;

			// 再次添加新数据
			//CsvDequeRecorder::GetInstance()->push_back(_dut4);

		}
		catch (const std::exception& e) {
			std::cerr << "错误: " << e.what() << std::endl;
			return;
		}
	}

//private:
//	const QString m_Dir = QString(".\\config\\AdpCsv");
//	const QString m_FilePath = QString(".\\config\\AdpCsv\\AdpCsv.csv");
//	CsvDequeRecorder* m_CsvDequeRecorder = nullptr;

};
