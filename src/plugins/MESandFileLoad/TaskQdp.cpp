#include "TaskQdp.h"
#include "MesMode.h"
#include "MesMode.h"

TaskQdp::TaskQdp(const QdpCsvHeader& data, const SummaryCsvHeader& summaryData, const std::string& sourcePath, const std::string& rsultPath):
    m_qdpCsvHeader(data),
    m_summaryCsvHeader(summaryData),
    m_sourcePath(sourcePath),
    m_resultPath(rsultPath)
{

}

Result TaskQdp::execute()
{
    int startTime = QDateTime::currentMSecsSinceEpoch();
    qWarning() << "### QDP ###: start AUtoDP.";

    ProcessingTask proTask;
    proTask.taskId = 1;

    std::string sourceImagePath = m_sourcePath;

    while (!sourceImagePath.empty() &&
        (sourceImagePath.back() == '\\' || sourceImagePath.back() == '/')) {
        sourceImagePath.pop_back();
    }

    m_sourcePath = sourceImagePath;
    proTask.sourceImageDir = sourceImagePath;
    proTask.resultSaveDir = m_resultPath;
    proTask.submitTime = std::chrono::system_clock::now();
    bool flag = ImageProcessor().processTask(proTask);
    if(!flag){
        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
        return Result(false, "AutoDP run error.");
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << QString("### QDP ###: end AUtoDP. time is %1 ms...").arg(takeTime);

#if 0

    startTime = QDateTime::currentMSecsSinceEpoch();
    qWarning() << QString("### QDP ###: start Compression file, source dir is %1, result dir is %2.")
        .arg(QString::fromStdString(proTask.sourceImageDir))
        .arg(QString::fromStdString(proTask.resultSaveDir));

    SimpleBandizipCompressor::ZipVolumeInfo zipVolumeInfo;
    QString resultDir;
    QString autoDPResult;
    Result ret = zipDir(proTask, autoDPResult, resultDir, zipVolumeInfo);
    if(!ret.success){
        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
        return Result(false, "Compression file error, " + ret.errorMsg);
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << QString("### QDP ###: end Compression file, time is %1 ms, source dir is %2, result dir is %3.")
        .arg(takeTime)
        .arg(QString::fromStdString(proTask.sourceImageDir))
        .arg(QString::fromStdString(proTask.resultSaveDir));

    startTime = QDateTime::currentMSecsSinceEpoch();
    qWarning() << "### QDP ###: update AutoDP csv start, " << resultDir;

    m_qdpCsvHeader.zip_files.clear();
    for (std::string file : zipVolumeInfo.volumeFiles) {
        m_qdpCsvHeader.zip_files.append(QString::fromStdString(fs::path(file).filename().string()));
    }
    QString newFilePathCSV;
    QList<QStringList> initData;
    ret = updateCsv(autoDPResult, m_qdpCsvHeader, newFilePathCSV, initData);
    if (!ret.success) {
        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
        return Result(false, "Update AutoDP csv error, " + ret.errorMsg);
    }

    QString summaryFilePathCSV;
    updateSummaryCsv(autoDPResult, m_summaryCsvHeader, summaryFilePathCSV, initData);

    if (!ret.success) {
        MetricsData::instance()->updateDutAutoDPResult("Metrics_ERR");
        return Result(false, "Update AutoDP csv error, " + ret.errorMsg);
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    qWarning() << QString("### QDP ###: update AutoDP csv end. time is %1 ms, %2").arg(takeTime).arg(resultDir);

    //TODO: tcp
    //ExternalTOSPathToRDbody dataRD;
    //dataRD.lensId = m_qdpCsvHeader.serial_number.toStdString();
    //dataRD.filePath = zipVolumeInfo.volumeFiles;
    //ret = MesMode::instance().sendTcpRD(dataRD);
    //if (!ret.success) {
    //    return ret;
    //}

    ExternalTOSPathbody data;
    data.csvPath = newFilePathCSV.toStdString();
    data.zipPath = zipVolumeInfo.volumeFiles;
    ret = MesMode::instance().sendTcp(data);
    if (!ret.success) {
        MetricsData::instance()->updateDutAutoDPResult("MES_ERR");
        return Result(false, "Upload data ZIP error, " + ret.errorMsg);
    }


    //bool pass = false;

    //if (!ret.success) {
    //    //qCritical() << "QDFData updateCsv error, " << QString::fromStdString(ret.errorMsg);
    //    MetricsData::instance()->updateDutAutoDPResult(false);
    //    continue;
    //}

    //pass = (metricsResult.result == 1);
    //MetricsData::instance()->updateDutAutoDPResult(pass);

#endif // 0

    return Result();
}

QString TaskQdp::taskInfo()
{
    return QString("QDP Task");
}

void TaskQdp::setStop(std::shared_ptr<std::atomic<bool>> stopFlag)
{

}

void TaskQdp::onSuccess()
{
}

void TaskQdp::onFailure()
{
}

Result TaskQdp::zipDir(ProcessingTask task, QString& autoDPResult, QString& resultDir, SimpleBandizipCompressor::ZipVolumeInfo &zipVolumeInfo)
{

    std::string sourceFolderName = fs::path(task.sourceImageDir).filename().string();

    std::string tempDir = ImageProcessor().getTempDirPath(task.sourceImageDir);

    std::string finalResultDir = task.resultSaveDir + "\\" + sourceFolderName;

    std::string autoDPDir;

    resultDir = QString::fromStdString(finalResultDir);

    if (ImageProcessor().moveDirectory(tempDir, finalResultDir)) {
        std::cout << "Task " << task.taskId << " completed successfully. Results saved to: "
            << finalResultDir << std::endl;

        SimpleBandizipCompressor::CompressionConfig config;
        SimpleBandizipCompressor::CompressionResult result;

        autoDPDir = moveCSVFilesFromPyAutoDPFolders(finalResultDir);
        autoDPResult = QString::fromStdString(autoDPDir);
        std::string autoDPFolderName = fs::path(autoDPDir).filename().string();

        if (fs::is_directory(autoDPDir)) {
            result = SimpleBandizipCompressor::instance()->CompressDirectory(autoDPDir, autoDPDir, config);
        }
        else {
            result = SimpleBandizipCompressor::instance()->CompressFile(autoDPDir, autoDPDir, config);
        }

        SimpleBandizipCompressor::ZipVolumeInfo autoDPZipDir = SimpleBandizipCompressor::instance()->ScanZipVolumeFiles(task.resultSaveDir, autoDPFolderName);
        zipVolumeInfo.volumeFiles.insert(zipVolumeInfo.volumeFiles.end(), autoDPZipDir.volumeFiles.begin(), autoDPZipDir.volumeFiles.end());

        if (fs::is_directory(finalResultDir)) {
            result = SimpleBandizipCompressor::instance()->CompressDirectory(finalResultDir, finalResultDir, config);
        }
        else {
            result = SimpleBandizipCompressor::instance()->CompressFile(finalResultDir, finalResultDir, config);
        }

        SimpleBandizipCompressor::ZipVolumeInfo srcZipDir = SimpleBandizipCompressor::instance()->ScanZipVolumeFiles(task.resultSaveDir, sourceFolderName);
        zipVolumeInfo.volumeFiles.insert(zipVolumeInfo.volumeFiles.end(), srcZipDir.volumeFiles.begin(), srcZipDir.volumeFiles.end());

    }
    return Result();
}

Result TaskQdp::updateCsv(const QString& finalResultDir, QdpCsvHeader qdpCsvHeader, QString &newFilePath, QList<QStringList>& initData)
{
    QString filePathCsv;
    Result ret = QDFData().getAutoDpCsv(filePathCsv, finalResultDir);
    if (!ret.success) {
        return ret;
    }

    McMetricsResult metricsResult;
    ret = QDFData().updateCsv(filePathCsv, newFilePath, qdpCsvHeader, initData, metricsResult);
    if (!ret.success) {
        return ret;
    }

    ret = MesMode::instance().singleLensUnload(metricsResult.result == METRICS_PASS);
    return ret;
}

Result TaskQdp::updateSummaryCsv(const QString& filePath, SummaryCsvHeader summaryCsvHeader, QString& newFilePath, QList<QStringList>& initData)
{
    Result ret = QDFData().updateSummaryCsv(filePath, newFilePath, summaryCsvHeader, initData);
    if (!ret.success) {
        return ret;
    }
    return Result();
}

bool TaskQdp::copyDirectory(const QString& sourcePath, const QString& destinationPath)
{
    QDir sourceDir(sourcePath);
    if (!sourceDir.exists())
        return false;

    QDir destDir(destinationPath);
    if (!destDir.exists())
        destDir.mkpath(destinationPath);

    QStringList files = sourceDir.entryList(QDir::Files);
    for (const QString& file : files) {
        QString srcName = sourcePath + "/" + file;
        QString destName = destinationPath + "/" + file;
        if (!QFile::copy(srcName, destName))
            return false;
    }

    QStringList dirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (const QString& dir : dirs) {
        QString srcName = sourcePath + "/" + dir;
        QString destName = destinationPath + "/" + dir;
        if (!copyDirectory(srcName, destName))
            return false;
    }

    return true;
}

std::string TaskQdp::moveCSVFilesFromPyAutoDPFolders(const std::string& finalResultDir)
{
    std::string newFolderPath = "";

    try {
        QString qFinalResultDir = QString::fromStdString(finalResultDir);
        QDir sourceDir(qFinalResultDir);

        if (!sourceDir.exists()) {
            std::cerr << "Source directory does not exist: " << finalResultDir << std::endl;
            return newFolderPath;
        }

        QDir parentDir = sourceDir;
        parentDir.cdUp();
        QString targetDirPath = parentDir.absolutePath() + "/" + sourceDir.dirName() + "_AutoDP";
        newFolderPath = targetDirPath.toStdString();

        QDir targetDir(targetDirPath);
        if (!targetDir.exists()) {
            if (!targetDir.mkpath(".")) {
                std::cerr << "Failed to create directory: " << targetDirPath.toStdString() << std::endl;
                newFolderPath = "";
                return newFolderPath;
            }
            std::cout << "creat dir: " << targetDirPath.toStdString() << std::endl;
        }

        QString baseName = sourceDir.dirName();

        QString baseNameWithoutTimestamp = baseName;
        QRegularExpression timestampRegex("_\\d{8}T\\d{6}$");
        baseNameWithoutTimestamp.remove(timestampRegex);

        std::cout << "Base name for matching: " << baseNameWithoutTimestamp.toStdString() << std::endl;

        std::vector<QString> foldersToMove;
        std::vector<QString> filesToMove;

        QFileInfoList entries = sourceDir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        for (const QFileInfo& entry : entries) {
            if (entry.isDir()) {
                QString folderName = entry.fileName();

                if (folderName.startsWith("pyAutoDP")) {
                    foldersToMove.push_back(entry.absoluteFilePath());
                    std::cout << "Found pyAutoDP folder: " << folderName.toStdString() << std::endl;
                }
            }
            else if (entry.isFile()) {
                QString filename = entry.fileName();

                if (filename.endsWith(".csv") && filename.contains(baseNameWithoutTimestamp)) {
                    filesToMove.push_back(entry.absoluteFilePath());
                    std::cout << "Found matching CSV file: " << filename.toStdString() << std::endl;
                }
            }
        }

        int movedCount = 0;

        for (const QString& sourceFolder : foldersToMove) {
            QFileInfo folderInfo(sourceFolder);
            QString targetFolder = targetDirPath + "/" + folderInfo.fileName();

            try {
                QDir sourceDirObj(sourceFolder);
                if (QDir(targetFolder).exists()) {
                    if (!QDir(targetFolder).removeRecursively()) {
                        std::cerr << "Failed to remove existing folder: " << targetFolder.toStdString() << std::endl;
                        continue;
                    }
                }

                if (sourceDirObj.rename(sourceFolder, targetFolder)) {
                    std::cout << "Moved folder: " << folderInfo.fileName().toStdString()
                        << " -> " << targetDir.dirName().toStdString() << std::endl;
                    movedCount++;
                }
                else {
                    std::cerr << "Failed to move folder: " << sourceFolder.toStdString() << std::endl;

                    if (copyDirectory(sourceFolder, targetFolder) && QDir(sourceFolder).removeRecursively()) {
                        std::cout << "Moved folder (via copy+delete): " << folderInfo.fileName().toStdString()
                            << " -> " << targetDir.dirName().toStdString() << std::endl;
                        movedCount++;
                    }
                    else {
                        std::cerr << "Failed to move folder even with copy+delete: " << sourceFolder.toStdString() << std::endl;
                        newFolderPath = "";
                    }
                }
            }
            catch (const std::exception& ex) {
                std::cerr << "Exception moving folder: " << sourceFolder.toStdString()
                    << " - " << ex.what() << std::endl;
                newFolderPath = "";
            }
        }

        for (const QString& sourceFile : filesToMove) {
            QFileInfo fileInfo(sourceFile);
            QString targetFile = targetDirPath + "/" + fileInfo.fileName();

            try {
                QFile file(sourceFile);

                if (QFile::exists(targetFile)) {
                    if (!QFile::remove(targetFile)) {
                        std::cerr << "Failed to remove existing file: " << targetFile.toStdString() << std::endl;
                        continue;
                    }
                }

                if (file.rename(targetFile)) {
                    std::cout << "Moved file: " << fileInfo.fileName().toStdString()
                        << " -> " << targetDir.dirName().toStdString() << std::endl;
                    movedCount++;
                }
                else {
                    std::cerr << "Failed to move file: " << sourceFile.toStdString()
                        << " - " << file.errorString().toStdString() << std::endl;

                    if (QFile::copy(sourceFile, targetFile) && QFile::remove(sourceFile)) {
                        std::cout << "Moved file (via copy+delete): " << fileInfo.fileName().toStdString()
                            << " -> " << targetDir.dirName().toStdString() << std::endl;
                        movedCount++;
                    }
                    else {
                        std::cerr << "Failed to move file even with copy+delete: " << sourceFile.toStdString() << std::endl;
                        newFolderPath = "";
                    }
                }
            }
            catch (const std::exception& ex) {
                std::cerr << "Exception moving file: " << sourceFile.toStdString()
                    << " - " << ex.what() << std::endl;
                newFolderPath = "";
            }
        }

        std::cout << "Total moved " << movedCount << " items to " << targetDirPath.toStdString() << std::endl;

    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        newFolderPath = "";
    }

    return newFolderPath;
}
