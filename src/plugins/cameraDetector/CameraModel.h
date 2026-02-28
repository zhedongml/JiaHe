/****************************************************************************
* @author  : Michael Gao                                                     
* @date    : 2021/12/21                                                      
* @file    :                                                                 
* @brief   : CameraModel,handle camera interactions,                         
*            such as exposure, stream and so on                              
*----------------------------------------------------------------------------
*                           Change History                                   
*----------------------------------------------------------------------------
* Date        | Version   | Author         | Description                     
*----------------------------------------------------------------------------
*             |           |                |                                 
*****************************************************************************/
#pragma once

#include <QObject>
#include "PrjCommon/service/ml_camera.h"
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QVector>
#include <QList>
#if defined(MYSHAREDLIB_LIBRARY)
#  define MYSHAREDLIB_EXPORT Q_DECL_EXPORT
#else
#  define MYSHAREDLIB_EXPORT Q_DECL_IMPORT
#endif

using namespace CORE;

class MYSHAREDLIB_EXPORT CameraModel : public QObject, public MLCameraCallback
{
    Q_OBJECT
public:
   CameraModel(QObject* parent = Q_NULLPTR);
   ~CameraModel();

public slots:
    void SetMLExposureAuto();

    void SetExposureTime(double time);

    double GetExposureTime();

    cv::Mat GetImage();

    void SubscribeCameraEvent(MLCameraEvent event);

    void UnsubscribeCameraEvent(MLCameraEvent event);

    void Enqueue(const QImage image);

    QImage GetQImage();

    void ClearImageQueue();

    int GetGrayLevel();

signals:
	void updateCameraStatusSignal(MLCameraState status);
signals:
	void updateCameraStreamSignal();
signals:
	void updateCameraGrayLevelSignal(int gray_level);


private:
	void NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state);
	void NotifyCameraFrameReceived(MLFrame* frame_ptr, void* buffer_ptr);
	void NotifyCameraGrayLevel(int gray_level);
    void NotifyAutoFocus(MLStatus status, double progress, int* steparray, double* distance, double* data, int len) {};
    bool m_bConnected = false;

protected:
    MLCamera* m_pCamera = nullptr;

    QImage m_Image;

    int m_GrayLevel;

    QList<QImage> m_Data;
    mutable QMutex m_Lock;            
    QWaitCondition m_DataAvailable;    
    bool m_Stopping;
    const size_t m_MaxSize = 3;
public :
   bool connect(const char* cameraSn);
   bool disConnect();
};
