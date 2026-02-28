#pragma once
#include <QObject>
#include <thread>
#include <mutex>
#include "MLColorimeterMode.h"
#include "ML_DiopterScan.h"

using namespace ML_AF;

enum AUTO_FOUS_STATUS
{
    AF_NO_CONNECT,
    AF_STATUS_START,
    AF_STATUS_OK,
    AF_STATUS_ERROR,

    AF_STATUS_CONNECTTING,

    AF_STATUS_STOPFOCUS,
    AF_STATUS_IN_SETFOCUSING,
    AF_STATUS_IN_FOCUSING,

    AF_STATUS_FOCUS_FINE,
    AF_STATUS_FOCUS_ROUGH
};

class AutoFocusModel: public QObject
{
    Q_OBJECT
  public:
    static AutoFocusModel *instance(QObject *parent = nullptr);
    ~AutoFocusModel();

    Result DiopterScanProcess(DiopterScanConfig config, std::string file_name, std::string direction);
    Result FocusScanProcess(FocusScanConfig config, std::string file_name, std::string direction);

    std::vector<cv::Point3f> GetDiopterScanResult();
    MLDiopterScan* GetDiopterScanManage();

    void StopTreeSystemCallback(bool isStop);

    Result SetFocusMotionByLight(QString lightsource);

  private:
    AutoFocusModel(QObject *parent = nullptr);

    void init();
    Result GetMTFROIs_Grid(std::string file_path, std::vector<cv::Rect>& rois, const cv::Mat& image);
  private:
    static AutoFocusModel *m_instance;
    std::mutex m_mutex;

    MLDiopterScan* m_AFProcess = nullptr;

    bool m_coarseThruFocusStop;
};
