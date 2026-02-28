// Copyright 2023 MLOptic
#include "OrientalMotor.h"

#include "MLSpdlog.h"

void OrientalMotor::Subscribe() {}
void OrientalMotor::UnSubscribe() {}

OrientalMotor::OrientalMotor(const MLOrientalConfiguation& config):m_config(config)
{
}

OrientalMotor::~OrientalMotor()
{
    if (_callback != nullptr)
        delete _callback;
}

MLOrientalResult OrientalMotor::ML_OpenSerial(
    MLOrientalMotorCallback* cb) {
    MLOrientalResult ret;
    try {
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->InitSerialPort(m_config.serial_info);
        if (m_config.MLIOCommand_enable) {
            m_command = m_config.MLIOCommand;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->Open(m_config.port);
        if (rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_bIsOpen = true;
            MLSerialManage::instance()->setConnection(m_config.port);
            m_status = MLOrientalStatus::MLOrientalStatus_OK;
            ret.code = rt;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_OpenSerial successfully";
            ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
            if (_callback != nullptr) {
                m_status = MLOrientalStatus::
                    MLOrientalStatus_Connecting;
                ret.status = m_status;
                _callback->NotifyOrientalStatusChanged(m_config.name, m_status);
            }
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = rt;
        m_status = MLOrientalStatus::MLOrientalStatus_Error;
        ret.code = rt;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_OpenSerial failed";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = false;
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_OpenSerial exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_OpenSerialSync() {
    MLOrientalResult ret;
    try {
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->InitSerialPort(m_config.serial_info);
        if (m_config.MLIOCommand_enable) {
            m_command = m_config.MLIOCommand;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->Open(m_config.port);

        // wait open finish
        if (rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_bIsOpen = true;
            MLSerialManage::instance()->setConnection(m_config.port);
            m_status = MLOrientalStatus::MLOrientalStatus_OK;
            ret.code = rt;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_OpenSerialSync successfully";
            ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = rt;
        m_status = MLOrientalStatus::MLOrientalStatus_Error;
        ret.code = rt;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_OpenSerialSync failed";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = false;
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_OpenSerialSync exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_CloseSerial() {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_CloseSerial error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()->closeConnection(m_config.port);
        int connection =
            MLSerialManage::instance()->getConnection(m_config.port);
        if (connection == 0) {
            bool rt =
                MLSerialManage::instance()->getMLSerial(m_config.port)->Close();
            if (rt) {
                std::lock_guard<std::mutex> locker(_status_mutex);
                m_bIsOpen = false;
                m_status =
                    MLOrientalStatus::MLOrientalStatus_Close;
                ret.code = rt;
                ret.status = m_status;
                ret.msg = _msg + " Oriental ML_CloseSerial successfully";
                ML::MLSpdlog::instance()->info("OrientalMotor: " +
                                               ret.msg);
                return ret;
            }
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = rt;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_CloseSerial error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = false;
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_CloseSerial not all drives turn off," +
                  " serial port cannot be turned off";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_bIsOpen = false;
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_CloseSerial exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

bool OrientalMotor::ML_IsOpen() {
    try {
        bool isOpen =
            MLSerialManage::instance()->getMLSerial(m_config.port)->IsOpen() &&
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->CheckSerial(m_config.station) &&
            m_bIsOpen;
        return isOpen;
    } catch (const std::exception&) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return false;
    }
}

bool OrientalMotor::ML_IsMoving() {
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return false;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (filter && MLSerialManage::instance()
        //                   ->getMLSerial(m_config.port)
        //                   ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     return false;
        // }
        // if (axis && MLSerialManage::instance()
        //                 ->getMLSerial(m_config.port)
        //                 ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     return false;
        // }
        bool isMoving = MLSerialManage::instance()
                           ->getMLSerial(m_config.port)
                           ->IsMoving(m_config.station);
        if (!isMoving) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status =
                MLOrientalStatus::MLOrientalStatus_Stationary;
            return false;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Moving;
        return true;
    } catch (const std::exception&) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return false;
    }
}

bool OrientalMotor::ML_IsAlarm() {
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return false;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        bool isAlarm = MLSerialManage::instance()
                           ->getMLSerial(m_config.port)
                           ->IsAlarm(m_config.station);

        if (!isAlarm) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_OK;
            return false;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        return true;
    } catch (const std::exception&) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return false;
    }
}

MLOrientalResult OrientalMotor::ML_StopMove() {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_StopMove error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (filter && MLSerialManage::instance()
        //                   ->getMLSerial(m_config.port)
        //                   ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_StopMove status: filter alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        // if (axis && MLSerialManage::instance()
        //                 ->getMLSerial(m_config.port)
        //                 ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_StopMove status: axis alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        bool rt = MLSerialManage::instance()
                     ->getMLSerial(m_config.port)
                     ->StopMove(m_config.station);
        if (rt && !ML_IsMoving()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status =
                MLOrientalStatus::MLOrientalStatus_Stationary;
            ret.code = true;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_StopMove successfully";
            ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Error;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_StopMove error";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_StopMove exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_Reset(
    MLOrientalMotorCallback* cb) {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_ResetAxis error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_ResetAxis alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->Reset(m_config.station);
        if (!rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_ResetAxis error, move failed";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        if (_callback != nullptr) {
            m_status = MLOrientalStatus::MLOrientalStatus_Moving;
            _callback->NotifyOrientalStatusChanged(
                m_config.name,
                MLOrientalStatus::MLOrientalStatus_Moving);
        }
        ret.code = rt;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_ResetAxis successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_ResetAxis exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_ResetSync() {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_ResetAxisSync error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_ResetAxisSync alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->Reset(m_config.station);
        // wait for reset finish
        if (!rt || !ML_WaitForFinish(30000).code) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_ResetAxisSync error, move failed";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_ResetAxisSync successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_ResetAxisSync exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_MoveByDegree(
    double degree, MLOrientalMotorCallback* cb) {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxis error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_MoveByAxis alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        if (degree > m_config.Max || degree < m_config.Min) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxis error, axis out of range";
            ML::MLSpdlog::instance()->warn("OrientalMotor: " + ret.msg);
            return ret;
        }
        // wait for movement finish
        int pulse = static_cast<int>(
            static_cast<double>(m_config.PulsePerCycle / 360.0) *
            degree);
        int origin_pulse =
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->GetIndexPosition(m_config.station, m_config.Init_Index);
        if (m_config.Init_Index == 0) {
            origin_pulse = 0;
        }
        bool rt =
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->MoveByPulse(m_config.station, pulse + origin_pulse,
                              m_config.Move_Index);
        if (!rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxis move axis error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        if (_callback != nullptr) {
            m_status = MLOrientalStatus::MLOrientalStatus_Moving;
            _callback->NotifyOrientalStatusChanged(
                m_config.name,
                MLOrientalStatus::MLOrientalStatus_Moving);
        }
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxis successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxis exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_MoveByDegreeSync(
    double degree) {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxisSync error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_MoveByAxisSync alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        if (degree > m_config.Max || degree < m_config.Min) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg =
                _msg + " Oriental ML_MoveByAxisSync error, axis out of range";
            ML::MLSpdlog::instance()->warn("OrientalMotor: " + ret.msg);
            return ret;
        }
        int pulse = static_cast<int>(
            static_cast<double>(m_config.PulsePerCycle / 360.0) *
            degree);
        int origin_pulse =
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->GetIndexPosition(m_config.station, m_config.Init_Index);
        if (m_config.Init_Index == 0) {
            origin_pulse = 0;
        }
        bool rt =
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->MoveByPulse(m_config.station, pulse + origin_pulse,
                              m_config.Move_Index);
        if (!rt || !ML_WaitForFinish(30000).code) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxisSync move axis error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
		if (std::abs(ML_GetCurrDegree() - degree) > 0.01) {
			std::lock_guard<std::mutex> locker(_status_mutex);
			m_status = MLOrientalStatus::MLOrientalStatus_Error;
			ret.code = false;
			ret.status = m_status;
			ret.msg = _msg + " Oriental ML_MoveByAxisSync move to axis to " +
				std::to_string(degree) + " degree failed";
			ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
			return ret;
		}
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxisSync successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxisSync exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_MoveByDegreeSync1(
    double degree) {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxisSync error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        if (degree > m_config.Max || degree < m_config.Min) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg =
                _msg + " Oriental ML_MoveByAxisSync error, axis out of range";
            ML::MLSpdlog::instance()->warn("OrientalMotor: " + ret.msg);
            return ret;
        }
        int pulse = static_cast<int>(
            static_cast<double>(m_config.PulsePerCycle / 360.0) *
            degree);

        bool rt =
            MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->MoveByPulse(m_config.station, pulse,
                m_config.Move_Index);
        if (!rt || !ML_WaitForFinish(30000).code) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxisSync move axis error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        if (ML_GetCurrDegree() != degree) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByAxisSync move to axis to " +
                std::to_string(degree) + " degree failed";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxisSync successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    }
    catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByAxisSync exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

double OrientalMotor::ML_GetCurrDegree()
{
    try {
        std::lock_guard<std::mutex> locker(_status_mutex);
        if (!ML_IsOpen()) {
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        if (MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->IsAlarm(m_config.station)) {
            m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
            return INT_MAX;
        }
        int pulse = MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->GetCurrPosition(m_config.station);
        if (pulse == INT_MAX) {
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        double deg = pulse * 360.0 / m_config.PulsePerCycle;
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        return deg;
    }
    catch (const std::exception&) {
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return INT_MAX;
    }
}

MLOrientalResult
OrientalMotor::ML_MoveByPulse(
    int pulse, MLOrientalMotorCallback* cb) {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByPulse error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_MoveByPulse alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        if (pulse > m_config.Max * m_config.PulsePerCycle ||
            pulse < m_config.Min * m_config.PulsePerCycle) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg =
                _msg + " Oriental ML_MoveByPulse error, pulse out of range";
            ML::MLSpdlog::instance()->warn("OrientalMotor: " + ret.msg);
            return ret;
        }
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->MoveByPulse(m_config.station, pulse,
                                    m_config.Move_Index);
        if (!rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByPulse move axis error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByPulse successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByPulse exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult
OrientalMotor::ML_MoveByPulseSync(int pulse) {
	MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByPulseSync error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     ret.code = false;
        //     ret.status = m_status;
        //     ret.msg = _msg + " Oriental ML_MoveByPulseSync alarm";
        //     ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        //     return ret;
        // }
        if (pulse > m_config.Max * m_config.PulsePerCycle ||
            pulse < m_config.Min * m_config.PulsePerCycle) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg =
                _msg + " Oriental ML_MoveByPulseSync error, pulse out of range";
            ML::MLSpdlog::instance()->warn("OrientalMotor: " + ret.msg);
            return ret;
        }
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->MoveByPulse(m_config.station, pulse,
                                    m_config.Move_Index);
        if (!rt || !ML_WaitForFinish(30000).code) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByPulseSync move axis error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        if (ML_GetCurrPulse() != pulse) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_MoveByPulseSync move to " +
                      std::to_string(pulse) + " pulse failed";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByPulseSync successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_MoveByPulseSync exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

double OrientalMotor::ML_GetCurrDegree1() {
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        //     return INT_MAX;
        // }
        int pulse = MLSerialManage::instance()
                        ->getMLSerial(m_config.port)
                        ->GetCurrPosition(m_config.station);
        if (pulse == INT_MAX) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        int origin_index = m_config.Init_Index;
        int origin_pulse =
            MLSerialManage::instance()
                ->getMLSerial(m_config.port)
                ->GetIndexPosition(m_config.station, origin_index);
        if (origin_index == 0) {
            origin_pulse = 0;
        }
        double pos = static_cast<double>(pulse - origin_pulse) * 360.0 /
                     static_cast<double>(m_config.PulsePerCycle);
        double degree = static_cast<int>((pos >= 0) ? (floor(pos + 0.5))
                                                 : (ceil(pos - 0.5)));
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        return degree;
    } catch (const std::exception&) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return INT_MAX;
    }
}

int OrientalMotor::ML_GetCurrPulse() {
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        // if (MLSerialManage::instance()
        //         ->getMLSerial(m_config.port)
        //         ->IsAlarm(m_config.axis_info.station)) {
        //     std::lock_guard<std::mutex> locker(_status_mutex);
        //     m_status = ML::MLFilterWheel::MLFilterStatus::MLFilterStatus_Alarm;
        //     return INT_MAX;
        // }
        int pulse = MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->GetCurrPosition(m_config.station);
        if (pulse == INT_MAX) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        return pulse;
    }
    catch (const std::exception&) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return INT_MAX;
    }
}

MLOrientalStatus OrientalMotor::ML_GetStatus() {
    std::lock_guard<std::mutex> locker(_status_mutex);
    return m_status;
}

MLOrientalResult OrientalMotor::ML_WaitForFinish(
    int timeout) {
    MLOrientalResult ret;
    try {
        int count = 0;
        if (ML_IsOpen()) {
            while (ML_IsMoving()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                count++;
                if (count * 200 > timeout) {
                    std::lock_guard<std::mutex> locker(_status_mutex);
                    m_status = MLOrientalStatus::
                        MLOrientalStatus_Timeout;
                    ret.code = false;
                    ret.status = m_status;
                    ret.msg = _msg + " Oriental ML_WaitForFinish timeout";
                    ML::MLSpdlog::instance()->warn("OrientalMotor: " +
                                                   ret.msg);
                    return ret;
                }
            }
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status =
                MLOrientalStatus::MLOrientalStatus_Stationary;
            ret.code = true;
            ret.status = m_status;
            ret.msg = _msg + " Oriental ML_WaitForFinish successfully";
            ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Error;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_WaitForFinish error, not opened";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_WaitForFinish exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult OrientalMotor::ML_WaitResetFinish(
    int timeout) {
    MLOrientalResult ret;
    try {
        int count = 0;
        if (ML_IsOpen()) {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                count++;
                if (count * 200 > timeout) {
                    std::lock_guard<std::mutex> locker(_status_mutex);
                    m_status = MLOrientalStatus::
                        MLOrientalStatus_Timeout;
                    ret.code = false;
                    ret.status = m_status;
                    ret.msg = _msg + " Oriental ML_WaitResetFinish timeout";
                    ML::MLSpdlog::instance()->warn("OrientalMotor: " +
                                                   ret.msg);
                    return ret;
                }
                MLSerialManage::instance()
                    ->getMLSerial(m_config.port)
                    ->SetMLIOCommand(m_command);
                MLSerialManage::instance()
                    ->getMLSerial(m_config.port)
                    ->SetMotorType(m_config.motorType);
                if (MLSerialManage::instance()
                        ->getMLSerial(m_config.port)
                        ->GetCurrIndex(m_config.station) == 0) {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(1500));
                    if (MLSerialManage::instance()
                            ->getMLSerial(m_config.port)
                            ->GetCurrIndex(m_config.station) == 0) {
                        std::lock_guard<std::mutex> locker(_status_mutex);
                        m_status = MLOrientalStatus::
                            MLOrientalStatus_OK;
                        ret.code = true;
                        ret.status = m_status;
                        ret.msg =
                            _msg + " Oriental ML_WaitResetFinish successfully";
                        ML::MLSpdlog::instance()->info("OrientalMotor: " +
                                                       ret.msg);
                        return ret;
                    }
                }
            }
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Error;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_WaitResetFinish error, not opened";
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " Oriental ML_WaitResetFinish exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

MLOrientalResult
OrientalMotor::ML_ClearAlarm() {
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " ML_ClearAlarm error, not opened";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        bool rt = MLSerialManage::instance()
                      ->getMLSerial(m_config.port)
                      ->ClearAlarm(m_config.station);
        if (!rt) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " MLSerial ClearAlarm error";
            ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
            return ret;
        }
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_OK;
        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " ML_ClearAlarm successfully";
        ML::MLSpdlog::instance()->info("OrientalMotor: " + ret.msg);
        return ret;
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " ML_ClearAlarm exception: " + e.what();
        ML::MLSpdlog::instance()->error("OrientalMotor: " + ret.msg);
        return ret;
    }
}

int OrientalMotor::ML_ReadAlarmCode() {
    try {
        if (!ML_IsOpen()) {
            std::lock_guard<std::mutex> locker(_status_mutex);
            m_status = MLOrientalStatus::MLOrientalStatus_Error;
            return INT_MAX;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
        return INT_MAX;
    } catch (const std::exception) {
        std::lock_guard<std::mutex> locker(_status_mutex);
        m_status =
            MLOrientalStatus::MLOrientalStatus_SerialException;
        return INT_MAX;
    }
}

MLOrientalResult OrientalMotor::ML_SetSpeed(int pulse)
{
    MLOrientalResult ret;
    try {
        if (!ML_IsOpen()) {
            m_status =
                MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " ML_SetSpeed error, not opened";
            return ret;
        }
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMLIOCommand(m_command);
        MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetMotorType(m_config.motorType);
        if (MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->IsAlarm(m_config.station)) {
            m_status = MLOrientalStatus::MLOrientalStatus_Alarm;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " ML_SetSpeed alarm";
            return ret;
        }
        bool rt = MLSerialManage::instance()
            ->getMLSerial(m_config.port)
            ->SetSpeed(m_config.station, pulse);
        if (!rt) {
            m_status =
                MLOrientalStatus::MLOrientalStatus_Error;
            ret.code = false;
            ret.status = m_status;
            ret.msg = _msg + " ML_SetSpeed error, set speed failed";
            return ret;
        }
        m_status = MLOrientalStatus::MLOrientalStatus_OK;

        ret.code = true;
        ret.status = m_status;
        ret.msg = _msg + " ML_SetSpeed successfully";
        return ret;
    }
    catch (const std::exception& e) {
        m_bIsOpen = false;
        m_status = MLOrientalStatus::MLOrientalStatus_SerialException;
        ret.code = false;
        ret.status = m_status;
        ret.msg = _msg + " ML_SetSpeed exception: " + e.what();
        return ret;
    }
}