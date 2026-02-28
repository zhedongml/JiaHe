#include "ml_camera_observer.h"

void MLCameraObserver::CameraListChanged(CameraPtr camera_ptr, UpdateTriggerType reason)
{
    if (UpdateTriggerPluggedIn == reason || UpdateTriggerPluggedOut == reason)
    {
        camera_list_changed_(camera_ptr, reason, system_);// Emit the new camera signal
    }
}
