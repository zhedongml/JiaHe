#include "ImageProcessor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

int main() {
    ImageProcessor processor;

    // 配置处理器
    processor.setConfig(
        "D:\\project\\jiaX\\zww\\AutoDP\\Sandbox_v0.0_D86391381_20251106_Sunny_test\\",
        "Sunny_DIQT1",                                  
        "Kitefin P2",                                 
        "D:\\project\\jiaX\\zww\\slb\\20251114_SLB_Right_MetricsTest\\",
        "D:\\AutoDPImage"                                  // 临时目录
    );

    // 启动处理服务
    if (!processor.startService().success) {
        std::cerr << "Failed to start processing service!" << std::endl;
        return -1;
    }

    std::cout << "Image processing service started successfully." << std::endl;

    // 模拟采集多组图像并提交处理任务
    std::vector<std::pair<std::string, std::string>> tasks = {
        {"D:\\project\\jiaX\\zww\\dut\\2\\3N2QG01HBK0892_20251121T125127_MetricsTest", "D:\\AutoDPResult"},
        {"D:\\project\\jiaX\\zww\\dut\\1\\3N2QG01HBK0891_20251121T124007_MetricsTest", "D:\\AutoDPResult"}
    };

    for (int i = 0; i < tasks.size(); ++i) {
        const auto& task = tasks[i];

        if (processor.submitProcessingTask(task.first, task.second).success) {
            std::cout << "Successfully submitted task " << (i + 1) << std::endl;
        }
        else {
            std::cerr << "Failed to submit task " << (i + 1) << std::endl;
        }

        // 模拟采集间隔（2秒）
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // 监控任务进度
    while (processor.isRunning() && processor.getPendingTaskCount() > 0) {
        std::cout << "Pending tasks: " << processor.getPendingTaskCount()
            << ". Waiting for completion..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // 停止服务
    processor.stopService();

    std::cout << "All tasks completed. Program finished." << std::endl;
    return 0;
}