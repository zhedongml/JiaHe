#ifndef MLVIMBACAMERA_SYSTEM_H_
#define MLVIMBACAMERA_SYSTEM_H_

#include "ml_camera_observer.h"
#include "LogPlus.h"

class __declspec(dllexport) MLCameraSystem
{
public:
    static MLCameraSystem& GetInstance() {
        static MLCameraSystem vimbaSystem;
        return vimbaSystem;
    }

private:
    MLCameraSystem();
    ~MLCameraSystem();                           // ctor hidden
    MLCameraSystem(MLCameraSystem const&);            // copy ctor hidden
    MLCameraSystem& operator=(MLCameraSystem const&); // assign op. hidden

public:
    VmbErrorType StartUp();

    void ShutDown();

    CameraPtr GetCameraPtrBySN(std::string serial_number);

    static void MLCameraListChanged2(CameraPtr camera_ptr, UpdateTriggerType reason, void* context);


private:
    // A reference to our Vimba singleton
    VimbaSystem& m_system_;

    ICameraListObserverPtr camera_observer_ptr_;
};

#endif // !MLVIMBACAMERA_SYSTEM_H_