#include "PLCConfig.h"

bool PLCConfig::Load()
{
    m_path = configPath;
    std::ifstream ifs(configPath, std::fstream::in);

    if (ifs.fail())
    {
        return false;
    }

    ifs >> m_JsonControl;
    return true;
}

QMap<int, PLCCell> PLCConfig::ReadInfo()
{
    QMap<int, PLCCell> map;
    auto cells = m_JsonControl["Cells"];
    for (auto& item:cells.items())
    {
        PLCCell cell;
        cell.index = item.value()["Index"].get<int>();
        cell.size = item.value()["BitSize"].get<int>();
        cell.name = QString::fromStdString(item.value()["Name"].get<string>());
        cell.offset = QString::fromStdString(item.value()["Offset"].get<string>());
        cell.on = QString::fromStdString(item.value()["On"].get<string>());
        cell.off = QString::fromStdString(item.value()["Off"].get<string>());
        cell.isDdefault = item.value()["Default"].get<int>();
        cell.readoniy = item.value()["ReadOnly"].get<int>();
        cell.visible = item.value()["Visible"].get<int>();
        map.insert(cell.index,cell);
    }
    return map;
}

ConnectInfo PLCConfig::GetConnectInfo()
{
    ConnectInfo info;
    info.ip = m_JsonControl["Connection"]["IpAddress"].get<std::string>();
    info.rack = m_JsonControl["Connection"]["Rock"].get<int>();
    info.slot = m_JsonControl["Connection"]["Solt"].get<int>();
    return info;
}

bool PLCConfig::GetInterLockDefaultValue()
{
    int interLock = m_JsonControl["InterLock"].get<int>();
    if (interLock)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PLCConfig::getItemOffset(QString &name, int &off_byte, int &off_bit)
{
    QStringList offsets = name.split(".");
    off_byte = offsets.at(0).toInt();
    off_bit = offsets.at(1).toInt();
}
