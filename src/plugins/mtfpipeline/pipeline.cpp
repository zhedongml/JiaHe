#include "pipeline.h"
#include "jsonparser.h"
ML_MTF_TYPE PipeLine::roiType = ML_MTF_TYPE::CROSS;
PipeLine::PipeLine()
{
	processors.push_back(new PreProcessor());
	processors.push_back(new FitProcessor());   
	processors.push_back(new OverSampleProcessor());
	processors.push_back(new ESFProcessor());
	processors.push_back(new LSFProcessor());
	processors.push_back(new DFTProcessor());
	processors.push_back(new MTFPostProcessor());
	pipeLineInput = new PipeLineInput();
	pipeLineOutput = new PipeLineOutPut();
}

PipeLine::~PipeLine()
{
	for (vector<AbstractProcessor*>::iterator it = processors.begin(); it != processors.end(); it++) {
		delete* it;
	}
	delete pipeLineInput;
	freeoutputParmMemory(pipeLineOutput);
	delete pipeLineOutput;
}
bool PipeLine::configProcessor(string configFilePath)
{
	JsonParser* jp = JsonParser::getInstance();
	return jp->loadConfig(configFilePath);
}
bool PipeLine::registProcessor(AbstractProcessor* processor,int index)
{
	std::vector<AbstractProcessor*>::iterator it=processors.begin();
    processors.insert(it+index, processor);
	return true;
}

void PipeLine::setROIType(ML_MTF_TYPE type)
{
	roiType = type;
}

int PipeLine::culc_mtf(cv::Mat &roi, ML_MTF_TYPE type)
{
	freeoutputParmMemory(pipeLineOutput);
	pipeLineOutput->errNum = 0;
	pipeLineInput->roi = roi;
    if (type == SLANT)
    {
		for (auto it = processors.cbegin(); it != processors.cend(); it++) {
			if (!(*it)->processEdge(pipeLineInput, pipeLineOutput))
			{
				break;
			}
		}
	}
	else {
		std::vector<AbstractProcessor*> processors2 = processors;
		swap(processors2[3], processors2[4]);
		for (auto it = processors2.cbegin(); it != processors2.cend(); it++) {
			if (!(*it)->processCross(pipeLineInput, pipeLineOutput)) {
				break;
			}
		}
	}
	if (pipeLineOutput->errNum == 0) {
		if (pipeLineOutput->direction == VERTICAL || pipeLineOutput->direction == HORIZONTAL)
		{
			double* value = pipeLineOutput->Mtf;
			double* freq = pipeLineOutput->Freq;
			int len = pipeLineOutput->bin_len;
			for (auto iter = value; iter < value + len; iter++)
			{
				(*iter) = (*iter) * 100.0;
				//qDebug() << (*iter)<<":"<<endl;
			}

			for (auto it = freq; it != freq + len; ++it)
			{
				*it = *it * m_bUnit;
			}
		}
	}
	else 
		return -1;

	return pipeLineOutput->direction;
}

int PipeLine::culc_mtf(cv::Mat roi)
{
	//return culc_mtf(roi,roiType);
    return culc_mtf(roi, CROSS);
}

void PipeLine::freeoutputParmMemory(PipeLineOutPut* pipeLineOutput)	
{
	if (pipeLineOutput->Esf_Lsf_x != nullptr) {
		free(pipeLineOutput->Esf_Lsf_x);
		pipeLineOutput->Esf_Lsf_x = nullptr;
	}
	if (pipeLineOutput->Esf != nullptr) {
		free(pipeLineOutput->Esf);
		pipeLineOutput->Esf = nullptr;
	}
	if (pipeLineOutput->Lsf != nullptr) {
		free(pipeLineOutput->Lsf);
		pipeLineOutput->Lsf = nullptr;
	}
	if (pipeLineOutput->Freq != nullptr) {
		free(pipeLineOutput->Freq);
		pipeLineOutput->Freq = nullptr;
	}
	if (pipeLineOutput->Mtf != nullptr) {
		free(pipeLineOutput->Mtf);
		pipeLineOutput->Mtf = nullptr;
	}
	if (pipeLineOutput->freq_interp != nullptr) {
		free(pipeLineOutput->freq_interp);
		pipeLineOutput->freq_interp = nullptr;
	}
	if (pipeLineOutput->mtf_interp != nullptr) {
		free(pipeLineOutput->mtf_interp);
		pipeLineOutput->mtf_interp = nullptr;
	}
}

void PipeLine::freeInputParmMemory(PipeLineInput* pipeLineInput)
{
}

double* PipeLine::get_freq()
{
	return pipeLineOutput->Freq;
}

double* PipeLine::get_mtf()
{
	return pipeLineOutput->Mtf;
}

double* PipeLine::get_esf_lsf_x()
{
	return pipeLineOutput->Esf_Lsf_x;
}

double* PipeLine::get_esf()
{
	return pipeLineOutput->Esf;
}

double* PipeLine::get_lsf()
{
	return pipeLineOutput->Lsf;
}

int PipeLine::get_bin_len()
{
	return pipeLineOutput->bin_len;
}

void PipeLine::SetPixelValue(double value)
{
	SlantedEdge::SetSlantedPixel(value);
	SlantedCross::SetCrossPixel(value);
}

void PipeLine::SetBinning(int binning)
{
    SlantedEdge::SetBinning(binning);
    SlantedCross::SetBinning(binning);
}

void PipeLine::set_freq_unit(ML_FREQ_UNIT unit, double focalLength, double coefficient)
{
	m_Unit = unit;

	if (unit == lp_deg)
	{
		m_bUnit = focalLength * CV_PI / 180.0 * coefficient;
	}
	else
	{
		m_bUnit = 1.0;
	}

}
double PipeLine::getMtfByFreq(double x)
{
	if ((pipeLineOutput->Freq == NULL) || (pipeLineOutput->Mtf == NULL)) return 0;
	SplineSpace::Spline sp(pipeLineOutput->Freq, pipeLineOutput->Mtf, pipeLineOutput->bin_len,
		SplineSpace::GivenSecondOrder, 0, 0);
	double mtf_need;
	sp.SinglePointInterp(x, mtf_need);
	return mtf_need;

}
double* PipeLine::get_freq_interp()
{
	return pipeLineOutput->freq_interp;
}

double* PipeLine::get_mtf_interp()
{
	return pipeLineOutput->mtf_interp;
}

int PipeLine::get_len_interp()
{
	return pipeLineOutput->len_interp;
}

void PipeLine::set_mtf_interp(int interpNum)
{

	if (pipeLineOutput->Freq != NULL && pipeLineOutput->Mtf != NULL)
	{
		pipeLineOutput->len_interp = (pipeLineOutput->bin_len - 1) * interpNum + 1;
		pipeLineOutput->freq_interp = (double*)malloc(pipeLineOutput->len_interp * sizeof(double));
		pipeLineOutput->mtf_interp = (double*)malloc(pipeLineOutput->len_interp * sizeof(double));

		double interpVal = (pipeLineOutput->Freq[1] - pipeLineOutput->Freq[0]) / double(interpNum);
		for (int i = 0; i < pipeLineOutput->len_interp; i++)
		{
			pipeLineOutput->freq_interp[i] = pipeLineOutput->Freq[i / interpNum] + double(i % interpNum) * interpVal;
		}


		//MTF²åÖµ
		SplineSpace::Spline sp(pipeLineOutput->Freq, pipeLineOutput->Mtf, pipeLineOutput->bin_len, SplineSpace::GivenSecondOrder, 0, 0);
		sp.MultiPointInterp(pipeLineOutput->freq_interp, pipeLineOutput->len_interp, pipeLineOutput->mtf_interp);

	}

}

