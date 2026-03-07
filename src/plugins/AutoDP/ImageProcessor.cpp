#define _HAS_STD_BYTE 0
#include "ImageProcessor.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

#include "SimpleIni.h"

namespace fs = std::filesystem;

ImageProcessor& ImageProcessor::instance()
{
    static ImageProcessor self;
    return self;
}

ImageProcessor::ImageProcessor()
    : m_isRunning(false)
    , m_taskCounter(0) {
    loadINIConfig();
    UpdateINIConfig();
}

ImageProcessor::~ImageProcessor() {
    stopService();
}

result ImageProcessor::setConfig(const std::string& pyAutoDPPath,
    const std::string& testType,
    const std::string& projectName,
    const std::string& configDir,
    const std::string& tempDir,
    const std::string& exceptionDir) {

    result rt;

    m_pyAutoDPPath = pyAutoDPPath + "pyAutoDP.exe";
    m_autoDPBasePath = pyAutoDPPath;
    m_testType = testType;
    m_projectName = projectName;
    m_configDir = configDir;
    m_tempBaseDir = tempDir;
    m_exceptionDir = exceptionDir;

    if (!fs::exists(m_tempBaseDir)) {
        fs::create_directories(m_tempBaseDir);
    }

    if (!fs::exists(m_exceptionDir)) {
        fs::create_directories(m_exceptionDir);
    }

    if (!checkPyAutoDPExists()) {
        std::cerr << "Warning: pyAutoDP.exe not found at specified path: " << m_pyAutoDPPath << std::endl;
    }

    UpdateINIConfig();

    return rt;
}

void ImageProcessor::loadINIConfig()
{
    CSimpleIniA ini;

    SI_Error rc = ini.LoadFile(m_AutoDPConfig.c_str());
    if (rc < 0) {
        std::cerr << "can not load file: " << m_AutoDPConfig << std::endl;
    }

    const char* autoDPBasePath = ini.GetValue("AutoDP", "pyAutoDPPath", "");
    if (!autoDPBasePath || strlen(autoDPBasePath) == 0) {
        std::cerr << "lackof pyAutoDPPath setting" << std::endl;
    }

    m_autoDPBasePath = autoDPBasePath;
    m_pyAutoDPPath = m_autoDPBasePath + "pyAutoDP.exe";

    m_testType = ini.GetValue("AutoDP", "testType", "");
    m_projectName = ini.GetValue("AutoDP", "projectName", "");
    m_configDir = ini.GetValue("AutoDP", "configDir", "");
    m_tempBaseDir = ini.GetValue("AutoDP", "tempDir", "");
    m_exceptionDir = ini.GetValue("AutoDP", "exceptionDir", "");

    if (m_testType.empty() || m_projectName.empty()) {
        std::cerr << "lackof nuccessary sections" << std::endl;
    }

}

void ImageProcessor::UpdateINIConfig()
{
    std::string filename = m_autoDPBasePath + "Data_processing_" + m_testType + ".ini";

    CSimpleIniA ini;
    ini.LoadFile(filename.c_str());

    ini.SetValue("", "ref_src_dir", m_configDir.c_str());
    ini.SetValue("", "dat_src_dir", m_tempBaseDir.c_str());
    ini.SaveFile(filename.c_str());
}

bool ImageProcessor::checkPyAutoDPExists() const {
    return fs::exists(m_pyAutoDPPath);
}

std::string ImageProcessor::getTempDirPath(const std::string& sourceImageDir) const {

    std::string result = sourceImageDir;

    while (!result.empty() &&
        (result.back() == '\\' || result.back() == '/')) {
        result.pop_back();
    }

    std::string folderName = fs::path(result).filename().string();
    //return m_tempBaseDir + "\\" + folderName;
    return m_tempBaseDir + folderName;
}

std::string ImageProcessor::getExceptionDirPath(const std::string& sourceImageDir) const {

    std::string result = sourceImageDir;

    while (!result.empty() &&
        (result.back() == '\\' || result.back() == '/')) {
        result.pop_back();
    }

    std::string folderName = fs::path(result).filename().string();
    //return m_tempBaseDir + "\\" + folderName;
    return m_exceptionDir + folderName;
}

result ImageProcessor::startService() {
    result rt;
    if (m_isRunning) {
        std::cout << "Service is already running!" << std::endl; 
        rt.errorMessage = "Service is already running!";
        rt.success = false;
        return rt;
    }

    if (!checkPyAutoDPExists()) {
        std::cerr << "Cannot start service: pyAutoDP.exe not found at " << m_pyAutoDPPath << std::endl;
        rt.errorMessage = "Cannot start service: pyAutoDP.exe not found at " + m_pyAutoDPPath;
        rt.success = false;
        return rt;
    }

    m_isRunning = true;
    m_workerThread = std::thread(&ImageProcessor::workerThread, this);

    std::cout << "Image processing service started..." << std::endl;
    std::cout << "Using pyAutoDP: " << m_pyAutoDPPath << std::endl;
    rt.success = true;
    return rt;
}

void ImageProcessor::stopService() {
    m_isRunning = false;
    m_queueCondition.notify_all();
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

bool ImageProcessor::isRunning() const {
    return m_isRunning;
}

size_t ImageProcessor::getPendingTaskCount() const {
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_taskQueue.size();
}

result ImageProcessor::submitProcessingTask(const std::string& sourceImageDir,
    const std::string& resultSaveDir) {

    result rt;
    std::string sourceImagePath = sourceImageDir;

    while (!sourceImagePath.empty() &&
        (sourceImagePath.back() == '\\' || sourceImagePath.back() == '/')) {
        sourceImagePath.pop_back();
    }

    if (!fs::exists(sourceImagePath)) {
        std::cerr << "Source image directory does not exist: " << sourceImagePath << std::endl;
        rt.errorMessage = "Source image directory does not exist: " + sourceImagePath;
        rt.success = false;
        return rt;
    }

    if (!fs::exists(resultSaveDir)) {
        fs::create_directories(resultSaveDir);
    }

    ProcessingTask task;
    task.taskId = std::to_string(++m_taskCounter);
    task.sourceImageDir = sourceImagePath;
    task.resultSaveDir = resultSaveDir;
    task.submitTime = std::chrono::system_clock::now();

    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_taskQueue.push(task);
    }

    m_queueCondition.notify_one();

    std::cout << "Task " << task.taskId << " submitted. Source: " << sourceImageDir
        << " -> Result: " << resultSaveDir << std::endl;
    rt.success = true;
    return rt;
}

bool ImageProcessor::copyDirectory(const std::string& sourceDir, const std::string& destDir) {
    try {

        fs::create_directories(destDir);
      /*  if (!fs::exists(destDir)) {
            if (!fs::create_directories(destDir)) {
                std::cerr << "Failed to create destination directory: " << destDir << std::endl;
                return false;
            }
        }*/

        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            const auto& path = entry.path();
            auto relativePathStr = path.string().substr(sourceDir.length());
            auto targetPath = destDir + relativePathStr;

            if (entry.is_directory()) {
                fs::create_directories(targetPath);
            }
            else if (entry.is_regular_file()) {
                fs::copy_file(path, targetPath, fs::copy_options::overwrite_existing);
            }
        }

        std::cout << "Successfully copied directory from " << sourceDir << " to " << destDir << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error copying directory: " << e.what() << std::endl;
        return false;
    }
}

#include <shellapi.h>

bool ImageProcessor::processTask(const ProcessingTask& task) {
    std::cout << "Processing task " << task.taskId << "..." << std::endl;

    if (!fs::exists(task.sourceImageDir))
    {
        return false;
    }

    std::string tempDir =  getTempDirPath(task.sourceImageDir);

    std::string exceptionDir = getExceptionDirPath(task.sourceImageDir);

	if (tempDir.compare(task.sourceImageDir) != 0)
		cleanupTempDir(tempDir);

    //if (!copyDirectory(task.sourceImageDir, tempDir)) {
    //    std::cerr << "Failed to copy source directory to temp directory: " << tempDir << std::endl;
    //    return false;
    //}

    fs::path source(task.sourceImageDir);
    fs::path temp_destination(tempDir);
    fs::create_directories(temp_destination.parent_path());

    fs::path ex_destination(exceptionDir);
    fs::create_directories(ex_destination.parent_path());

    if (!temp_destination.parent_path().empty() && !fs::exists(temp_destination.parent_path())) {
        std::cerr << "错误：目标目录 '" << temp_destination.parent_path() << "' 不存在。" << std::endl;
        return false;
    }

    try {
        fs::rename(source, temp_destination);
        std::cout << "File moved successfully！" << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        std::cout << "fs::filesystem_error: " << e.what() << std::endl;
        std::cout << "source: " << e.path1() << std::endl;
        std::cout << "dst: " << e.path2() << std::endl;

        std::error_code ec = e.code();
        int ev = ec.value();

        if (e.code().value() == EPERM || e.code().value() == EBUSY) {
            std::cout << "The file or folder may be in use by another process" << std::endl;
        }

        return false;
    }
    catch (const std::exception& e) {
        std::cout << "unknow error: " << e.what() << std::endl;
        return false;
    }

    //1222 version
    std::string command = "\"" + m_pyAutoDPPath + "\" -c -t " + m_testType +
        " -p \"" + m_projectName + "\"" +
        " -d \"" + m_tempBaseDir  + "\"" +
        " -b \"" + m_configDir + "\"";

    //0119 version
    //std::string command = "\"" + m_pyAutoDPPath + "\" -c -t " + m_testType +
    //    " -p \"" + m_projectName + "\"" +
    //    " -d " + m_tempBaseDir + "" +
    //    " -b " + m_configDir + "";

    std::cout << "Executing: " << command << std::endl;
  
    std::string pyAutoDPDir = fs::path(m_pyAutoDPPath).parent_path().string();

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmd[1024];
    strcpy_s(cmd, command.c_str());

    if (!CreateProcessA(
        NULL,          
        cmd,           
        NULL,         
        NULL,          
        FALSE,         
        0,             
        NULL,           
        pyAutoDPDir.c_str(), 
        &si,           
        &pi)         
        ) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;

        //fs::rename(source, destination);
        //fs::rename(source, temp_destination);

        if (moveDirectory(temp_destination.string(), ex_destination.string())) {
            std::cout << "Task " << task.taskId << " move temp folder to exception folder successfully. Dut folder saved to: "
                << ex_destination.string() << std::endl;
        }

        //cleanupTempDir(tempDir);
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0) {
        std::cerr << "pyAutoDP process failed with exit code: " << exitCode << std::endl;
        if (moveDirectory(temp_destination.string(), ex_destination.string())) {
            std::cout << "Task " << task.taskId << " move temp folder to exception folder successfully. Dut folder saved to: "
                << ex_destination.string() << std::endl;
        }
        //cleanupTempDir(tempDir);
        return false;
    }

    std::cout << "pyAutoDP processing completed successfully for task " << task.taskId << std::endl;
    return true;
}

//void ImageProcessor::updateMetricsResult(QString errMsg)
//{
//    bool pass = (errMsg == "");
//    MetricsData::instance()->updateDutAutoDPResult(pass);
//}
//NO Windows
// 
//bool ImageProcessor::processTask(const ProcessingTask& task) {
//    std::cout << "Processing task " << task.taskId << "..." << std::endl;
//
//    std::string tempDir = getTempDirPath(task.sourceImageDir);
//    cleanupTempDir(tempDir);
//
//    if (!copyDirectory(task.sourceImageDir, tempDir)) {
//        std::cerr << "Failed to copy source directory to temp directory: " << tempDir << std::endl;
//        return false;
//    }
//
//    std::string parameters = "-c -t " + m_testType +
//        " -p \"" + m_projectName + "\"" +
//        " -d \"" + m_tempBaseDir + "\"" +
//        " -b \"" + m_configDir + "\"";
//
//    std::cout << "Executing: " << m_pyAutoDPPath << " " << parameters << std::endl;
//
//    std::string pyAutoDPDir = fs::path(m_pyAutoDPPath).parent_path().string();
//
//    SHELLEXECUTEINFOA sei = { 0 };
//    sei.cbSize = sizeof(sei);
//    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//    sei.lpFile = m_pyAutoDPPath.c_str();
//    sei.lpParameters = parameters.c_str();
//    sei.lpDirectory = pyAutoDPDir.c_str();
//    sei.nShow = SW_HIDE; 
//
//    if (!ShellExecuteExA(&sei)) {
//        DWORD error = GetLastError();
//        std::cerr << "ShellExecuteEx failed (" << error << ")." << std::endl;
//        cleanupTempDir(tempDir);
//        return false;
//    }
//
//    WaitForSingleObject(sei.hProcess, INFINITE);
//
//    DWORD exitCode;
//    GetExitCodeProcess(sei.hProcess, &exitCode);
//    CloseHandle(sei.hProcess);
//
//    if (exitCode != 0) {
//        std::cerr << "pyAutoDP process failed with exit code: " << exitCode << std::endl;
//        cleanupTempDir(tempDir);
//        return false;
//    }
//
//    std::cout << "pyAutoDP processing completed successfully for task " << task.taskId << std::endl;
//    return true;
//}

bool ImageProcessor::moveDirectory(const std::string& sourceDir, const std::string& destDir) {
    try {

        if (fs::exists(destDir)) {
            fs::remove_all(destDir);
        }

        fs::rename(sourceDir, destDir);

        std::cout << "Successfully moved directory from " << sourceDir << " to " << destDir << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error moving directory: " << e.what() << std::endl;

        try {
            if (copyDirectory(sourceDir, destDir)) {
                fs::remove_all(sourceDir);
                std::cout << "Used copy+delete method to move directory" << std::endl;
                return true;
            }
        }
        catch (const std::exception& e2) {
            std::cerr << "Alternative move method also failed: " << e2.what() << std::endl;
        }

        return false;
    }
}

void ImageProcessor::workerThread() {
    while (m_isRunning) {
        ProcessingTask task;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCondition.wait(lock, [this]() {
                return !m_isRunning || !m_taskQueue.empty();
                });

            if (!m_isRunning && m_taskQueue.empty()) {
                break;
            }

            if (!m_taskQueue.empty()) {
                task = m_taskQueue.front();
                m_taskQueue.pop();
            }
        }

        if (!task.taskId.empty()) {
            std::cout << "Start processing task " << task.taskId << std::endl;

            if (processTask(task)) {

                std::string sourceFolderName = fs::path(task.sourceImageDir).filename().string();

                std::string tempDir = getTempDirPath(task.sourceImageDir);

                std::string finalResultDir = task.resultSaveDir + "\\" + sourceFolderName;

                if (moveDirectory(tempDir, finalResultDir)) {
                    std::cout << "Task " << task.taskId << " completed successfully. Results saved to: "
                        << finalResultDir << std::endl;

                    SimpleBandizipCompressor::CompressionConfig config;
                    SimpleBandizipCompressor::CompressionResult result;

                    if (fs::is_directory(finalResultDir)) {
                        result = SimpleBandizipCompressor::instance()->CompressDirectory(finalResultDir, finalResultDir, config);
                    }
                    else {
                        result = SimpleBandizipCompressor::instance()->CompressFile(finalResultDir, finalResultDir, config);
                    }
                    SimpleBandizipCompressor::ZipVolumeInfo info = SimpleBandizipCompressor::instance()->ScanZipVolumeFiles(task.resultSaveDir, sourceFolderName);
                    QString filePathCsv;
                    Result ret = QDFData().getAutoDpCsv(filePathCsv, QString::fromStdString(finalResultDir));
                    if (!ret.success) {
                        //qCritical() << "QDFData getAutoDpCsv error, " << QString::fromStdString(ret.errorMsg);
                        continue;
                    }

                    //TODO: to be done
                    QdpCsvHeader qdpCsvHeader;
                    QString newFilePath;
                    QList<QStringList> initData;
                    McMetricsResult metricsResult;
                    ret = QDFData().updateCsv(filePathCsv, newFilePath, qdpCsvHeader, initData, metricsResult);
                    bool pass = false;
                    
                    if (!ret.success) {
                        //qCritical() << "QDFData updateCsv error, " << QString::fromStdString(ret.errorMsg);
                        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
                        continue;
                    }

                    pass = (metricsResult.result == 1);
                    MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");

                    emit taskCompleted("test", "test1", "test2");
                }
                else {
                    std::cerr << "Failed to move results for task " << task.taskId << std::endl;

                    cleanupTempDir(tempDir);
                }
            }
            else {
                std::cerr << "Task " << task.taskId << " failed during processing." << std::endl;

                cleanupTempDir(getTempDirPath(task.sourceImageDir));
            }
        }
    }

    std::cout << "Image processing worker thread stopped." << std::endl;
}

void ImageProcessor::cleanupTempDir(const std::string& tempDir) {
    try {
        if (fs::exists(tempDir)) {
            fs::remove_all(tempDir);
            std::cout << "Cleaned up temp directory: " << tempDir << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error cleaning up temp directory: " << e.what() << std::endl;
    }
}