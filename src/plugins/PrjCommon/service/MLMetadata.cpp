#include "MLMetadata.h"

namespace CORE
{
MLMetaData::MLMetaData()
{
}

MLMetaData::~MLMetaData()
{
}



void MLMetaData::SetCaptureData(std::string channel,CaptureData  data)
{
	if (channel == "X"|| channel == "Y"|| channel == "Z")
	{
		m_CaptureData.frameList[channel] = data;

        m_CaptureData.datatime = CurrentDatetimeToString();
	}
}

void MLMetaData::ClearCaptureData()
{
    m_CaptureData.frameList.clear();

    m_CaptureData.datatime = "";
}

bool MLMetaData::onlyYChannel()
{
    if (m_CaptureData.frameList.size() == 1 && m_CaptureData.frameList.count("Y"))
        return true;
    else
        return false;
}


void MLMetaData::SetColorShiftData(ColorShiftData data)
{
    m_ColorShift = data;

    m_ColorShift.datatime = CurrentDatetimeToString();
}

void MLMetaData::SetPostProcessData(PostProcessData data)
{
    m_PostProcess = data;

    m_PostProcess.datatime = CurrentDatetimeToString();
}

void MLMetaData::SetDistortionData(DistortionData data)
{
    m_Distortion = data;

    m_Distortion.datatime = CurrentDatetimeToString();
}

void MLMetaData::SetFFCData(FFCData data)
{
    m_FFC = data;

    m_FFC.datatime = CurrentDatetimeToString();
}

void MLMetaData::SetFourColorData(FourColorData data)
{
    m_FourColor = data;

    m_FourColor.datatime = CurrentDatetimeToString();
}

void MLMetaData::SetBinningData(BinningData data)
{
    m_Binning = data;

    m_Binning.datatime = CurrentDatetimeToString();
}



std::string CurrentDatetimeToString()
{
    time_t time = NULL;

    struct tm* tm_ = localtime(&time);               
    int year, month, day, hour, minute, second;
    year = tm_->tm_year + 1900;               
    month = tm_->tm_mon + 1;                   
    day = tm_->tm_mday;                        
    hour = tm_->tm_hour;                       
    minute = tm_->tm_min;                      
    second = tm_->tm_sec;                      
    char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];
    sprintf(yearStr, "%d", year);              
    sprintf(monthStr, "%d", month);            
    sprintf(dayStr, "%d", day);                
    sprintf(hourStr, "%d", hour);              
    sprintf(minuteStr, "%d", minute);          
    if (minuteStr[1] == '\0')                  
    {
        minuteStr[2] = '\0';
        minuteStr[1] = minuteStr[0];
        minuteStr[0] = '0';
    }
    sprintf(secondStr, "%d", second);          
    if (secondStr[1] == '\0')                  
    {
        secondStr[2] = '\0';
        secondStr[1] = secondStr[0];
        secondStr[0] = '0';
    }
    char s[20];                                
    sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);
    std::string str(s);                            
    return str;                                
}
}