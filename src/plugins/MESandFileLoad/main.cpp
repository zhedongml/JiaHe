//#include "BandizipCompressor.h"
//#include <iostream>
//#include <chrono>
//#include <iomanip>
//
//void PrintUsage() {
//    std::cout << "=== 简单Bandizip分卷压缩工具 ===" << std::endl;
//    std::cout << "用法:" << std::endl;
//    std::cout << "  直接运行程序，按照提示操作" << std::endl;
//    std::cout << "支持的格式: zip, 7z" << std::endl;
//    std::cout << "分卷命名: .zip, .z01, .z02, ... 或 .7z.001, .7z.002, ..." << std::endl;
//    std::cout << "完全支持UTF-8编码的文件名和内容" << std::endl;
//    std::cout << std::endl;
//}
//
//void PrintResult(const SimpleBandizipCompressor::CompressionResult& result) {
//    std::cout << std::endl << "=== 压缩结果 ===" << std::endl;
//
//    if (result.success) {
//        std::cout << "✓ 压缩成功!" << std::endl;
//        std::cout << "生成的文件:" << std::endl;
//
//        for (const auto& file : result.volumeFiles) {
//            uint64_t fileSize = 0;
//            try {
//                fileSize = fs::file_size(file);
//            }
//            catch (...) {}
//            double sizeMB = fileSize / (1024.0 * 1024.0);
//            std::cout << "  " << file << " (" << std::fixed << std::setprecision(2) << sizeMB << " MB)" << std::endl;
//        }
//
//        std::cout << "总压缩大小: " << (result.totalCompressedSize / (1024.0 * 1024.0)) << " MB" << std::endl;
//        std::cout << "压缩率: " << std::fixed << std::setprecision(1) << result.compressionRatio << "%" << std::endl;
//
//        std::cout << std::endl << "提示: 要解压分卷文件，请解压第一个文件(.zip 或 .7z.001)" << std::endl;
//    }
//    else {
//        std::cout << "✗ 压缩失败!" << std::endl;
//        std::cout << "错误信息: " << result.errorMessage << std::endl;
//    }
//}
//
//int main() {
//    PrintUsage();
//
//    // 检查Bandizip可用性
//    if (!SimpleBandizipCompressor::IsBandizipAvailable()) {
//        std::cerr << "错误: Bandizip不可用" << std::endl;
//        std::cerr << "请确保Bandizip已安装，并且Bandizip.exe在PATH中或可执行文件同目录下" << std::endl;
//        return -1;
//    }
//
//    std::cout << "Bandizip 版本: " << SimpleBandizipCompressor::GetBandizipVersion() << std::endl;
//
//    std::string inputPath, outputArchive;
//    uint64_t volumeSizeMB;
//    std::string format;
//
//    std::cout << "请输入要压缩的文件或目录路径: ";
//    std::getline(std::cin, inputPath);
//
//    std::cout << "请输入输出归档文件路径: ";
//    std::getline(std::cin, outputArchive);
//
//    std::cout << "请输入分卷大小(MB): ";
//    std::cin >> volumeSizeMB;
//
//    std::cout << "请选择压缩格式 (zip/7z): ";
//    std::cin >> format;
//
//    // 创建压缩器
//    //SimpleBandizipCompressor compressor;
//
//    // 配置压缩参数
//    SimpleBandizipCompressor::CompressionConfig config;
//    config.volumeSizeMB = volumeSizeMB;
//    config.archiveFormat = format;
//    config.compressionLevel = 5; // 中等压缩
//    config.multiThread = true;
//
//    std::cout << "开始压缩..." << std::endl;
//    auto startTime = std::chrono::high_resolution_clock::now();
//
//    // 执行压缩
//    SimpleBandizipCompressor::CompressionResult result;
//
//    if (fs::is_directory(inputPath)) {
//        result = SimpleBandizipCompressor::instance()->CompressDirectory(inputPath, outputArchive, config);
//    }
//    else {
//        result = SimpleBandizipCompressor::instance()->CompressFile(inputPath, outputArchive, config);
//    }
//
//    auto endTime = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
//
//    std::cout << std::endl << "压缩耗时: " << duration.count() << " 秒" << std::endl;
//
//    // 显示结果
//    PrintResult(result);
//
//    if (!result.success) {
//        return -1;
//    }
//
//    std::cout << std::endl << "按任意键退出..." << std::endl;
//    std::cin.ignore();
//    std::cin.get();
//
//    return 0;
//}