// Copyright 2025 MLOptic
// 注意：m_mono 的生命周期由外部管理，MLDiopterScan 不负责 delete

#ifndef SRC_MLAUTOFOCUSPRCESS_MLDIOPTERSCAN_H_
#define SRC_MLAUTOFOCUSPRCESS_MLDIOPTERSCAN_H_

#include <QtCore/qfuturewatcher.h>
#include <QtCore/qobject.h>

#include <map>
#include <tuple>
#include <vector>

#include "MLAutoFocusCommon.h"
#include "MLColorimeterCommon.h"
#include "MLMonoBusinessManage.h"
#include "MPMCThreadPool/MPMCThreadPool.hpp"
#include "Result.h"
#include "mlautofocusprocess_global.h"

using namespace ML::MLColorimeter;

namespace ML {
namespace AutoFocus {

enum class DiopterScanEnum { Motion, VID, Diopter, MTF };

class MLAUTOFOCUSPROCESS_EXPORT MLDiopterScan : public QObject {
    Q_OBJECT

 public:
    MLDiopterScan();
    ~MLDiopterScan();

    Result ML_SetMonoManage(ML::MLColorimeter::MLMonoBusinessManage* mono);

    bool ML_StartDiopterScan();

    bool ML_StopDiopterScan();

    Result ML_WaitDiopterScanFinish();

    Result ML_InitDiopterScanConfig(std::string configPath, std::string motionName, ML::AutoFocus::DiopterScanConfig config);

    ML::AutoFocus::DiopterScanConfig ML_GetDiopterScanConfig();

    Result ML_DiopterScanAsync(std::string motionName, ML::AutoFocus::DiopterScanConfig config);

    Result ML_CoarseDiopterScan(std::string motionName, ML::AutoFocus::DiopterScanConfig config, bool useMeanStdDev = false);

    Result ML_FineDiopterScan(std::string motionName, ML::AutoFocus::DiopterScanConfig config);

    Result ML_SaveDiopterScanResult(std::string path, std::string prefix = "");

    std::vector<cv::Point3d> ML_GetDiopterScanResult();

    Result ML_CalculateCombinMap(std::string direction, std::vector<int> indexVec);

    std::map<std::string, cv::Point3d> ML_GetCombinResult();

 public:
    Result WaitForTasksFinish(int single_task_wait = 60);

 private:
    Result diopterScan(std::string motionName, ML::AutoFocus::DiopterScanConfig config);

    Result diopterScanFinish();

    Result saveDataToCsv(std::string filedir, std::string filename,
                         std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> dataMap);

    Result calMTFTasks(double pos, double vid, cv::Mat image, ML::AutoFocus::DiopterScanConfig config,
                       std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>>& curve, bool useMeanStdDev,
                       int single_task_wait = 60);

    Result calPeakRange(std::map<ML::AutoFocus::ScanEnum, std::vector<double>> dataMap, int& start, int& end);

    Result polynomialFit_And_calPeakRange(std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> srcMap,
                                          std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>>& dstMap,
                                          std::vector<cv::Point3d>& resultData, bool calPeak = true);

 signals:
    void coarseScanProgress(double);

    void fineScanProgress(double);

    void diopterScanFinishSignal(Result);

 private:
    ML::MLColorimeter::MLMonoBusinessManage* m_mono = nullptr;
    string CAMERA_KEY = "VieCamera1";

    QMutex m_mutex;
    QFutureWatcher<Result> m_watcher;
    ML::AutoFocus::DiopterScanConfig m_diopterScanConfig;
    bool m_isAutoFocusStart = false;
    bool m_isReverse = false;
    double m_absDiopter = 0.0;
    int m_absIndex = 0;

    // save images thread pool
    mpmc_tp::MPMCThreadPool m_threadPool;
    std::mutex m_futuresMutex;
    std::vector<std::future<void>> m_taskFutures;

    // curve    [{ROI: {scanEnum, curve}}]
    std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_coarseCurve;
    std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_coarseFitCurve;
    std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_fineCurve;
    std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_fineFitCurve;

    // combination curve    ROI[<scanEnum, curve>]
    std::map<std::string, std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_combinCoarseCurve;
    std::map<std::string, std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_combinCoarseFitCurve;
    std::map<std::string, std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_combinFineCurve;
    std::map<std::string, std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> m_combinFineFitCurve;

    // auto focus result, [x : diopter, y : position, z : mtf]
    std::vector<cv::Point3d> m_result;
    std::map<std::string, cv::Point3d> m_combinResult;
};
}  // namespace AutoFocus
}  // namespace ML

#endif  //  SRC_MLAUTOFOCUSPRCESS_MLDIOPTERSCAN_H_
