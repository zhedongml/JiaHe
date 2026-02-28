#include "Helper.h"
#include <map>

std::string GetScanRoutineName (FDRScan FDR, FDGScan FDG);
std::string GetRunningScanRoutineName (int ID, FDRScan FDR, FDGScan FDG);
std::string GetRunningScanRoutineName (int ID, std::vector <std::string> ScanNames);

std::string GetDRR (int ID, std::string ScanRoutineName);
void GetResults (int ID, std::vector<std::string> ScanRoutineNames);
std::vector<std::string> GetScans (int ID, FDRScans FDRs, FDGScans FDGs);
BOOL IsRoutineRunning (int ID, std::string ScanRoutineName);

void RoutinesRunningResults (int ID, FDRScan FDR, FDGScan FDG);
void RoutinesRunningStop (int ID, std::vector<std::string> ScanRoutineNames);
void RoutinesRunningSavedata (int ID, std::string ScanRoutineName);
void RoutinesRunningShowdata (int ID, std::string ScanRoutineName);

void ShowScans (int ID, FDRScans FDRs, FDGScans FDGs);

void MoveAxesMOV (int ID, std::map <std::string, double> AxesPositions);

BOOL ConnectUSB (int* pID);
BOOL FADefineAreaScanRoutine (int ID
                              ,const char* ScanRoutineName
                              ,char* axis
                              ,const double ScanAxisRange
                              ,const char* StepAxis
                              ,const double StepAxisRange
                              ,const char* Parameters);


BOOL FADefineGradientScanRoutine (int ID
                                  ,const char* ScanRoutineName
                                  ,char* axis
                                  ,const char* StepAxis
                                  ,const char* Parameters);

BOOL FAStartsRoutine ( int ID ,const char* ScanRoutineName );

BOOL FAGetRoutineResults (int ID
                          ,const char* ScanRoutineName
                          ,const unsigned int ResultId
                          ,char* Result
                          ,int BufferSize);


BOOL FAGetRoutineState (int ID ,const char* ScanRoutineName, int* OptionsArray);

BOOL SetDataRecorderTableTriggerSource (int ID
                                        ,const int* RecordTableIdsArray
                                        ,int* TriggerSourceArray
                                        ,char* Values
                                        ,int ArraySize);

BOOL SetDataRecorderConfiguration (int ID
                                   ,const int* RecordTableIdsArray
                                   ,char* RecordSourceIds
                                   ,int* RecordOptionsArray);

BOOL ReadDataRecordTables ( int ID
                           ,const int* RecordTableIdsArray
                           ,int NumberOfRecTables
                           ,int OffsetOfFirstPointInRecordTable
                           ,int MinNumberOfValues
                           ,double** ValueArray
                           ,char* GcsArrayHeader
                           ,int GcsArrayHeaderMaxSize );

BOOL FAGetInputChannelValue (int ID
                             ,const int* FastAlignmentInputIdsArray
                             ,double* CalculatedInputValueArray
                             ,int ArraySize);


std::string ReadControllerName (int ID);
std::string ReadControllerAlias (int ID);
void GetGCSCommandSet (int ID, std::string Command, std::string& Answer);

std::string GetError (int ID);

