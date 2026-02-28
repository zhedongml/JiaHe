#pragma once
// 注意：m_mono 的生命周期由外部管理，MLDiopterScan 不负责 delete
#include "MLColorimeterCommon.h"
#include "MLMonoBusinessManage.h"
#include "MPMCThreadPool/MPMCThreadPool.hpp"
#include "Result.h"
#include "mlautofocusprocess_global.h"
#include "opencv2/opencv.hpp"
#include <QObject>

using namespace ML::MLColorimeter;

namespace ML_AF {

    enum class DiopterScanEnum {
        Motion,
        VID,
        Diopter,
        MTF
    };

class MLAUTOFOCUSPROCESS_EXPORT MLDiopterScan : public QObject {
    Q_OBJECT

 public:
    MLDiopterScan(MLMonoBusinessManage* mono);
    ~MLDiopterScan();

    Result ML_DiopterScan(std::string keyName, std::string savepath,
                          ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig());

    Result DiopterScan(std::string keyName, std::string savepath, ML::MLColorimeter::DiopterScanConfig dscanConfig);

    Result ML_DiopterCoarseScan(std::string keyName, std::string savepath,
                                ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig(),
                                bool useMeanStdDev = false);

    Result ML_DiopterFineScan(std::string keyName, std::string savepath,
                              ML::MLColorimeter::DiopterScanConfig dscanconfig = ML::MLColorimeter::DiopterScanConfig());

    Result ML_CalculateCombinMap(std::string direction, std::vector<int> indexVec, bool isfine = false);

    Result ML_SaveDiopterScanResult(std::string path, std::string prefix = "");

    std::vector<cv::Point3f> ML_GetDiopterScanResult();

    std::map<std::string, cv::Point3f> ML_GetDiopterScanHVAVGReMap();

    Result WaitForTasksFinish(int single_task_wait);

    void StartThroughFocus();

    void StopThroughFocus();


 private:
    Result CalculatePeakRange(std::map<DiopterScanEnum, std::vector<double>> dataMap, int& start, int& end);

    Result saveDataToCsv(std::string filedir, std::string filename, std::map<DiopterScanEnum, std::vector<double>> dataMap);

 signals:
    void coarseScanProgress(double);
    void fineScanProgress(double);

 private:
    MLMonoBusinessManage* m_mono = nullptr;

    QMutex m_mutex;
    bool m_isAutoFocusStart = false;
    const string CAMERA_KEY = "VieCamera1";

    ML::MLColorimeter::DiopterScanConfig m_diopterScanConfig;
    double m_absDiopter;

    std::vector<std::map<DiopterScanEnum, std::vector<double>>> m_coarseDiop_vec;
    std::vector<std::map<DiopterScanEnum, std::vector<double>>> m_fineDiop_vec;
    std::vector<std::map<DiopterScanEnum, std::vector<double>>> m_fitDiop_vec;
    std::map<std::string, std::map<DiopterScanEnum, std::vector<double>>> m_coarseDiop_combinMap;
    std::map<std::string, std::map<DiopterScanEnum, std::vector<double>>> m_fineDiop_combinMap;
    std::map<std::string, std::map<DiopterScanEnum, std::vector<double>>> m_fitDiop_combinMap;

    // x : diopter, y : vid, z : mtf.
    std::vector<cv::Point3f> m_diopterScanResult;
    std::map<std::string, cv::Point3f> m_diopScanRe_combinMap;

    // save images thread pool
    mpmc_tp::MPMCThreadPool threadPool;
    std::mutex m_futuresMutex;
    std::vector<std::future<void>> m_taskFutures;
};
}  // namespace ML_AF
