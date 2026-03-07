#include "BandizipCompressor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <windows.h>

SimpleBandizipCompressor* SimpleBandizipCompressor::self = nullptr;

SimpleBandizipCompressor::SimpleBandizipCompressor() {
    m_bandizipPath = GetBandizipExecutablePath();
}

SimpleBandizipCompressor::~SimpleBandizipCompressor() {
    delete self;
    self = nullptr;
}

SimpleBandizipCompressor* SimpleBandizipCompressor::instance()
{
    if (self == nullptr)
    {
        self = new SimpleBandizipCompressor();
    }
    return self;
}

bool SimpleBandizipCompressor::ExecuteBandizipCommand(const std::string& command, std::string& output) {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hStdoutRd, hStdoutWr;
    if (!CreatePipe(&hStdoutRd, &hStdoutWr, &saAttr, 0)) {
        return false;
    }
    SetHandleInformation(hStdoutRd, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOA));

    siStartInfo.cb = sizeof(STARTUPINFOA);
    siStartInfo.hStdError = hStdoutWr;
    siStartInfo.hStdOutput = hStdoutWr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    siStartInfo.dwFlags |= STARTF_USESHOWWINDOW; 
    siStartInfo.wShowWindow = SW_HIDE;            

    char* cmdLine = _strdup(command.c_str());

    BOOL success = CreateProcessA(
        NULL,
        cmdLine,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW, 
        NULL,
        NULL,
        &siStartInfo,
        &piProcInfo
    );

    free(cmdLine);
    CloseHandle(hStdoutWr);

    if (!success) {
        DWORD error = GetLastError();
        output = "CreateProcess failed with error: " + std::to_string(error);
        CloseHandle(hStdoutRd);
        return false;
    }

    DWORD dwRead;
    CHAR buffer[4096];
    BOOL bStillReading = TRUE;
    output.clear();

    while (bStillReading) {
        bStillReading = ReadFile(hStdoutRd, buffer, sizeof(buffer) - 1, &dwRead, NULL);
        if (bStillReading && dwRead > 0) {
            buffer[dwRead] = '\0';
            output += buffer;
        }
    }

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hStdoutRd);

    return exitCode == 0;
}

std::string SimpleBandizipCompressor::BuildBandizipCommand(const std::string& inputPath,
    const std::string& outputArchive,
    const CompressionConfig& config) {
    std::stringstream cmd;

    cmd << "\"" << m_bandizipPath << "\" c ";

    if (config.archiveFormat == "zip") {
        cmd << "-fmt:zip ";
    }
    else if (config.archiveFormat == "7z") {
        cmd << "-fmt:7z ";
    }

    if (config.volumeSizeMB > 0) {
        cmd << "-v:" << config.volumeSizeMB << "m ";
    }

    cmd << "-l:" << config.compressionLevel << " ";

    if (!config.password.empty()) {
        cmd << "-p:" << config.password << " ";
    }

    cmd << "-r ";

    cmd << "\"" << outputArchive << "\" ";

    cmd << "\"" << inputPath << "\"";

    std::string command = cmd.str();
    std::cout << "¹¹½¨µÄBandizipÃüÁî: " << command << std::endl;

    return command;
}

SimpleBandizipCompressor::CompressionResult SimpleBandizipCompressor::CompressFile(
    const std::string& inputFile,
    const std::string& outputArchive,
    const CompressionConfig& config) {

    CompressionResult result;

    if (!FileExists(inputFile)) {
        result.errorMessage = "The input file does not exist: " + inputFile;
        return result;
    }

    if (!IsBandizipAvailable()) {
        result.errorMessage = "Bandizip is not available. Please install Bandizip¡£";
        return result;
    }

    try {
    
        std::string command = BuildBandizipCommand(inputFile, outputArchive, config);

        std::string output;
        bool success = ExecuteBandizipCommand(command, output);


        if (!success) {
            result.errorMessage = "Bandizip command execution failed. output: " + output;
            return result;
        }

        FindVolumeFiles(outputArchive, result.volumeFiles);

        if (!result.volumeFiles.empty()) {
            uint64_t originalSize = GetFileSize(inputFile);

            for (const auto& file : result.volumeFiles) {
                result.totalCompressedSize += GetFileSize(file);
            }

            if (originalSize > 0) {
                result.compressionRatio = (1.0 - (double)result.totalCompressedSize / originalSize) * 100.0;
            }
        }

        result.success = true;

    }
    catch (const std::exception& e) {
        result.errorMessage = std::string("Abnormalities occurred during the compression process: ") + e.what();
    }

    return result;
}

SimpleBandizipCompressor::CompressionResult SimpleBandizipCompressor::CompressDirectory(
    const std::string& inputDir,
    const std::string& outputArchive,
    const CompressionConfig& config) {

    CompressionResult result;

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        result.errorMessage = "The input directory does not exist: " + inputDir;
        return result;
    }

    try {
  
        std::string command = BuildBandizipCommand(inputDir, outputArchive, config);

        std::string output;
        bool success = ExecuteBandizipCommand(command, output);


        if (!success) {
            result.errorMessage = "Bandizip command execution failed. output: " + output;
            return result;
        }

        FindVolumeFiles(outputArchive, result.volumeFiles);

        if (!result.volumeFiles.empty()) {
            uint64_t originalSize = 0;
            for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
                if (entry.is_regular_file()) {
                    originalSize += entry.file_size();
                }
            }

            for (const auto& file : result.volumeFiles) {
                result.totalCompressedSize += GetFileSize(file);
            }

            if (originalSize > 0) {
                result.compressionRatio = (1.0 - (double)result.totalCompressedSize / originalSize) * 100.0;
            }
        }

        result.success = true;

    }
    catch (const std::exception& e) {
        result.errorMessage = std::string("Abnormalities occurred during the compression process: ") + e.what();
    }

    return result;
}

void SimpleBandizipCompressor::FindVolumeFiles(const std::string& baseArchiveName, std::vector<std::string>& volumeFiles) {
    volumeFiles.clear();

    std::string basePath = baseArchiveName;
    size_t dotPos = basePath.find_last_of('.');
    std::string baseName, extension;

    if (dotPos != std::string::npos) {
        baseName = basePath.substr(0, dotPos);
        extension = basePath.substr(dotPos + 1);
    }
    else {
        baseName = basePath;
        extension = "";
    }

    if (FileExists(baseArchiveName)) {
        volumeFiles.push_back(baseArchiveName);
    }

    if (extension == "zip" || baseArchiveName.find(".zip") != std::string::npos) {
        std::string mainZipFile = baseName + ".zip";
        if (FileExists(mainZipFile) && std::find(volumeFiles.begin(), volumeFiles.end(), mainZipFile) == volumeFiles.end()) {
            volumeFiles.push_back(mainZipFile);
        }

        for (int i = 1; i <= 99; i++) {
            std::string volumeFile = baseName + ".z" + (i < 10 ? "0" : "") + std::to_string(i);
            if (FileExists(volumeFile)) {
                volumeFiles.push_back(volumeFile);
            }
            else {
                break;
            }
        }
    }

    if (volumeFiles.empty() && (extension == "7z" || baseArchiveName.find(".7z.") != std::string::npos)) {
        for (int i = 1; i <= 999; i++) {
            std::string volumeFile;
            if (i < 10) {
                volumeFile = baseName + ".7z.00" + std::to_string(i);
            }
            else if (i < 100) {
                volumeFile = baseName + ".7z.0" + std::to_string(i);
            }
            else {
                volumeFile = baseName + ".7z." + std::to_string(i);
            }

            if (FileExists(volumeFile)) {
                volumeFiles.push_back(volumeFile);
                std::cout << "Find 7z Volume: " << volumeFile << std::endl;
            }
            else if (i == 1) {
                break;
            }
            else {
                break;
            }
        }
    }

    std::sort(volumeFiles.begin(), volumeFiles.end());
}

bool SimpleBandizipCompressor::FileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

uint64_t SimpleBandizipCompressor::GetFileSize(const std::string& path) {
    try {
        return fs::file_size(path);
    }
    catch (...) {
        return 0;
    }
}

std::string SimpleBandizipCompressor::GetBandizipExecutablePath() {

    const std::vector<std::string> possiblePaths = {
        "Bandizip.exe",
        "bz.exe",
        //"C:\\Program Files\\Bandizip\\Bandizip.exe",
        "C:\\Program Files\\Bandizip\\bz.exe",
        "C:\\Program Files (x86)\\Bandizip\\Bandizip.exe",
        "C:\\Program Files (x86)\\Bandizip\\bz.exe"
    };

    for (const auto& path : possiblePaths) {
        if (FileExists(path)) {
            return path;
        }
    }

    return "Bandizip.exe"; 
}

bool SimpleBandizipCompressor::IsBandizipAvailable() {
    SimpleBandizipCompressor compressor;
    std::string testOutput;

    std::string testCommand = "\"" + compressor.m_bandizipPath + "\"";
    bool available = compressor.ExecuteBandizipCommand(testCommand, testOutput);

    if (available) {
    }
    else {
    }

    return available;
}

std::string SimpleBandizipCompressor::GetBandizipVersion() {
    if (!IsBandizipAvailable()) {
        return "Bandizip Unavailable";
    }

    SimpleBandizipCompressor compressor;
    std::string output;
    std::string testCommand = "\"" + compressor.m_bandizipPath + "\"";
    if (compressor.ExecuteBandizipCommand(testCommand, output)) {
        size_t pos = output.find("Bandizip");
        if (pos != std::string::npos) {
            size_t end = output.find("\n", pos);
            if (end != std::string::npos) {
                return output.substr(pos, end - pos);
            }
        }
    }

    return "Bandizip (Version unknown)";
}

SimpleBandizipCompressor::ZipVolumeInfo SimpleBandizipCompressor::ScanZipVolumeFiles(const std::string& directory,
    const std::string& targetFileName)
{
    ZipVolumeInfo info;
    namespace fs = std::filesystem;

    if (directory.empty() || targetFileName.empty()) {
        std::cerr << "file path is empty" << std::endl;
        return info;
    }

    try {
        fs::path targetPath(targetFileName);
        std::string baseName = targetPath.stem().string();

 
        fs::path mainFilePath = fs::path(directory) / targetFileName;
        if (fs::exists(mainFilePath)) {
            info.mainArchive = mainFilePath.string();
            //info.volumeFiles.push_back(mainFilePath.string());
        }

        std::string volumePattern = baseName + ".z";

        for (const auto& entry : fs::directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;

            std::string filename = entry.path().filename().string();
            std::string fullPath = entry.path().string();

            if (filename == targetFileName) continue;

            if (filename.find(volumePattern) == 0) {
                info.volumeFiles.push_back(fullPath);
              /*  std::string extension = filename.substr(volumePattern.length());
                if (extension.length() == 2 &&
                    std::isdigit(extension[0]) &&
                    std::isdigit(extension[1])) {
                    info.volumeFiles.push_back(fullPath);
                }*/
            }
        }

        std::sort(info.volumeFiles.begin(), info.volumeFiles.end());

        info.totalVolumes = info.volumeFiles.size();
        info.isMultiVolume = (info.totalVolumes > 1);

    }
    catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return info;
}

SimpleBandizipCompressor::ExtractionResult SimpleBandizipCompressor::ExtractArchive(
    const std::string& archivePath,
    const std::string& outputDir,
    const std::string& password) {

    ExtractionResult result;

    if (!FileExists(archivePath)) {
        result.errorMessage = "The compressed file does not exist: " + archivePath;
        return result;
    }

    if (!IsBandizipAvailable()) {
        result.errorMessage = "Bandizip is not available";
        return result;
    }

    try {
        std::string command = BuildExtractCommand(archivePath, outputDir, password);

        std::string output;
        bool success = ExecuteBandizipCommand(command, output);

        if (!success) {
            result.errorMessage = "Decompression failed: " + output;
            return result;
        }

        if (outputDir.empty()) {
            result.extractedPath = fs::current_path().string();
        }
        else {
            result.extractedPath = outputDir;
        }

        result.success = true;

    }
    catch (const std::exception& e) {
        result.errorMessage = std::string("Exception occurred during decompression process: ") + e.what();
    }

    return result;
}

bool SimpleBandizipCompressor::IsSplitArchive(const std::string& archivePath) {
    std::vector<std::string> volumeFiles;
    FindVolumeFiles(archivePath, volumeFiles);
    return volumeFiles.size() > 1;
}

std::string SimpleBandizipCompressor::BuildExtractCommand(
    const std::string& archivePath,
    const std::string& outputDir,
    const std::string& password) {

    std::stringstream cmd;

    cmd << "\"" << m_bandizipPath << "\" x ";

    if (!outputDir.empty()) {
        cmd << "-o:\"" << outputDir << "\" ";
    }

    if (!password.empty()) {
        cmd << "-p:" << password << " ";
    }

    cmd << "-y ";

    cmd << "\"" << archivePath << "\"";

    return cmd.str();
}