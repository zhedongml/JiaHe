#pragma once
#include <QMutex>
#include <QObject>
#include <atomic>
#include <chrono>
#include <semaphore>

#include "MLBinoBusinessManage.h"
#include "MLMonoBusinessManage.h"
#include "MLTaskCommon.h"
#include "MLTaskManagerPlugin.h"
#include "MPMCThreadPool/MPMCThreadPool.hpp"
#include "Result.h"
#include "mltaskmanager_global.h"
using namespace ML::MLColorimeter;
using namespace ML::CameraV2;
using namespace ML::MLFilterWheel;

namespace MLAbstractProcess {
class AbstractProcessor;
}
class ProcessEventCallback;
class DarkCorrect;
class ColorShiftCorrect;
class DistortionCorrect;
class ExposureCorrect;
class FFCCorrect;
class FlipRotationCorrect;
class FourColorCorrect;
class FOVCropCorrect;
class LuminanceCorrect;
class KeystoneCorrect;

Q_DECLARE_METATYPE(std::shared_ptr<ML_Task::ImageAlgoMetaData>)

namespace ML_Task {
class MLTASKMANAGER_EXPORT MLTaskManager : public QObject, public MLTaskManagerPlugin {
    Q_OBJECT

    Q_INTERFACES(MLTaskManagerPlugin)

    Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MLTaskManager.json")

 public:
    MLTaskManager(MLBinoBusinessManage* bino);

    MLTaskManager();

    ~MLTaskManager();

 public:
    Result CaptureImageSyncTask(ImageCaptureConfig c_config);

    Result CaptureImageAsyncTask(ImageCaptureConfig c_config);

    Result CaptureImageAsyncTask(ImageCaptureConfig c_config, ML::CameraV2::Binning softBinning,
                                 ML::CameraV2::BinningMode softBinningMode);

    Result LoadCalibrationModule(ProcessType, const CalibrationConfig2& config, const std::string& configPath = "");

    Result GetNormalTasksResult(int single_task_wait);

    Result GetFourcolorTasksResult(int single_task_wait);

    void RegisterSaveCallBack(ProcessEventCallback* cb);

 signals:
    void dispatch_image_to_compute(std::shared_ptr<ML_Task::ImageAlgoMetaData> metadata);
    void sigCorrectsStatus(std::string, std::string, bool);

 //public:
 //   Result ProcessTask_Rotate(ProcessorData processorData, CalibrationFlag cali_flag,bool is_raw);

 private:
    Result ProcessTask(ProcessorData processorData, CalibrationFlag cali_flag, CalibrationFlag cxt_flag, std::string uid,
                       bool is_raw);

    std::vector<ProcessorData> DequeuePreDataByUID(mpmc_tp::ConcurrentQueue<std::pair<std::string, ProcessorData>>& queue,
                                                   const std::string& uid);

    Result saveResultData(ProcessorData output, bool isFourColor);

    Result saveRawData(ProcessorData output);

    Result CaptureProcess(CaptureData& data, ImageCaptureConfig& c_config, ML::MLFilterWheel::MLFilterEnum xyzFilter);

 private:
    MLMonoBusinessManage* m_mono = nullptr;
    MLBinoBusinessManage* m_bino = nullptr;
    std::string m_configPath;
    const string ND_FILTER_KEY = "NDFilter";
    const string XYZ_FILTER_KEY = "ColorFilter";

 private:
    // MPMC thread pool
    mpmc_tp::MPMCThreadPool threadPool;
    mpmc_tp::ConcurrentQueue<std::pair<SaveDataMeta, ProcessorData>> saveDataQuene;
    std::vector<std::future<Result>> m_taskFutures, m_fourcolorTaskFutures;
    std::mutex m_futuresMutex;
    mpmc_tp::ConcurrentQueue<std::pair<std::string, ProcessorData>> preFourColorQuene;
    std::thread m_saveThread;
    bool m_stopSaveFlag = false;
    std::atomic<int> m_pendingSaves;
    ProcessEventCallback* m_eventCbkPt = nullptr;

    // process
    DarkCorrect* darkCorrect = nullptr;
    ColorShiftCorrect* colorShiftCorrect = nullptr;
    DistortionCorrect* distortionCorrect = nullptr;
    ExposureCorrect* exposureCorrect = nullptr;
    FFCCorrect* ffcCorrect = nullptr;
    FlipRotationCorrect* flipRotationCorrect = nullptr;
    FourColorCorrect* fourColorCorrect = nullptr;
    FOVCropCorrect* fovCropCorrect = nullptr;
    LuminanceCorrect* luminanceCorrect = nullptr;
    KeystoneCorrect* keystoneCorrect = nullptr;
    std::unordered_map<ProcessType, MLAbstractProcess::AbstractProcessor*> processModules;
    const QString& RuleFile = "./ImangeNameType.ini";
    // ImageNameRuleInfo nameRuleInfo;
    QMutex qmutex;
};
}  // namespace ML_Task
