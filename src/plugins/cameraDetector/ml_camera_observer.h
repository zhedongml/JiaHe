#ifndef MLVIMBACAMERA_CAMERA_OBSERVER_H_
#define MLVIMBACAMERA_CAMERA_OBSERVER_H_

#include <VimbaCPP/Include/VimbaCPP.h>
//#include "ml_vimba_camera.h"

using namespace AVT::VmbAPI;

typedef void(*MLCameraListChanged)(CameraPtr camera_ptr, UpdateTriggerType reason,void* context);

class MLCameraObserver : public ICameraListObserver
{
public:

    MLCameraObserver(MLCameraListChanged camera_list_changed, void* context) : ICameraListObserver()
    {
         camera_list_changed_ = camera_list_changed;
         system_ = context;
    }
       
   

    //
    // This is our callback routine that will be executed every time a camera was plugged in or out
    //
    // Parameters:
    //  [in]    pCam            The camera that triggered the callback
    //  [in]    reason          The reason why the callback was triggered
    //
    virtual void CameraListChanged(CameraPtr camera_ptr, UpdateTriggerType reason);

private:

    MLCameraListChanged camera_list_changed_;

    void* system_;
};

#endif // !MLVIMBACAMERA_CAMERA_OBSERVER_H_
