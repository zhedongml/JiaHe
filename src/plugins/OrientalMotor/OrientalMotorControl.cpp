#include "OrientalMotorControl.h"
#include "MLSpdlog.h"

OrientalMotorControl* OrientalMotorControl::m_instance = nullptr;

OrientalMotorControl::OrientalMotorControl()
{
	Init();
}

OrientalMotorControl::~OrientalMotorControl()
{
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ) {
		if (it->second != nullptr) {
			delete it->second;
		}
		it = m_MotorMap.erase(it);
	}
}

void OrientalMotorControl::Init()
{
	std::map<int, MLOrientalConfiguation> configMap = OrientalMotorConfig::instance()->GetOrientalMotorConfigMap();
	for (auto it = configMap.begin(); it != configMap.end(); ++it) {
		MLSerialManage::instance()->createMLSerial(it->second.port);
		OrientalMotor* motor = new OrientalMotor(it->second);
		m_MotorMap.insert(std::make_pair(it->first, motor));
	}
}

Result OrientalMotorControl::Connect()
{
	Result ret;
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it) {
		MLOrientalResult result = it->second->ML_OpenSerialSync();
		ret.success = result.code;
		ret.errorMsg = result.msg;
		if (!ret.success)
			return ret;
	}
	m_bIsOpen = true;
	return ret;
}

Result OrientalMotorControl::Disconnect()
{
	Result ret;
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it) 
	{
		MLOrientalResult result = it->second->ML_CloseSerial();
		ret.success = result.code;
		ret.errorMsg = result.msg;
		if (!ret.success)
			return ret;
	}
	m_bIsOpen = false;
	return ret;
}

bool OrientalMotorControl::IsConnected()
{
	return m_bIsOpen;

	bool result;
	return true;
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		result = it->second->ML_IsOpen();
		if (!result)
			return result;
	}
	return result;
}

Result OrientalMotorControl::MoveByDegreeAsync(int type,double degree)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return ret;

	ret = ClearAlarm();
	if (!ret.success)
		return ret;

	MLOrientalResult result = m_MotorMap[type]->ML_MoveByDegree(degree);
	return Result(result.code, result.msg);
}

Result OrientalMotorControl::MoveByDegreeSync(int type, double degree)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return ret;

	ret = ClearAlarm();
	if (!ret.success)
		return ret;

	MLOrientalResult result = m_MotorMap[type]->ML_MoveByDegreeSync(degree);
	return Result(result.code, result.msg);
}

Result OrientalMotorControl::StopMove(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return ret;

	MLOrientalResult result = m_MotorMap[type]->ML_StopMove();
	return Result(result.code, result.msg);
}

Result OrientalMotorControl::StopMove()
{
	if (!IsConnected())
		return Result(false, "Motor is not connect.");

	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		MLOrientalResult result = it->second->ML_StopMove();
		if (!result.code)
			return Result(result.code, result.msg);
	}

	return Result();
}

Result OrientalMotorControl::HomingSync(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return ret;

	ret = ClearAlarm();
	if (!ret.success)
		return ret;

	MLOrientalResult result = m_MotorMap[type]->ML_ResetSync();
	return Result(result.code, result.msg);
}

Result OrientalMotorControl::HomingAsync(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return ret;

	ret = ClearAlarm();
	if (!ret.success)
		return ret;

	MLOrientalResult result = m_MotorMap[type]->ML_Reset();
	return Result(result.code, result.msg);
}

Result OrientalMotorControl::HomingAsync()
{
	if (!IsConnected())
		return Result(false, "Motor is not connect.");

	Result ret = ClearAlarm();
	if (!ret.success)
		return ret;

	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		MLOrientalResult result = it->second->ML_Reset();
		if (!result.code)
			return Result(result.code, result.msg);
	}

	return Result();
}

Result OrientalMotorControl::SetSpeed(int pulse)
{
	if (!IsConnected())
		return Result(false, "Motor is not connect.");

	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		MLOrientalResult result = it->second->ML_SetSpeed(pulse);
		if (!result.code)
			return Result(result.code, result.msg);
	}

	return Result();
}

bool OrientalMotorControl::IsHome(int type)
{
	return false;
}

bool OrientalMotorControl::IsMoving(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return false;

	return m_MotorMap[type]->ML_IsMoving();
}

bool OrientalMotorControl::IsMoving()
{
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		Result ret = JudegOpen(it->first);
		if(!ret.success)
			return false;

		if (it->second->ML_IsMoving())
			return true;
	}
	return false;
}

double OrientalMotorControl::GetPosition(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return false;

	ret = ClearAlarm();
	if (!ret.success)
		return DBL_MAX;

	return m_MotorMap[type]->ML_GetCurrDegree();
}

Result OrientalMotorControl::ClearAlarm()
{
	Result ret;
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		MLOrientalResult result = it->second->ML_ClearAlarm();
		ret.success = result.code;
		ret.errorMsg = result.msg;
		if (!ret.success)
			return ret;
	}
	return ret;
}

bool OrientalMotorControl::IsAlarm(int type)
{
	Result ret = JudegOpen(type);
	if (!ret.success)
		return false;

	return m_MotorMap[type]->ML_IsAlarm();
}

bool OrientalMotorControl::IsAlarm()
{
	return false;
	for (auto it = m_MotorMap.begin(); it != m_MotorMap.end(); ++it)
	{
		Result ret = JudegOpen(it->first);
		if (!ret.success)
			return false;

		if (it->second->ML_IsAlarm())
			return true;
	}
	return false;
}

Result OrientalMotorControl::JudegOpen(int type)
{
	if (!m_MotorMap.count(type))
		return Result(false, "Motor map is not contain.");

	if(!m_bIsOpen)
		return Result(false, "Motor is not open.");

	//if (!m_MotorMap[type]->ML_IsOpen())
	//	return Result(false, type + " motor is not open.");

	return Result();
}

OrientalMotorControl* OrientalMotorControl::getInstance()
{
	if (!m_instance)
	{
		static std::mutex mutex;
		std::unique_lock<std::mutex> locker(mutex);
		if (!m_instance)
		{
			m_instance = new OrientalMotorControl();
		}
	}
	return m_instance;
}

