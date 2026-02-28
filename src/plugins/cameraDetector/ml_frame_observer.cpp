#include "ml_frame_observer.h"
#include "iostream"

void MLFrameObserver::FrameReceived(const FramePtr frame_ptr)
{
    VmbFrameStatusType receive_status;

    if (frame_received_ == false && VmbErrorSuccess == frame_ptr->GetReceiveStatus(receive_status))
    {
        if (VmbFrameStatusComplete == receive_status)
        {
            frame_received_ = true;

            Mat img = vimba_camera_->SourceImage2CV(frame_ptr);

            vimba_camera_->CVImage2MLFrame(img);

            frame_received_ = false;
        }
        else
        {
            std::cout << " received frame unsuccessfully. Error code:" + receive_status << std::endl;
        }
        
    }

    // If any error occurred we queue the frame without notification
    if (false == frame_received_)
    {
        m_pCamera->QueueFrame(frame_ptr);
    }
}
