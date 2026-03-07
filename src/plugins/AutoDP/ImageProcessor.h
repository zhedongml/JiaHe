#pragma once
#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <filesystem>
#include <chrono>
#include <QObject>
#include <QString>
#include "autodp_global.h"
#include "BandizipCompressor.h"
#include "QDFData.h"

struct AUTODP_EXPORT result
{
    bool success = true;
    std::string errorMessage;
};

struct AUTODP_EXPORT AutoDPConfig
{
    std::string pyAutoDPPath;
    std::string testType;
    std::string projectName;
    std::string configDir;
    std::string tempDir;
    std::string exceptionDir;
};

struct ProcessingTask {
    std::string taskId;
    std::string sourceImageDir;
    std::string resultSaveDir;
    std::chrono::system_clock::time_point submitTime;
};

class AUTODP_EXPORT ImageProcessor : public QObject {

    Q_OBJECT

public:
    static ImageProcessor& instance();
    ImageProcessor();
    ~ImageProcessor();

    result setConfig(const std::string& pyAutoDPPath,
        const std::string& testType,
        const std::string& projectName,
        const std::string& configDir,
        const std::string& tempDir,
        const std::string& exceptionDir);

    void loadINIConfig();

    void UpdateINIConfig();

    result submitProcessingTask(const std::string& sourceImageDir,
        const std::string& resultSaveDir);

    result startService();

    void stopService();

    bool isRunning() const;

    size_t getPendingTaskCount() const;

signals:
    void taskCompleted(const QString& fileName, const QString& fileBasePath, const QString& csvName);

public:

    bool copyDirectory(const std::string& sourceDir, const std::string& destDir);

    bool processTask(const ProcessingTask& task);

    bool moveDirectory(const std::string& sourceDir, const std::string& destDir);

    void cleanupTempDir(const std::string& tempDir);

    void workerThread();

    bool checkPyAutoDPExists() const;

    std::string getTempDirPath(const std::string& sourceImageDir) const;

    std::string getExceptionDirPath(const std::string& sourceImageDir) const;

    //void updateMetricsResult(QString errMsg);

    std::string m_pyAutoDPPath;
    std::string m_autoDPBasePath;
    std::string m_testType;
    std::string m_projectName;
    std::string m_configDir;
    std::string m_parentDir;
    std::string m_tempBaseDir;
    std::string m_exceptionDir;

    std::queue<ProcessingTask> m_taskQueue;
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;

    std::atomic<bool> m_isRunning;
    std::thread m_workerThread;
    std::atomic<size_t> m_taskCounter;

    const std::string m_AutoDPConfig = ".\\config\\AutoDPConfig.ini";
};