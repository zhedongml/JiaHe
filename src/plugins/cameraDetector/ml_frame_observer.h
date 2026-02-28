#ifndef MLVIMBACAMERA_FRAME_OBSERVER_H_
#define MLVIMBACAMERA_FRAME_OBSERVER_H_

#include <VimbaCPP/Include/VimbaCPP.h>
#include "ml_vimba_camera.h"
#include "PrjCommon/service/ml.h"

using namespace CORE;
using namespace AVT::VmbAPI;

class MLFrameObserver : virtual public IFrameObserver
{
public:
    //
    // We pass the camera that will deliver the frames to the constructor
    //
    // Parameters:
    //  [in]    pCamera         The camera the frame was queued at
    //
    MLFrameObserver(CameraPtr camera_ptr, void* context) : IFrameObserver(camera_ptr)
    {

        vimba_camera_ = (MLVimbaCamera*)context;

        frame_received_ = false;
    }

    //
    // This is our callback routine that will be executed on every received frame.
    // Triggered by the API.
    //
    // Parameters:
    //  [in]    pFrame          The frame returned from the API
    //
    virtual void FrameReceived(const FramePtr pFrame);

private:

    MLVimbaCamera* vimba_camera_;

    bool frame_received_;


};

#endif // !MLVIMBACAMERA_FRAME_OBSERVER_H_

