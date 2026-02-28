#ifndef ML_KEYENCELRF_H_
#define ML_KEYENCELRF_H_


#include "PrjCommon/service/ml_rangefinder.h"

#ifdef MLKEYENCELRF_EXPORTS
#define MLRANGEFINDER_API __declspec(dllexport)
#else
#define MLRANGEFINDER_API __declspec(dllimport)
#endif

/*
* Implementation of MLRangeFinder using Keyence sensor
*/
class MLRANGEFINDER_API MLKeyenceLRF : public MLRangeFinder
{
public:
    MLKeyenceLRF();
    ~MLKeyenceLRF();

public:
    /*
    * Connect the Keyence sensor.
    * 
    * @param type Keyence connection type. Currently, only "USB" is supported.
    * @return true if connected successfully, otherwise false.
    */
    bool Connect(const char* type,int id=0);

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
    * Make a MLRangeFinder instance.
    * 
    * A factory method to create an MLRangeFinder instance.
    */
    static MLRangeFinder* MakeRangeFinder();

    void subscribe(RFMotionEvent event, RFMotionCallback *callback) override;
    void unsubscribe(RFMotionEvent event, RFMotionCallback *callback) override;

private:
    bool is_connected_ = false;
};

#endif // !ML_KEYENCELRF_H_
