#include "dutConfig.h"

bool dutConfig::Load(const char* path)
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

std::map<std::string, dutConfigInfo> dutConfig::GetDutConfigInfo()
{
	if (m_dutConfigInfoMap.size() != 0)
		return m_dutConfigInfoMap;

	auto newDUTs = m_JsonControl["NewDUT"];

	for (auto& it : newDUTs.items())
	{
		dutConfigInfo dutConfigInfo_;
		dutConfigInfo_.dutName = it.value()["name"].get<std::string>();
		dutConfigInfo_.eyeType = it.value()["eyeType"].get<std::string>();
		//dutConfigInfo_.eyeRelief = it.value()["eyeRelief"].get<double>();
		dutConfigInfo_.walkoffDistance = it.value()["walkoffDistance"].get<double>();
		dutConfigInfo_.eyeBoxPupil = it.value()["eyeBoxPupil"].get<double>();
		dutConfigInfo_.fiducialZMotorPos = it.value()["fiducialZMotorPos"].get<double>();
		dutConfigInfo_.originPointMotorPositionInMV_.x = it.value()["originPointMotorPositionInMV"]["x"].get<double>();
		dutConfigInfo_.originPointMotorPositionInMV_.y = it.value()["originPointMotorPositionInMV"]["y"].get<double>();

		auto fiducials = it.value()["fiducialOffset"];
		int fidId = 1;
		for (auto& it : fiducials.items())
		{
			offsetPos offset;
			//int id = it.value()["id"].get<int>();
			offset.x = it.value()["x"].get<double>();
			offset.y = it.value()["y"].get<double>();
			dutConfigInfo_.fiducialOffset_[fidId] = offset;
			fidId++;
		}

		dutConfigInfo_.inputgratingOffset_.x = it.value()["inputgratingOffset"]["x"].get<double>();
		dutConfigInfo_.inputgratingOffset_.y = it.value()["inputgratingOffset"]["y"].get<double>();
		dutConfigInfo_.QROffset_.x = it.value()["QROffset"]["x"].get<double>();
		dutConfigInfo_.QROffset_.y = it.value()["QROffset"]["y"].get<double>();

		auto outputOffsets = it.value()["outputgratingOffset"];
		for (auto& it : outputOffsets.items())
		{
			offsetPos3D offset;
			int id = it.value()["id"].get<int>();
			offset.x = it.value()["x"].get<double>();
			offset.y = it.value()["y"].get<double>();
			offset.z = it.value()["eyeRelief"].get<double>();
			dutConfigInfo_.outputgratingOffset_[id] = offset;
		}

		m_dutConfigInfoMap[dutConfigInfo_.dutName] = dutConfigInfo_;
	}

	return m_dutConfigInfoMap;
}
