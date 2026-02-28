#include "pch.h"
#include "MLKeyenceLRF.h"
#include <string>
#include <algorithm>
#include "LKIF2.h"

MLKeyenceLRF::MLKeyenceLRF() : is_connected_(false)
{
    // TODO: logging - debug
}

MLKeyenceLRF::~MLKeyenceLRF()
{
    // TODO: logging - debug
}

bool MLKeyenceLRF::Connect(const char* type,int id)
{
    std::string s = type;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c);  });
    RC rc;

    if (s == "usb")
    {
        // TODO: logging - debug
        rc = LKIF2_OpenDeviceUsb();
    }
    else
    {
        // TODO: logging - debug
        throw MLRangeFinderError("unknown device type");
    }

    if (rc == RC_OK)
    {
        is_connected_ = true;
    }
    else
    {
        is_connected_ = false;
    }

    return is_connected_;
}

bool MLKeyenceLRF::Disconnect()
{
    // TODO: logging - debug
    RC rc = LKIF2_CloseDevice();
    is_connected_ = false;
    return is_connected_;
}

bool MLKeyenceLRF::IsConnected()
{
    // TODO: logging - debug
    return is_connected_;
}

double MLKeyenceLRF::GetPosition(int index)
{
    if (!is_connected_) {
        return 0.0;
    }
    LKIF_FLOATVALUE_OUT value;
    RC rc = LKIF2_GetCalcDataSingle(index, &value);
    // TODO: logging - debug

    if (rc == RC_OK)
    {
        return value.Value;
    }
    else if (rc == RC_ERR_NO_DEVICE)
    {
        // TODO: logging - debug
        throw MLRangeFinderError("Error no device.");
    }
    else if (rc == RC_ERR_RECEIVE)
    {
        // TODO: logging -debug
        throw MLRangeFinderError("Error receiving data.");
    }
    else
    {
        // TODO: logging - debug
        throw MLRangeFinderError("Error unknown.");
    }
}

MLRangeFinder* MLKeyenceLRF::MakeRangeFinder()
{
    return new MLKeyenceLRF();
}

void MLKeyenceLRF::subscribe(RFMotionEvent event, RFMotionCallback *callback)
{
}

void MLKeyenceLRF::unsubscribe(RFMotionEvent event, RFMotionCallback *callback)
{
}
