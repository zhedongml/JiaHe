#include "MLSystemWrapper.h"
#include <QtCore/QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
//#include "LoggingWrapper.h"
void MLSystemWrapper::Invoke(QString cmd, QString param)
{
	if (cmd == "WaitForUser")
	{
		if (param == "/session/get_dut_info")
		{
			emit SendStatusSignal(CORE::Wrapper_Session_Get_DUT_Info, "Start");
		}
		else if (param == "/session/start")
		{
			emit SendStatusSignal(CORE::Wrapper_Session_start, "Start");
		}
		else if (param == "/session/stop")
		{
			emit SendStatusSignal(CORE::Wrapper_Session_stop, "Start");
		}
	}
	else if (cmd == "LoadDutConfiguration")
	{

	}
	else if (cmd == "DUTType")
	{
		if (param == "/DPA")
		{
			emit SelectDUTTypeSignal(0);
			emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
		}
		else if (param == "/WaveGuide")
		{
			emit SelectDUTTypeSignal(1);
			emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
		}
		else if (param == "/FullSystem")
		{
			emit SelectDUTTypeSignal(2);
			emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
		}
		else {
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Error_fatal, "no such DUT type");
		}

	}
	else if (cmd == "Sleep")
	{
		Result ret;
		QStringList list = param.split("/");
		for(QString paramstr: list)
		{
			QStringList paramPair = paramstr.split("=");
			if (paramPair.size() == 2)
			{
				QString key = paramPair[0].trimmed();
				QString val = paramPair[1].trimmed();
				if (key == "Millisecond") {
					QFuture<void> future = QtConcurrent::run(this, &MLSystemWrapper::delay, val.toInt());
				}
			}
		}
	}
	else if (cmd == "Save")
	{
		QStringList p = param.split("_");
		if (p.size() != 2)
		{
            emit SendStatusSignal(CORE::Wrapper_Error_fatal, "Invalid folder parameter.Did not save data");
		}
		else
		{
			QString pathfolder = QString::fromStdString("./"/*m_fileConfig.outputDir*/) + p[1];
			if (!QDir().exists(pathfolder)) 
			{ 				
				if (!QDir().mkdir(pathfolder))
				{
					pathfolder = QDir::homePath();
					//LoggingWrapper::getinstance()->debug("Invalid path. Saving to home directory");
				}
			}
			emit SendStatusSignal(CORE::Wrapper_Session_save, pathfolder);
		}
	}
	else if (cmd == "InternalLight")
	{
		if (param == "on" || param == "off")
		{
			emit SendInternalLightSignal(param);
			std::string msg = "Internal Light turned " + param.toStdString() + ".";
			emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, QString::fromStdString(msg));
		}
		else 
		{
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Error_fatal,
                                  "Please provide a param of either on or off");
		}
    }
    else
    {
        m_Status = CORE::Wrapper_Error_fatal;
        emit SendStatusSignal(m_Status, QString("\"%1\" cmd is mismatch, please check the execution fields.").arg(cmd));
    }
}

MLSystemWrapper::~MLSystemWrapper(){}

MLSystemWrapper::MLSystemWrapper(QObject* parent):QObject(parent)
{ 
	//m_fileConfig=FileConfig::GetInstance().ReadInfo(); 
}


CORE::WrapperStatus MLSystemWrapper::GetStatus()
{
	return m_Status;
}

void MLSystemWrapper::delay(int ms)
{
	QThread::msleep(ms);
	emit SendStatusSignal(CORE::Wrapper_Done, "");
}
