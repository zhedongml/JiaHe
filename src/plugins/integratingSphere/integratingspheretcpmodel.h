#pragma once

#include <windows.h>
#include <QObject>
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <QTcpSocket>
#include <QTimer>
#include"lsapi.h"
#include "PrjCommon/service/ml.h"
#include "PrjCommon/service/ml_motion.h"
#include "integratingsphere_global.h"
#include <QProcess>

using namespace CORE;
using Json = nlohmann::json;
struct ISSolution
{
    int index;
	QString name = "";
	float scale;
    QString colorFocus = "";
    QList<float> currentOutputs;

	ISSolution()
    {
    }
    ISSolution(int indexT, QString nameT, double scaleT, QString colorFocusT)
    {
        index = indexT;
        name = nameT;
        scale = scaleT;
        colorFocus = colorFocusT;
    }

    ISSolution(int indexT, QString nameT, double scaleT, QString colorFocusT, QList<float> currentOutputsT)
    {
        index = indexT;
        name = nameT;
        scale = scaleT;
        colorFocus = colorFocusT;
        currentOutputs = currentOutputsT;
    }

    ISSolution(int indexT, double scaleT)
    {
        index = indexT;
        scale = scaleT;
    }

    bool isEqual(ISSolution sol)
    {
        if (index != sol.index)
        {
            return false;
        }
        if (!qFuzzyCompare(scale, sol.scale))
        {
            return false;
        }

        if (sol.currentOutputs.size() != currentOutputs.size())
        {
            return false;
        }

        for (int i = 0; i < currentOutputs.size(); ++i){
            if (!qFuzzyCompare(sol.currentOutputs[i], currentOutputs[i]))
            {
                return false;
            }
        }
        return true;
    }
};

class INTEGRATINGSPHERE_EXPORT IntegratingSphereTCPModel : public QObject
{
	Q_OBJECT

public:
	static  IntegratingSphereTCPModel* getInstance(QObject* parent = nullptr);
	~IntegratingSphereTCPModel();
	QVector<ISSolution> getSolutions();
	bool changeSolution(QString name, double scale);
	int queryCurSolution(double& scale);
	bool isConnect();
	bool closeSoftware();
	bool turnColor(QString color);

    Result Connect();
	void Disconnect();
    bool isOpened();

    Result SetColorIlluminate(const QString &enColor, double fValue);
    Result SetColorIlluminate(Color enColor, double fValue);

	double GetColorIlluminate(Color enColor);
	double GetColorCurrent(Color enColor);
	double GetColorVoltage(Color enColor);

	Result setCurrentOutput(const QString &enColor, float fValue);
    Result setCurrentOutput(const QString &enColor, QMap<QString, float> valueMap);
    Result setCurrentOutput(QMap<QString, float> valueMap);

    Result setAllCurrentOutputZero();

    Result setSolution(const ISSolution &solution);
    Result setSolution(const QString &name);

    Result setFan(bool open);

    Result connectDetect(lsapi_device_DeviceInfo deviceInfo);
    Result getLuminance(float& luminance, Color color = W);

  public:
    double RCalibrationFactor = 0.0;
	double GCalibrationFactor = 0.0;
    double BCalibrationFactor = 0.0;

signals:
	void reponseMsg(QString msg);
    void connectStatus(bool isOpened, QString msg);
    void refreshData(float currentR, float currentG, float currentB);

private:
	IntegratingSphereTCPModel(QObject* parent);
	static IntegratingSphereTCPModel* self;
	QString  writeData(QString data, QString cmd);
	void loadconfig(std::string filename);

	Color switchColorEnum(const QString &enColor);
    void updateUpper(QMap<QString, float> &map);
    bool isEqual(QMap<QString, float> valueMap);

    Color getColor(QMap<QString, float> valueMap);

private:
	bool GetDeviceInfo(lsapi_DeviceHandleT hDevice);
	bool LoadDll();
	bool OpenAPI();
	bool SetCurrentOutput(lsapi_DeviceHandleT g_hDevice,double fValue);
	bool SetVoltageOutput(lsapi_DeviceHandleT g_hDevice, double fValue);
	double GetCurrentOutput(lsapi_DeviceHandleT g_hDevice);
	//void SetVoltageOutput(lsapi_DeviceHandleT g_hDevice, double fValue);
	double GetVoltageOutput(lsapi_DeviceHandleT g_hDevice);
	bool TurnOnLamp(lsapi_DeviceHandleT g_hDevice);
	bool TurnOffLamp(lsapi_DeviceHandleT g_hDevice);

	bool ControlOutputEnable(lsapi_DeviceHandleT hDevice, bool bEnable);
	typedef lsapi_ResultT(_stdcall* PFN_lsapi_ApiCall)(lsapi_ApiCallHdr* pHdr);
	PFN_lsapi_ApiCall g_pfnApiCall = nullptr;
	// Global variable for an LS API "device handle"
	lsapi_DeviceHandleT g_hDevice[4] = { lsapi_INVALID_HANDLE_VALUE,lsapi_INVALID_HANDLE_VALUE,lsapi_INVALID_HANDLE_VALUE,lsapi_INVALID_HANDLE_VALUE };
	
    //lsapi_ApiCall_Device_CatalogDevices_Params catParams;
	lsapi_ApiCall_Device_CatalogDeviceClass_Params catParams;
    lsapi_DeviceHandleT m_hDeviceFan[1] = {lsapi_INVALID_HANDLE_VALUE};

    HMODULE hDll = nullptr;

private:
    QString serverIp = "localhost";
    int port = 3434;
    QTcpSocket *_socket = nullptr;
    bool isSend = false;
    QVector<ISSolution> solutionsVec;
    QTimer m_timer;

    bool bIsconnect = false;

    int m_diviceIndexR = 0;
    int m_diviceIndexG = 1;
    int m_diviceIndexB = 2;
    int m_diviceIndexDetect = 3;

	float m_whiteRatioR = 0.0;
    float m_whiteRatioG = 0.0;
    float m_whiteRatioB = 0.0;

	const double VR = 92.3; // voltage
    const double VG = 74.7;
    const double VB = 84.5;

    Color m_lastColor = W;
};

