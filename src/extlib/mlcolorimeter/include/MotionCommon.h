// Copyright 2023 MLOptic

#ifndef LOGIC_MOTION_PIMOTIONCONTROLLER2_PIMOTIONCOMMON_H_
#define LOGIC_MOTION_PIMOTIONCONTROLLER2_PIMOTIONCOMMON_H_

namespace ML {
namespace Motion {
enum MLMotionState {
    kMLStatusStationary = 4,

    kMLStatusIsMoving = 3,

    kMLStatusConnected = 2,

    /** @brief operate successfully */
    kMLStatusOk = 1,

    /** @brief operation failure */
    kMLStatusFailed = 0,

    kMLStatusDisConnected = -1,

    /** @brief unknown error */
    kMLStatusUnknow = -2,
};

enum MLMotionEvent { kStateChanged, kPositionChanged };

class MLMotionCallback {
 public:
    virtual void NotifyMotionStateChanged(MLMotionState old_state,
                                          MLMotionState new_state) = 0;
    virtual void NotifyMotionPosition(double position) = 0;
};
}  // namespace Motion
}


#endif  // LOGIC_MOTION_PIMOTIONCONTROLLER2_PIMOTIONCOMMON_H_
