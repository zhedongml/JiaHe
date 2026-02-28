#include "ELLOLib.h"
#include <msclr/gcroot.h>
#include <msclr/marshal_cppstd.h>
//#include <windows.h>

#using "Thorlabs.Elliptec.ELLO_DLL.dll"
using namespace Thorlabs::Elliptec::ELLO_DLL;
using namespace msclr;
using namespace System;
using namespace System::Collections::Generic;
using namespace msclr::interop;

using namespace CORE;

class ELLOMOTORImpl
{
public:
    ELLOMOTORImpl();

    ~ELLOMOTORImpl();

    bool Connect(const char* type_name);

    bool Disconnect();

    bool IsConnected();

    void SetEnable(bool enable);

    bool IsEnabled();

    void MoveRelAsync(int step);

    void MoveAbsAsync(int pos);

    void MoveRel(int step);

    void MoveAbs(int pos);

    void StopMove();

    void Homing();

    bool IsHome();

    void SetSpeed(int speed);

    int GetSpeed();

    bool IsMoving();

    int GetPosition();

    MLMotionState GetState();

    void Subscribe(MLMotionEvent event, CoreMotionCallback* callback);

    void Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback);
private:
    gcroot<ELLDevices^> m_ellDevices;
    gcroot<ELLDevice^> m_addressedDevice;
    gcroot<ELLDevicePort^> m_deviceport;
    bool isConnected = false;
};

ELLOMOTORImpl::ELLOMOTORImpl()
{
    m_ellDevices = gcnew ELLDevices();
    m_deviceport = gcnew ELLDevicePort();
}

ELLOMOTORImpl::~ELLOMOTORImpl()
{
}

bool  ELLOMOTORImpl::Connect(const char* type_name)
{
    if (IsConnected()) return true;

    String^ address = gcnew String(type_name);

    bool ret = m_deviceport->Connect(address);
    if (ret)
    {
        m_ellDevices->SetDevicePort(m_deviceport);
        auto res = m_ellDevices->ScanAddresses('0', 'F');

        if (res->Count == 0)
            return false;

        if (m_ellDevices->Configure(res[0]))
        {
            m_ellDevices->Connect();

            m_addressedDevice =
                safe_cast<ELLDevice^> (m_ellDevices->AddressedDevice(PtrToStringChars(res[0])[0]));

            m_addressedDevice->SetDevicePort(m_deviceport);
            m_addressedDevice->SetDerDevicePort(m_deviceport);

            Homing();
        }

        Homing();
    }
    isConnected = true;
    return ret;
}

bool  ELLOMOTORImpl::Disconnect()
{
    bool ret = true;

    if (IsConnected())
        ret = m_deviceport->Disconnect();

    isConnected = false;
    return true;
}

bool  ELLOMOTORImpl::IsConnected()
{
    //return m_deviceport->IsConnected;
    return isConnected;
}

void ELLOMOTORImpl::SetEnable(bool enable)
{
}

bool ELLOMOTORImpl::IsEnabled()
{
    return false;
}

void ELLOMOTORImpl::MoveRelAsync(int step)
{
    double pst = step / 1000.f;
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy())
        m_addressedDevice->MoveRelative(Convert::ToDecimal(pst));
}

void ELLOMOTORImpl::MoveAbsAsync(int pos)
{
    double pst = pos / 1000.f;
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy())
        m_addressedDevice->MoveAbsolute(Convert::ToDecimal(pst));
}

void ELLOMOTORImpl::MoveRel(int step)
{
    double pst = step / 1000.f;
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy())
        m_addressedDevice->MoveRelative(Convert::ToDecimal(pst));

    while (true)
    {
        if (!m_addressedDevice->IsDeviceBusy())
            break;

        Sleep(500);
    }
}

void ELLOMOTORImpl::MoveAbs(int pos)
{
    double pst = pos / 1000.f;
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy())
        m_addressedDevice->MoveAbsolute(Convert::ToDecimal(pst));

    while (true)
    {
        if (!m_addressedDevice->IsDeviceBusy())
            break;

        Sleep(500);
    }
}

void ELLOMOTORImpl::StopMove()
{
}

void ELLOMOTORImpl::Homing()
{
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy())
        m_addressedDevice->Home(ELLBaseDevice::DeviceDirection::Linear);
}

bool ELLOMOTORImpl::IsHome()
{
    return false;
}

void ELLOMOTORImpl::SetSpeed(int speed)
{
    auto res = m_addressedDevice->DeviceInfo;
    bool retJug1 = m_addressedDevice->SetPeriod(L'1', true, Convert::ToDecimal(speed), true, true);
    bool retNeg1 = m_addressedDevice->SetPeriod(L'1', false, Convert::ToDecimal(speed), true, true);
    bool retJug2 = m_addressedDevice->SetPeriod(L'2', true, Convert::ToDecimal(speed), true, true);
    bool retNeg2 = m_addressedDevice->SetPeriod(L'2', false, Convert::ToDecimal(speed), true, true);
}

int ELLOMOTORImpl::GetSpeed()
{
    bool ret = m_addressedDevice->SearchPeriod(L'1', true);
    return 0;
}

bool ELLOMOTORImpl::IsMoving()
{
    if (m_deviceport->IsConnected && m_addressedDevice->IsDeviceBusy())
        return true;
    return false;
}

bool IsMoving()
{
    return false;
}

int ELLOMOTORImpl::GetPosition()
{
    int pos = 0;
    if (m_deviceport->IsConnected && !m_addressedDevice->IsDeviceBusy() && m_addressedDevice->GetPosition())
    {
        pos = Convert::ToDouble(m_addressedDevice->Position) * 1000;
    }

    return pos;
}

MLMotionState ELLOMOTORImpl::GetState()
{
    return MLMotionState();
}

void ELLOMOTORImpl::Subscribe(MLMotionEvent event, CoreMotionCallback* callback)
{

}

void ELLOMOTORImpl::Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback)
{

}

//ELLOMOTOR
ELLOMOTOR::ELLOMOTOR() : m_implELLOMOTOR{ std::make_unique<ELLOMOTORImpl>() }
{
}

ELLOMOTOR::~ELLOMOTOR()
{
}

Result ELLOMOTOR::Connect(const char* type_name)
{
    try
    {
        Result ret;
        ret.success = m_implELLOMOTOR->Connect(type_name);
        return ret;
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Connect exception: " + ex->Message);
        return Result(false, "Connect exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

bool ELLOMOTOR::Disconnect()
{
    return m_implELLOMOTOR->Disconnect();
}

bool ELLOMOTOR::IsConnected()
{
    return m_implELLOMOTOR->IsConnected();
}

void ELLOMOTOR::SetEnable(bool enable)
{
    m_implELLOMOTOR->SetEnable(enable);
}

bool ELLOMOTOR::IsEnabled()
{
    return m_implELLOMOTOR->IsEnabled();
}

Result ELLOMOTOR::MoveRelAsync(int step)
{
    try
    {
        m_implELLOMOTOR->MoveRelAsync(step);
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Move relative async exception: " + ex->Message);
        return Result(false, "Move relative async exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

Result ELLOMOTOR::MoveAbsAsync(int pos)
{
    try
    {
        m_implELLOMOTOR->MoveAbsAsync(pos);
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Move absolute async exception: " + ex->Message);
        return Result(false, "Move absolute async exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

Result ELLOMOTOR::MoveRel(int step)
{
    try
    {
        m_implELLOMOTOR->MoveRel(step);
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Move relative sync exception: " + ex->Message);
        return Result(false, "Move relative sync exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

Result ELLOMOTOR::MoveAbs(int pos)
{
    try
    {
        m_implELLOMOTOR->MoveAbs(pos);
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Move absolute sync exception: " + ex->Message);
        return Result(false, "Move absolute sync exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

Result ELLOMOTOR::StopMove()
{
    try
    {
        m_implELLOMOTOR->StopMove();
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Stop move exception: " + ex->Message);
        return Result(false, "Stop move exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

Result ELLOMOTOR::Homing()
{
    try
    {
        m_implELLOMOTOR->Homing();
        return Result();
    }
    catch (System::Exception^ ex)
    {
        System::Console::WriteLine("Homing exception: " + ex->Message);
        return Result(false, "Homing exception: " + std::string(msclr::interop::marshal_as<std::string>(ex->Message)));
    }
}

bool ELLOMOTOR::IsHome()
{
    return m_implELLOMOTOR->IsHome();
}

void ELLOMOTOR::SetSpeed(int speed)
{
    m_implELLOMOTOR->SetSpeed(speed);
}

int ELLOMOTOR::GetSpeed()
{
    return m_implELLOMOTOR->GetSpeed();
}

bool ELLOMOTOR::IsMoving()
{
    return m_implELLOMOTOR->IsMoving();
}

int ELLOMOTOR::GetPosition()
{
    return m_implELLOMOTOR->GetPosition();
}

MLMotionState ELLOMOTOR::GetState()
{
    return m_implELLOMOTOR->GetState();
}

void ELLOMOTOR::Subscribe(MLMotionEvent event, CoreMotionCallback* callback)
{
    m_implELLOMOTOR->Subscribe(event, callback);
}

void ELLOMOTOR::Unsubscribe(MLMotionEvent event, CoreMotionCallback* callback)
{
    m_implELLOMOTOR->Unsubscribe(event, callback);
}

bool ELLOMOTOR::ClearAlarm()
{
    return false;
}
