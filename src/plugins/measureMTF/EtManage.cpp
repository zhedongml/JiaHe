#include "MLColorimeterMode.h"
#include "EtManage.h"
#include <QFile>
#include <fstream>

EtManage& EtManage::instance()
{
    static EtManage self;
    return self;
}

EtManage::EtManage()
{
}

EtManage::~EtManage()
{
}

Result EtManage::setting(bool customize, bool save, const QString& dutType)
{
    m_customize = customize;
    m_save = false;
    if(customize && save){
        m_save = true;
    }

    if(!customize || m_save){
        m_fileName = fileByDutType(dutType);

        QFile file(m_fileName);
        file.open(QIODevice::ReadWrite);
        file.close();

        Result ret = loadFile(m_json, m_fileName);
        return ret;
    }

    return Result();
}

Result EtManage::writeData()
{
    if(!m_save){
        return Result();
    }

    if (m_json.is_null()) {
        Result ret = loadFile(m_json, m_fileName);
        if (!ret.success) {
            return ret;
        }
    }

    QString key = getKey();
    m_json[key.toStdString()] = MLColorimeterMode::Instance()->GetExposureTime();

    std::ofstream ofs(m_fileName.toStdString());
    if (!ofs.is_open())
    {
        return Result(false, QString("Update exposure time error, file name %1, update data %2.")
            .arg(m_fileName).arg(key).toStdString());
    }
    ofs << m_json.dump(4);
    ofs.close();
    return Result();
}

Result EtManage::getExposureTime(bool &isAuto, float& et)
{
    if(m_customize){
        return Result();
    }

    std::string key = getKey().toStdString();
    if(m_json.contains(key)){
        isAuto = false;
        et = m_json[key].get<float>();
        return Result();
    }

    return Result(false, QString("Get exposure time error, file name %1, get data %2.")
        .arg(m_fileName).arg(QString::fromStdString(key)).toStdString());
}

bool EtManage::getCustomize()
{
    return m_customize;
}

Result EtManage::loadFile(Json& json, const QString& fileName)
{
    std::ifstream jsonFile(fileName.toStdString());
    if (!jsonFile.is_open())
    {
        return Result(false, QString("Load file error, %1").arg(fileName).toStdString());
    }

    std::string contents = std::string((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
    jsonFile.close();

    if (!contents.empty()) {
        json = Json::parse(contents);
    }
    return Result();
}

QString EtManage::fileByDutType(const QString& dutType)
{
    QString fileName;
    if(dutType.isEmpty()){
        fileName = "ET";
    }else{
        fileName = "ET_" + dutType;
    }

    return QString("%1%2.json").arg(FILE_PATH).arg(fileName);
}

QString EtManage::getKey()
{
    //ImageNameInfo info = MetricsData::instance()->getImageNameInfo();
    //QString key = QString("%1_%2_%3_%4-%5")
    //    .arg(info.deviceColor)
    //    .arg(info.ndFilter)
    //    .arg(info.xyzFilter)
    //    .arg(info.deviceImage)
    //    .arg(info.imgFileName).toUpper();

    //return key;
    return "";
}
