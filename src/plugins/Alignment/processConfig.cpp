#include "processConfig.h"

bool processConfig::Load(const char* path)
{
	m_path = path;
	std::ifstream ifs(path, std::fstream::in);

	if (ifs.fail())
	{
		return false;
	}

	ifs >> m_JsonControl;
	return true;
}

processConfigInfo processConfig::GetProcessConfigInfo()
{
	/*m_processConfigInfo.loadPos.dutModule.x = m_JsonControl["Load"]["DutModule"]["X"].get<double>();
	m_processConfigInfo.loadPos.dutModule.y = m_JsonControl["Load"]["DutModule"]["Y"].get<double>();
	m_processConfigInfo.loadPos.dutModule.z = m_JsonControl["Load"]["DutModule"]["Z"].get<double>();*/

	/*m_processConfigInfo.parallelAdjustmentPos.adjustPos.x = m_JsonControl["DutParallelAdjustment"]["X"].get<double>();
	m_processConfigInfo.parallelAdjustmentPos.adjustPos.y = m_JsonControl["DutParallelAdjustment"]["Y"].get<double>();
	m_processConfigInfo.parallelAdjustmentPos.adjustPos.z = m_JsonControl["DutParallelAdjustment"]["Z"].get<double>();*/

	/*m_processConfigInfo.rangingPos.rangingPos.x = m_JsonControl["Ranging"]["X"].get<double>();
	m_processConfigInfo.rangingPos.rangingPos.y = m_JsonControl["Ranging"]["Y"].get<double>();
	m_processConfigInfo.rangingPos.rangingPos.z = m_JsonControl["Ranging"]["Z"].get<double>();*/

	auto offsetRotate = m_JsonControl["OffsetRoatate"];

	for (auto& it : offsetRotate.items())
	{
		std::string dutName = it.value()["name"].get<std::string>();
		m_processConfigInfo.offsetRoatate.projectionTiptilt[dutName].x = it.value()["projection"]["projectionTiptilt"]["dX"].get<double>();
		m_processConfigInfo.offsetRoatate.projectionTiptilt[dutName].y = it.value()["projection"]["projectionTiptilt"]["dY"].get<double>();
		m_processConfigInfo.offsetRoatate.projectionOffsetRelativeToMV[dutName].x = it.value()["projection"]["projectionOffsetRelativeToMV"]["offsetX"].get<double>();
		m_processConfigInfo.offsetRoatate.projectionOffsetRelativeToMV[dutName].y = it.value()["projection"]["projectionOffsetRelativeToMV"]["offsetY"].get<double>();

		/*	m_processConfigInfo.offsetRoatate.initImagingMeasurePos[dutName].x = it.value()["imaging"]["InitMeasurePos"]["X"].get<double>();
			m_processConfigInfo.offsetRoatate.initImagingMeasurePos[dutName].y = it.value()["imaging"]["InitMeasurePos"]["Y"].get<double>();
			m_processConfigInfo.offsetRoatate.initImagingMeasurePos[dutName].z = it.value()["imaging"]["InitMeasurePos"]["Z"].get<double>();*/
		m_processConfigInfo.offsetRoatate.imagingTiptilt[dutName].x = it.value()["imaging"]["imagingTiptilt"]["dX"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingTiptilt[dutName].y = it.value()["imaging"]["imagingTiptilt"]["dY"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingOffsetRelativeToMV[dutName].x = it.value()["imaging"]["imagingOffsetRelativeToMV"]["offsetX"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingOffsetRelativeToMV[dutName].y = it.value()["imaging"]["imagingOffsetRelativeToMV"]["offsetY"].get<double>();
		m_processConfigInfo.offsetRoatate.eyeBoxCenterKeyenceValue[dutName] = it.value()["imaging"]["eyeBoxCenterKeyenceValue"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingFixedPos[dutName].x = it.value()["imaging"]["fixedPos"]["X"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingFixedPos[dutName].y = it.value()["imaging"]["fixedPos"]["Y"].get<double>();
		m_processConfigInfo.offsetRoatate.imagingFixedPos[dutName].z = it.value()["imaging"]["fixedPos"]["Z"].get<double>();

		m_processConfigInfo.offsetRoatate.anticollision[dutName].dutMotorAngleMax = it.value()["Anticollision"]["dutMotorAngleMax"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].dutMotorAngleMin = it.value()["Anticollision"]["dutMotorAngleMin"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].inputAlignmentMotorPosXMax = it.value()["Anticollision"]["inputAlignmentMotorPosXMax"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].inputAlignmentMotorPosXMin = it.value()["Anticollision"]["inputAlignmentMotorPosXMin"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].inputAlignmentMotorPosYMax = it.value()["Anticollision"]["inputAlignmentMotorPosYMax"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].inputAlignmentMotorPosYMin = it.value()["Anticollision"]["inputAlignmentMotorPosYMin"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].rangingMax = it.value()["Anticollision"]["rangingMax"].get<double>();
		m_processConfigInfo.offsetRoatate.anticollision[dutName].rangingMin = it.value()["Anticollision"]["rangingMin"].get<double>();

		m_processConfigInfo.offsetRoatate.loadPos[dutName].dutModule.x = it.value()["LoadDut"]["X"].get<double>();
		m_processConfigInfo.offsetRoatate.loadPos[dutName].dutModule.y = it.value()["LoadDut"]["Y"].get<double>();
		m_processConfigInfo.offsetRoatate.loadPos[dutName].dutModule.z = it.value()["LoadDut"]["Z"].get<double>();

		m_processConfigInfo.offsetRoatate.rangingPos[dutName].rangingPos.x = it.value()["Ranging"]["X"].get<double>();
		m_processConfigInfo.offsetRoatate.rangingPos[dutName].rangingPos.y = it.value()["Ranging"]["Y"].get<double>();
		m_processConfigInfo.offsetRoatate.rangingPos[dutName].rangingPos.z = it.value()["Ranging"]["Z"].get<double>();

		m_processConfigInfo.offsetRoatate.parallelAdjustmentPos[dutName].adjustPos.x = it.value()["DutParallelAdjustment"]["X"].get<double>();
		m_processConfigInfo.offsetRoatate.parallelAdjustmentPos[dutName].adjustPos.y = it.value()["DutParallelAdjustment"]["Y"].get<double>();
		m_processConfigInfo.offsetRoatate.parallelAdjustmentPos[dutName].adjustPos.z = it.value()["DutParallelAdjustment"]["Z"].get<double>();
	}

	return m_processConfigInfo;
}