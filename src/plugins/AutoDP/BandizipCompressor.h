#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "autodp_global.h"

namespace fs = std::filesystem;

class AUTODP_EXPORT SimpleBandizipCompressor {
public:
    struct CompressionResult {
        bool success = false;
        std::vector<std::string> volumeFiles;
        std::string errorMessage;
        uint64_t totalCompressedSize = 0;
        double compressionRatio = 0.0;
    };

    struct CompressionConfig {
        uint64_t volumeSizeMB = 1024; // 1GB in MB
        int compressionLevel = 5; // 0-9
        std::string archiveFormat = "zip"; // "zip" or "7z"
        std::string password; // optional password
        bool multiThread = true;
    };

    struct ExtractionResult {
        bool success = false;
        std::string errorMessage;
        std::string extractedPath;
    };

    struct ZipVolumeInfo {
        int totalVolumes = 0;                 
        std::vector<std::string> volumeFiles;    
        std::string mainArchive;                 
        bool isMultiVolume = false;             
    };

public:
    ~SimpleBandizipCompressor();
    static SimpleBandizipCompressor* instance();
    static SimpleBandizipCompressor* self;

    CompressionResult CompressFile(const std::string& inputFile,
        const std::string& outputArchive,
        const CompressionConfig& config = CompressionConfig());

    CompressionResult CompressDirectory(const std::string& inputDir,
        const std::string& outputArchive,
        const CompressionConfig& config = CompressionConfig());

    ExtractionResult ExtractArchive(const std::string& archivePath,
        const std::string& outputDir = "",
        const std::string& password = "");

    bool IsSplitArchive(const std::string& archivePath);

    static bool IsBandizipAvailable();
    static std::string GetBandizipVersion();

    ZipVolumeInfo ScanZipVolumeFiles(const std::string& directory,
        const std::string& targetFileName);

private:

    bool ExecuteBandizipCommand(const std::string& command, std::string& output);

    std::string BuildBandizipCommand(const std::string& inputPath,
        const std::string& outputArchive,
        const CompressionConfig& config);

    std::string BuildExtractCommand(const std::string& archivePath,
        const std::string& outputDir,
        const std::string& password);

    void FindVolumeFiles(const std::string& baseArchiveName, std::vector<std::string>& volumeFiles);

    bool FileExists(const std::string& path);
    uint64_t GetFileSize(const std::string& path);
    std::string GetBandizipExecutablePath();

private:
    SimpleBandizipCompressor();

    std::string m_bandizipPath;
};