#pragma once
#include "PrjCommon/service/ml_rangefinder.h"
#include <map>
#include <list>
#include <iostream>
#include <thread>
#include <mutex>

#ifdef MLKEYENCELRF_EXPORTS
#define MLRANGEFINDER_API __declspec(dllexport)
#else
#define MLRANGEFINDER_API __declspec(dllimport)
#endif

enum MLKeyenceCLType{
    KEYENCE_TYPE_UP,
    KEYENCE_TYPE_DOWN
};

class MLRANGEFINDER_API MLKeyenceCL :public MLRangeFinder
{
public:
    MLKeyenceCL();
    ~MLKeyenceCL();

    /*
    * Connect the Keyence sensor.
    *
    * @param type Keyence connection type. Currently, only "USB" is supported.
    * @return true if connected successfully, otherwise false.
    */
    bool Connect(const char* type,int id = 0);

    /*
    * Disconnect the keyence sensor
    *
    * @return true if the sensor is disconnected successfully, otherwise false
    */
    bool Disconnect();

    /*
    * Get current Keyence sensor state.
    *
    * @return true if the sensor is in connected state; otherwise false.
    */
    bool IsConnected();

    /*
    * Get the position reading from the Keyence sensor.
    *
    * @param index the sensor index, starting from 0
    * @return the position value if the sensor reading is within the measurement range, or "inf" otherwise.
    */
    double GetPosition(int index = 0);

    /*
    *This function turns on or turns off the measurement light
    */
    bool turnLightControl(bool on = true, long deviceId = 0);
    /*
    * Make a MLRangeFinder instance.
    *
    * A factory method to create an MLRangeFinder instance.
    */
    static MLRangeFinder* MakeRangeFinder(MLKeyenceCLType type = KEYENCE_TYPE_DOWN);

    void subscribe(RFMotionEvent event, RFMotionCallback *callback) override;
    void unsubscribe(RFMotionEvent event, RFMotionCallback *callback) override;
    void threadStop();

  private:
    void threadFunc();
    void threadStart();
    void invokeStatesChangedCB(bool old_state, bool new_state);
    void notifyPosition();

  private:
    bool is_connected_ = false;
    long deviceId = -1;

    std::map<RFMotionEvent, std::list<RFMotionCallback *>> m_callbacks;
    std::thread m_thread_;
    std::atomic<bool> m_runThread_;
    std::string m_ip;
};

