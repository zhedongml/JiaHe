// Copyright 2025 MLOptic
// 注意：m_mono 的生命周期由外部管理，MLFocusScan 不负责 delete

#ifndef SRC_MLAUTOFOCUSPRCESS_MLFOCUSSCAN_H_
#define SRC_MLAUTOFOCUSPRCESS_MLFOCUSSCAN_H_

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

namespace ML {
namespace AutoFocus {

class MLAUTOFOCUSPROCESS_EXPORT MLFocusScan : public QObject {
    Q_OBJECT

 public:
    MLFocusScan();
    ~MLFocusScan();

    Result ML_SetMonoManage(ML::MLColorimeter::MLMonoBusinessManage* mono);

    bool ML_StartFocusScan();

    bool ML_StopFocusScan();

    Result ML_WaitFocusScanFinish();

    Result ML_InitFocusScanConfig(std::string configPath, std::string motionName, ML::AutoFocus::FocusScanConfig config);

    ML::AutoFocus::FocusScanConfig ML_GetFocusScanConfig();

    Result ML_FocusScanAsync(std::string motionName, ML::AutoFocus::FocusScanConfig config);

    Result ML_CoarseFocusScan(std::string motionName, ML::AutoFocus::FocusScanConfig config, bool useMeanStdDev = false);

    Result ML_FineFocusScan(std::string motionName, ML::AutoFocus::FocusScanConfig config);

    Result ML_SaveFocusScanResult(std::string path, std::string prefix = "");

    std::vector<cv::Point3d> ML_GetFocusScanResult();

    Result ML_CalculateCombinMap(std::string direction, std::vector<int> indexVec);

    std::map<std::string, cv::Point3d> ML_GetCombinResult();

 public:
    Result WaitForTasksFinish(int single_task_wait = 60);

 private:
    Result focusScan(std::string motionName, ML::AutoFocus::FocusScanConfig config);

    Result focusScanFinish();

    Result saveDataToCsv(std::string filedir, std::string filename,
                         std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> dataMap);

    Result calMTFTasks(double pos, double vid, cv::Mat image, ML::AutoFocus::FocusScanConfig focusConfig,
                       std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>>& curve, bool useMeanStdDev,
                       int single_task_wait = 60);

    Result calPeakRange(std::map<ML::AutoFocus::ScanEnum, std::vector<double>> dataMap, int& start, int& end);

    Result polynomialFit_And_calPeakRange(std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>> srcMap,
                                          std::vector<std::map<ML::AutoFocus::ScanEnum, std::vector<double>>>& dstMap,
                                          std::vector<cv::Point3d>& resultData, bool calPeak = true);

 signals:
    void coarseScanProgress(double);

    void fineScanProgress(double);

    void focusScanFinishSignal(Result);

 private:
    ML::MLColorimeter::MLMonoBusinessManage* m_mono = nullptr;
    string CAMERA_KEY = "VieCamera1";

    QMutex m_mutex;
    QFutureWatcher<Result> m_watcher;
    ML::AutoFocus::FocusScanConfig m_focusScanConfig;
    bool m_isAutoFocusStart = false;
    bool m_isReverse = false;
    double m_absPosition = 0.0;
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

    // auto focus result, [x : position, y : vid, z : mtf]
    std::vector<cv::Point3d> m_result;
    std::map<std::string, cv::Point3d> m_combinResult;
};
}  // namespace AutoFocus
}  // namespace ML

#endif  // SRC_MLAUTOFOCUSPRCESS_MLFOCUSSCAN_H_
