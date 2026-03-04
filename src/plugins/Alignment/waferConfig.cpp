#include "waferConfig.h"

bool waferConfig::Load(const char* path)
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

std::map<std::string, waferConfigInfo> waferConfig::GetWaferConfigInfo()
{
	if (m_waferConfigInfoMap.size() != 0)
		return m_waferConfigInfoMap;

	auto wafers = m_JsonControl["WaferType"];

	for (auto& it : wafers.items())
	{
		waferConfigInfo waferConfigInfo_;
		waferConfigInfo_.waferName = it.value()["Name"].get<std::string>();
        waferConfigInfo_.dutNum = it.value()["DutNums"].get<int>(); 

        WaferLoadPos waferLoadPos_;
        waferLoadPos_.x = it.value()["LoadPos"]["x"].get<double>();
        waferLoadPos_.y = it.value()["LoadPos"]["y"].get<double>();
        waferLoadPos_.z = it.value()["LoadPos"]["z"].get<double>();

        waferConfigInfo_.waferLoadPos = waferLoadPos_;

		DutScanPos base_dutSacnPos;
		base_dutSacnPos.x = it.value()["FirstDutScanPos"]["x"].get<double>();
		base_dutSacnPos.y = it.value()["FirstDutScanPos"]["y"].get<double>();

		DutFiducialViewPos base_dutFiducialViewPos;
		base_dutFiducialViewPos.x = it.value()["FirstDutFiducialViewPos"]["x"].get<double>();
		base_dutFiducialViewPos.y = it.value()["FirstDutFiducialViewPos"]["y"].get<double>();

        DutRangingPos base_dutRangingPos;
        base_dutRangingPos.x = it.value()["FirstDutRanging"]["x"].get<double>();
        base_dutRangingPos.y = it.value()["FirstDutRanging"]["y"].get<double>();
        base_dutRangingPos.z = it.value()["FirstDutRanging"]["z"].get<double>();

        DutParallelAdjustmentPos base_dutParallelAdjustmentPos;
        base_dutParallelAdjustmentPos.x = it.value()["FirstDutParallelAdjustment"]["x"].get<double>();
        base_dutParallelAdjustmentPos.y = it.value()["FirstDutParallelAdjustment"]["y"].get<double>();
        base_dutParallelAdjustmentPos.z = it.value()["FirstDutParallelAdjustment"]["z"].get<double>();

        if (it.value().contains("DutsOffset"))
        {
            const auto& offsetArray = it.value()["DutsOffset"];

            if (!offsetArray.empty())
            {
                for (const auto& offsetItem : offsetArray)
                {
                    int id = offsetItem.at("id").get<int>();
                    DutScanPos scan_pos;
                    scan_pos.x = base_dutSacnPos.x + offsetItem.at("x").get<double>();
                    scan_pos.y = base_dutSacnPos.y + offsetItem.at("y").get<double>();

                    DutFiducialViewPos fiducialView_pos;
                    fiducialView_pos.x = base_dutFiducialViewPos.x + offsetItem.at("x").get<double>();
                    fiducialView_pos.y = base_dutFiducialViewPos.y + offsetItem.at("y").get<double>();

                    DutRangingPos dutRangingPos;
                    dutRangingPos.x = base_dutRangingPos.x + offsetItem.at("x").get<double>();
                    dutRangingPos.y = base_dutRangingPos.y + offsetItem.at("y").get<double>();
                    dutRangingPos.z = base_dutRangingPos.z;

                    DutParallelAdjustmentPos dutParallelAdjustmentPos;
                    dutParallelAdjustmentPos.x = base_dutParallelAdjustmentPos.x + offsetItem.at("x").get<double>();
                    dutParallelAdjustmentPos.y = base_dutParallelAdjustmentPos.y + offsetItem.at("y").get<double>();
                    dutParallelAdjustmentPos.z = base_dutParallelAdjustmentPos.z;

                    waferConfigInfo_.dutScanPos_map[id] = scan_pos;
                    waferConfigInfo_.dutFiducialViewPos_map[id] = fiducialView_pos;
                    waferConfigInfo_.dutRangingPos_map[id] = dutRangingPos;
                    waferConfigInfo_.dutParallelAdjustmentPos_map[id] = dutParallelAdjustmentPos;
                }
            }        
        }

		m_waferConfigInfoMap[waferConfigInfo_.waferName] = waferConfigInfo_;
	}

	return m_waferConfigInfoMap;
}
