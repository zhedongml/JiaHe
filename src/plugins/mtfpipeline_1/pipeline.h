#pragma once
#include "abstractprocessor.h"
#include "dftprocessor.h"
#include "esfprocessor.h"
#include "fitprocessor.h"
#include "lsfprocessor.h"
#include "mftpostprocessor.h"
#include "preprocessor.h"
#include "oversampleprocessor.h"
#include "SlantedEdge.h"
#include "SlantedCross.h"

typedef enum MTF_TYPE
{
	CROSS = 0,

	SLANT = 1,

	SPOT = 2

}ML_MTF_TYPE;
typedef enum FREQ_UNIT
{
	lp_mm = 0,

	lp_deg = 1

}ML_FREQ_UNIT;

class __declspec(dllexport) PipeLine
{
private:
	std::vector<AbstractProcessor*> processors;
	PipeLineInput* pipeLineInput = nullptr;
	PipeLineOutPut* pipeLineOutput = nullptr;
	double m_bUnit = 1.0;
    ML_FREQ_UNIT m_Unit = lp_mm;
    static ML_MTF_TYPE roiType;

  public:
	PipeLine();
	~PipeLine();
	bool configProcessor(string configFilePath);
	bool registProcessor(AbstractProcessor* processor, int index);
    static void setROIType(ML_MTF_TYPE type);
    int culc_mtf(cv::Mat &roi, ML_MTF_TYPE type);
	int culc_mtf(cv::Mat roi);
	void freeoutputParmMemory(PipeLineOutPut* pipeLineOutput);
	//void alloctInputParmMemory(PipeLineInput* pipeLineInput);
	void freeInputParmMemory(PipeLineInput* pipeLineInput);

	double* get_freq();

	double* get_mtf();

	double* get_esf_lsf_x();

	double* get_esf();

	double* get_lsf();

	int get_bin_len();
	void SetPixelValue(double value);
    void SetBinning(int binning);
    void set_freq_unit(ML_FREQ_UNIT unit, double focalLength, double coefficient = 1.0);
	double getMtfByFreq(double x);

	double* get_freq_interp();

	double* get_mtf_interp();

	int get_len_interp();
	void set_mtf_interp(int interpNum);
};

