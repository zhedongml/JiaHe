#ifndef CORE_RANGEFINDER_H_
#define CORE_RANGEFINDER_H_

#include <exception>

class MLRangeFinderError : public std::exception
{
public:
    MLRangeFinderError() : std::exception("ML range finder error!") {}
    MLRangeFinderError(const char* msg) : std::exception(msg) {}
};

class RFMotionCallback
{
  public:
    virtual void NotifyMotionStateChanged(bool old_state, bool new_state) = 0;
    virtual void NotifyMotionPosition(double position) = 0;
};

typedef enum RFMotionEvent
{
    kStateChanged,
    kPositionChanged
};

class MLRangeFinder
{
public:
    virtual ~MLRangeFinder(){}
    virtual bool Connect(const char* type_name,int id =0) = 0;
    virtual bool Disconnect() = 0;
    virtual bool IsConnected() = 0;
    virtual double GetPosition(int index = 0) = 0;
    virtual bool turnLightControl(bool on = true, long deviceId = 0) {
        return true;
    };

    virtual void subscribe(RFMotionEvent event, RFMotionCallback *callback) = 0;
    virtual void unsubscribe(RFMotionEvent event, RFMotionCallback *callback) = 0;
    virtual void threadStop() {}
};

#endif // !CORE_RANGEFINDER_H_