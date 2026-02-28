#include "pch.h"
#include "MLKeyenceCL.h"
#include <string>
#include <algorithm>
#include "CL3_IF.h"

MLKeyenceCL::MLKeyenceCL()
{
    threadStart();
}

MLKeyenceCL::~MLKeyenceCL()
{
    threadStop();
}

bool MLKeyenceCL::Connect(const char* type,int id)
{
    std::string s = type;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c);  });
     LONG rc;

    if (s == "usb")
    {
        rc = CL3IF_OpenUsbCommunication(id,3000);
    }
    else
    {
        // TODO: logging - debug
        throw MLRangeFinderError("unknown device type");
    }

    if (rc == CL3IF_RC_OK)
    {
        is_connected_ = true;
        deviceId = id;
        /*CL3IF_MEASUREMENTMETHOD_PARAM param = {};
        param.paramDisplacementForTransparent.headNo = static_cast<BYTE>(0);
        param.paramDisplacementForTransparent.peak = static_cast<BYTE>(CL3IF_TRANSPARENTPEAK_PLUS1);
        const LONG returnCode = CL3IF_SetMeasurementMethod(deviceId, 0, 0, CL3IF_MEASUREMENTMETHOD_DISPLACEMENT_FOR_TRANSPARENT, param);*/
    }
    else
    {
        is_connected_ = false;
    }

    return is_connected_;
}

bool MLKeyenceCL::Disconnect()
{
    if (deviceId != -1) {
       LONG rc = CL3IF_CloseCommunication(deviceId);
       if (rc == CL3IF_RC_OK)
       {
           is_connected_ = false;
           deviceId = -1;
           return true;
       }
       else {
           return false;
       }
    }
    return true;
}

bool MLKeyenceCL::IsConnected()
{
	return is_connected_;
}

double MLKeyenceCL::GetPosition(int index)
{
    CL3IF_MEASUREMENT_DATA measurementData = {};

    const LONG returnCode = CL3IF_GetMeasurementData(deviceId, &measurementData);
    if (returnCode != CL3IF_RC_OK)
    {
        return -9999;
    }
    else {
        if (index < CL3IF_MAX_OUT_COUNT) {
          LONG dis =  measurementData.outMeasurementData[index].measurementValue;
          double res = dis / 10000.0;
          return res;
        }
    }
}

bool MLKeyenceCL::turnLightControl(bool on, long deviceId)
{
    if (deviceId != -1) {
        LONG returnCode = CL3IF_LightControl(on, deviceId);
        if (returnCode != CL3IF_RC_OK)
        {
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

MLRangeFinder* MLKeyenceCL::MakeRangeFinder(MLKeyenceCLType type)
{
    if(type == KEYENCE_TYPE_UP){
        static MLKeyenceCL selfUp;
        return &selfUp;
    }else{
        static MLKeyenceCL self; 
        return &self;
    }
}

void MLKeyenceCL::subscribe(RFMotionEvent event, RFMotionCallback *callback)
{
    if (event == RFMotionEvent::kStateChanged || event == RFMotionEvent::kPositionChanged)
    {
        if (m_callbacks.count(event) > 0)
        {
            std::list<RFMotionCallback *> callbacks = m_callbacks[event];
            bool found = (std::find(callbacks.begin(), callbacks.end(), callback) != callbacks.end());
            if (!found)
            {
                m_callbacks[event].push_back(callback);
            }
        }
        else
        {
            std::list<RFMotionCallback *> l;
            l.push_back(callback);
            m_callbacks[event] = l;
        }
        return;
    }
}

void MLKeyenceCL::unsubscribe(RFMotionEvent event, RFMotionCallback *callback)
{
    if (m_callbacks.count(event) > 0)
    {
        std::list<RFMotionCallback *> ll = m_callbacks[event];
        ll.remove(callback);
    }
}

void MLKeyenceCL::threadFunc()
{
    bool prevState = false;
    while (m_runThread_)
    {
        bool newState = IsConnected();
        if (prevState != newState)
        {
            if (this->m_callbacks.count(RFMotionEvent::kStateChanged) > 0)
            {
                invokeStatesChangedCB(prevState, newState);
            }
            prevState = newState;
        }

        if (newState)
        {
            notifyPosition();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void MLKeyenceCL::threadStart()
{
    m_thread_ = std::thread(&MLKeyenceCL::threadFunc, this);
    m_runThread_ = true;
}

void MLKeyenceCL::threadStop()
{
    if (m_runThread_) {
        m_runThread_ = false;
        m_thread_.join();
    }
}

void MLKeyenceCL::invokeStatesChangedCB(bool old_state, bool new_state)
{
    std::list<RFMotionCallback *> cb_list = m_callbacks[RFMotionEvent::kStateChanged];
    std::list<RFMotionCallback *>::iterator it;
    for (it = cb_list.begin(); it != cb_list.end(); it++)
    {
        (*it)->NotifyMotionStateChanged(old_state, new_state);
    }
}

void MLKeyenceCL::notifyPosition()
{
    std::list<RFMotionCallback *> cb_list = m_callbacks[RFMotionEvent::kPositionChanged];
    std::list<RFMotionCallback *>::iterator it;
    for (it = cb_list.begin(); it != cb_list.end(); it++)
    {
        (*it)->NotifyMotionPosition(GetPosition());
    }
}
