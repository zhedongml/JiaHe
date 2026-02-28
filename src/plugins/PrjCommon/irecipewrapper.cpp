#include "irecipewrapper.h"

IRecipeWrapper::IRecipeWrapper(QObject *parent)
	: QObject(parent)
{
	
}

IRecipeWrapper::~IRecipeWrapper()
{
}

void IRecipeWrapper::setSignalConnected(bool connected)
{
	signalConnected = connected;
}

bool IRecipeWrapper::getSignalConnected()
{
	return signalConnected;
}

void IRecipeWrapper::trigerSinal(CORE::WrapperStatus status, QString msg)
{
	emit  SendStatusSignal(status, msg);
}

void IRecipeWrapper::notifyStop(bool isstop)
{
	isStop = isstop;
}

void IRecipeWrapper::notifyPause(bool isPause)
{
	m_isPause = isPause;
}

bool IRecipeWrapper::getIsStop()
{
	return isStop;
}

bool IRecipeWrapper::getIsPause()
{
	return m_isPause;
}
