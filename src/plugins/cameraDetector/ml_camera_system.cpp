#include "ml_camera_system.h"

MLCameraSystem::MLCameraSystem():m_system_(VimbaSystem::GetInstance())
{
    StartUp();
}

MLCameraSystem::~MLCameraSystem()
{
    //ShutDown();
}

VmbErrorType MLCameraSystem::StartUp()
{
    std::string message;
    VmbErrorType res;

    // Start Vimba
    res = m_system_.Startup();


    if (VmbErrorSuccess == res)
    {
        SP_SET(camera_observer_ptr_, new MLCameraObserver(MLCameraListChanged2, this));

        // Register an observer whose callback routine gets triggered whenever a camera is plugged in or out
        res = m_system_.RegisterCameraListObserver(camera_observer_ptr_);
        if (VmbErrorSuccess != res) 
        {
            LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Camera observer register failed!");
        }
    }
    else
    {
        std::string message = "Could not start system. Error code: " + std::to_string(res);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
    }


    return res;
}

void MLCameraSystem::ShutDown()
{
    try
    {
        m_system_.Shutdown();
    }
    catch (...)
    {
        // TODO: logging
    }
}

CameraPtr MLCameraSystem::GetCameraPtrBySN(std::string serial_number)
{
    CameraPtrVector cameras;
    CameraPtr cam;
    // Get all known cameras
    if (VmbErrorSuccess == m_system_.GetCameras(cameras))
    {
        std::for_each(cameras.begin(), cameras.end(), [&](CameraPtr camera_ptr) {

            std::string sn;
            camera_ptr->GetSerialNumber(sn);

            if ((sn.compare(serial_number)) == 0)
            {
                cam = camera_ptr;
            }
            });
        // And return them
        return cam;
    }
    else
    {
        return cam;
    }
   
}

void MLCameraSystem::MLCameraListChanged2(CameraPtr camera_ptr, UpdateTriggerType reason, void* context)
{
    MLCameraSystem* camera_system = (MLCameraSystem*)context;
    std::string serial_number;
    camera_ptr->GetSerialNumber(serial_number);
    if (UpdateTriggerPluggedOut == reason)
    {
        std::string message = "The camera ["+ serial_number +"] was disconnected from the system ";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
    }
    else
    {
        std::string message = "The camera [" + serial_number + "] was connected to the system ";
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, message.c_str());
    }
}

