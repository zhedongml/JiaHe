/////////////////////////////////////////////////////////////////////////////
// This is a part of the PI-Software Sources
// (c)2008-2016 Physik Instrumente (PI) GmbH & Co. KG
// All rights reserved.
/////////////////////////////////////////////////////////////////////////////

//#include <windows.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
	#ifdef PI_HYDRAPOLLUX_DLL_EXPORTS
		#define HYDRA_FUNC_DECL __declspec(dllexport) WINAPI
	#else
		#define HYDRA_FUNC_DECL __declspec(dllimport) WINAPI
	#endif
#else
	#define HYDRA_FUNC_DECL __attribute__ ((visibility("default")))
#endif


#ifndef WIN32
	#ifndef BOOL
	#define BOOL int
	#endif

	#ifndef TRUE
	#define TRUE 1
	#endif

	#ifndef FALSE
	#define FALSE 0
	#endif
#endif //WIN32



/////////////////////////////////////////////////////////////////////////////
// DLL initialization and comm functions
int 	HYDRA_FUNC_DECL Hydra_ConnectRS232(int nPortNr, int iBaudRate);
int 	HYDRA_FUNC_DECL ConnectRS232ByDevName(const char* szDescription,int iBaudRate);
int 	HYDRA_FUNC_DECL Hydra_ConnectRS232ByDevName(const char* szDescription,int iBaudRate);
int	HYDRA_FUNC_DECL Hydra_ConnectTCPIP(const char* szHostname, int port);
int	HYDRA_FUNC_DECL Hydra_ConnectTCPIPByDescription(const char* szDescription);

int	HYDRA_FUNC_DECL	Hydra_SetTimeout(int ID, int timeoutInMS);

BOOL	HYDRA_FUNC_DECL Hydra_IsConnected(int ID);
void	HYDRA_FUNC_DECL Hydra_CloseConnection(int ID);
int	HYDRA_FUNC_DECL Hydra_GetError(int ID);
BOOL	HYDRA_FUNC_DECL Hydra_SetErrorCheck(int ID, BOOL bErrorCheck);
BOOL	HYDRA_FUNC_DECL Hydra_TranslateError(int errNr, char* szBuffer, int iBufferSize);

BOOL	HYDRA_FUNC_DECL Hydra_GcsCommandset(int ID, const char* szCommand);
BOOL	HYDRA_FUNC_DECL Hydra_GcsGetAnswer(int ID, char* szAnswer, int iBufferSize);
BOOL	HYDRA_FUNC_DECL Hydra_GcsGetAnswerSize(int ID, int* iAnswerSize);


BOOL	HYDRA_FUNC_DECL Hydra_qSAI(int ID, char* szAxes, int iBufferSize);
BOOL	HYDRA_FUNC_DECL Hydra_qSAI_ALL(int ID, char* szAxes, int iBufferSize);

BOOL	HYDRA_FUNC_DECL Hydra_qERR(int ID, int* pnError);
BOOL	HYDRA_FUNC_DECL Hydra_qIDN(int ID, char* szBuffer, int iBufferSize);
BOOL	HYDRA_FUNC_DECL Hydra_qHLP(int ID, char* szBuffer, int iBufferSize);
BOOL	HYDRA_FUNC_DECL Hydra_qCSV(int ID, double* pdCommandSyntaxVersion);
BOOL	HYDRA_FUNC_DECL Hydra_qHPA(int ID, char* szBuffer, int iBufferSize);

BOOL	HYDRA_FUNC_DECL Hydra_MOV(int ID, const char* szAxes, const double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_qMOV(int ID, const char* szAxes, double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_MVR(int ID, const char* szAxes, const double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_MVE(int ID, const char* szAxes, const double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_POS(int ID, const char* szAxes, const double* pdValueArray);

BOOL	HYDRA_FUNC_DECL Hydra_ACC(int ID, const char* szAxes, const double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_qACC(int ID, const char* szAxes, double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_qDEC(int ID, const char* szAxes, double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_qPOS(int ID, const char* szAxes, double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_VEL(int ID, const char* szAxes, double* pdValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_qVEL(int ID, const char* szAxes, double* pdValueArray);

BOOL	HYDRA_FUNC_DECL Hydra_IsMoving(int ID, const char* szAxes, BOOL* pbValueArray);
BOOL	HYDRA_FUNC_DECL Hydra_HLT(int ID, const char* szAxes);
BOOL	HYDRA_FUNC_DECL Hydra_STP(int ID);
BOOL	HYDRA_FUNC_DECL Hydra_qONT(int ID, const char* szAxes, BOOL* pbValueArray);

BOOL	HYDRA_FUNC_DECL Hydra_qCST(int ID, const char* szAxes, char* szNames, int iBufferSize);
BOOL    HYDRA_FUNC_DECL Hydra_CST(int ID, const char* szAxes, const char* szNames);
BOOL    HYDRA_FUNC_DECL Hydra_qVST(int ID, char* szBuffer, int iBufferSize);

/////////////////////////////////////////////////////////////////////////////
// general

BOOL HYDRA_FUNC_DECL Hydra_RBT(int ID);
BOOL HYDRA_FUNC_DECL Hydra_qVER(int ID, char* szBuffer, int iBufferSize);
BOOL HYDRA_FUNC_DECL Hydra_qTVI(int ID, char* szBuffer, int iBufferSize);
BOOL HYDRA_FUNC_DECL Hydra_IFC(int ID, const char* szParameters, const char* szValues);
BOOL HYDRA_FUNC_DECL Hydra_qIFC(int ID, const char* szParameters, char* szBuffer, int iBufferSize);
BOOL HYDRA_FUNC_DECL Hydra_qIFS(int ID, const char* szParameters, char* szBuffer, int iBufferSize);
//

BOOL HYDRA_FUNC_DECL Hydra_DFH(int ID, const char* szAxes);
BOOL HYDRA_FUNC_DECL Hydra_qDFH(int ID, const char* szAxes, double* pdValueArray);
BOOL HYDRA_FUNC_DECL Hydra_GOH(int ID, const char* szAxes);
//
//
BOOL HYDRA_FUNC_DECL Hydra_SVO(int ID, const char* szAxes, const BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_qSVO(int ID, const char* szAxes, BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_qSMO(int ID, const char* szAxes, int* piValueArray);
//
BOOL HYDRA_FUNC_DECL Hydra_RON(int ID, const char* szAxes, const BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_qRON(int ID, const char* szAxes, BOOL* pbValueArray);

//
BOOL HYDRA_FUNC_DECL Hydra_SPA(int ID, const char* szAxes, const unsigned int* iParameterArray, const double* pdValueArray, const char* szStrings);
BOOL HYDRA_FUNC_DECL Hydra_qSPA(int ID, const char* szAxes, unsigned int* iParameterArray, double* pdValueArray, char* szStrings, int iMaxNameSize);
BOOL HYDRA_FUNC_DECL Hydra_WPA(int ID, const char* szPassword, const char* szAxes, const unsigned int* iParameterArray);

BOOL HYDRA_FUNC_DECL Hydra_SAI(int ID, const char* szOldAxes, const char* szNewAxes);

BOOL HYDRA_FUNC_DECL Hydra_qLIM(int ID, const char* szAxes, BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_qTRS(int ID, const char* szAxes, BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_FNL(int ID, const char* szAxes);
BOOL HYDRA_FUNC_DECL Hydra_FPL(int ID, const char* szAxes);
BOOL HYDRA_FUNC_DECL Hydra_FRF(int ID, const char* szAxes);
BOOL HYDRA_FUNC_DECL Hydra_INI(int ID, const char* szAxes);
BOOL HYDRA_FUNC_DECL Hydra_FED(int ID, const char* szAxes, const int* piEdgeArray, const int* piParamArray);
BOOL HYDRA_FUNC_DECL Hydra_qFRF(int ID, const char* szAxes, BOOL* pbValueArray);
BOOL HYDRA_FUNC_DECL Hydra_DIO(int ID, const int* piChannelsArray, const BOOL* pbValueArray, int iArraySize);
BOOL HYDRA_FUNC_DECL Hydra_qDIO(int ID, const int* piChannelsArray, BOOL* pbValueArray, int iArraySize);
BOOL HYDRA_FUNC_DECL Hydra_qTIO(int ID, int* piInputNr, int* piOutputNr);
BOOL HYDRA_FUNC_DECL Hydra_IsControllerReady(int ID, int* piControllerReady);
BOOL HYDRA_FUNC_DECL Hydra_qSRG(int ID, const char* szAxes, const int* iRegisterArray, int* iValArray);

///////////////////////////////////////////////////////////////////////////////
//// limits
BOOL HYDRA_FUNC_DECL Hydra_qTMN(int ID, const char* szAxes, double* pdValueArray);
BOOL HYDRA_FUNC_DECL Hydra_qTMX(int ID, const char* szAxes, double* pdValueArray);

BOOL	HYDRA_FUNC_DECL Hydra_AddStage(int ID, const char* szAxes);
BOOL	HYDRA_FUNC_DECL	Hydra_RemoveStage(int ID, const char* szStageName);

#ifdef __cplusplus
}
#endif
