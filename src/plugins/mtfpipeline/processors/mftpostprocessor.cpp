#include "mftpostprocessor.h"
#include "../ml_math/SlantedCross.h"
#include "../ml_math/SlantedEdge.h"
bool MTFPostProcessorConfig::anglecorrect = true;
bool MTFPostProcessorConfig::smoothcorrect = true;
bool MTFPostProcessorConfig::ccdcorrect = true;
bool MTFPostProcessorConfig::derivationcorrect = true;
bool MTFPostProcessorConfig::mtfsyscorrect = true;

bool MTFPostProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	return true;
}
bool MTFPostProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int len =pipeLineOutPut->bin_len;
	int lsf_len = 2 * len;
	int half_window = 2;//平滑窗口大小
	int diff_window = 3;//微分窗口大小
	double sfrc;
	double* aveEdgeTmp = pipelineInput->overSample.aveEdgeCopy;
	double theta = fabs(atan(pipeLineOutPut->slope));
	double edgePixel = SlantedEdge::getSlantedPixel();
	if (pipeLineOutPut->Freq == NULL)
	{
		pipeLineOutPut->Freq = (double*)malloc(len * sizeof(double));
		memset(pipeLineOutPut->Freq, 0, len * sizeof(double));
	}
	if (pipeLineOutPut->Mtf == NULL)
		pipeLineOutPut->Mtf = (double*)malloc(len * sizeof(double));
	
	for (int i = 0; i < len; i++) {
		sfrc = aveEdgeTmp[i];
		//(*freq)[i] = ((double)i / (double)size_x) / pixel / cos(theta);// 每个点对应的频率
		if (pipeLineOutPut->Freq != NULL) {
			if (MTFPostProcessorConfig::anglecorrect) {
				pipeLineOutPut->Freq[i] = ((double)i / (double)(lsf_len)) / edgePixel * 4.0 / cos(theta);
				//qDebug() << SlantedEdge::pixel << ":zz" << endl;
			}
			else {
				pipeLineOutPut->Freq[i] = ((double)i / (double)(lsf_len)) / edgePixel * ALPHA;
			}// 每个点对应的频率
			pipeLineOutPut->Mtf[i] = (double)(sfrc / aveEdgeTmp[0]);// 归一化sfr
		}
	}
	double delta = edgePixel / ALPHA;

	if (MTFPostProcessorConfig::ccdcorrect) {
		for (int i = 1; i < len; i++) {
			double correct_sinc = sin(M_PI * pipeLineOutPut->Freq[i] * delta) / (M_PI * pipeLineOutPut->Freq[i] * delta) * sin(2.0 * M_PI * pipeLineOutPut->Freq[i] * delta * sin(theta / 2.0)) / (2 * M_PI * pipeLineOutPut->Freq[i] * delta * sin(theta / 2.0));
			pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_sinc;// 矫正CCD的MTF

		}
	}
	if (MTFPostProcessorConfig::derivationcorrect) {
		for (int i = 1; i < len; i++) {
			double n = 1;
			double correct_diff = 0;
			while (n <= diff_window)
			{
				correct_diff += sin(2 * M_PI * pipeLineOutPut->Freq[i] * n * (edgePixel / ALPHA)) / (2 * M_PI * pipeLineOutPut->Freq[i] * n * (edgePixel / ALPHA));
				n++;
			}
			pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_diff * diff_window;// 矫正窗口微分的MTF

		}
	}

	if (MTFPostProcessorConfig::smoothcorrect){
		half_window = ESFProcessorConfig::meanWindow;
		if(half_window>0){
			for (int i = 1; i < len; i++) {
				double correct_smooth = sin(((double)half_window * 2 + 1) * M_PI * pipeLineOutPut->Freq[i] * edgePixel / ALPHA) / sin(M_PI * pipeLineOutPut->Freq[i] * edgePixel / ALPHA) / ((double)half_window * 2 + 1);
				if (pipeLineOutPut->Freq[i] < ALPHA / edgePixel / (double)(half_window * 2 + 1)) {
					pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_smooth;// 矫正平滑

				}
				else
					break;
			}
	    }

		half_window = LSFProcessorConfig::meanWindowSize;
		if (half_window > 0) {
			for (int i = 1; i < len; i++) {
				double correct_smooth = sin(((double)half_window * 2 + 1) * M_PI * pipeLineOutPut->Freq[i] * edgePixel / ALPHA) / sin(M_PI * pipeLineOutPut->Freq[i] * edgePixel / ALPHA) / ((double)half_window * 2 + 1);
				if (pipeLineOutPut->Freq[i] < ALPHA / edgePixel / (double)(half_window * 2 + 1)) {
					pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_smooth;// 矫正平滑

				}
				else
					break;
			}
		}
	}
	free(pipelineInput->farea_old);
	free(pipelineInput->overSample.aveEdge);
	free(pipelineInput->overSample.aveEdgeCopy);
	return true;
}
bool MTFPostProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int len = pipeLineOutPut->bin_len;
	int lsf_len = 2 * len;
	int half_window = LSFProcessorConfig::meanWindowSize;//平滑窗口大小
	int diff_window = 3;//微分窗口大小
	double sfrc;
	double* aveEdgeTmp = pipelineInput->overSample.aveEdgeCopy;
	double theta = fabs(atan(pipeLineOutPut->slope));
	double crossPixel = SlantedCross::getCrossPixel();
	if (pipeLineOutPut->Freq == NULL)
		pipeLineOutPut->Freq = (double*)malloc(len * sizeof(double));
	if (pipeLineOutPut->Mtf == NULL)
		pipeLineOutPut->Mtf = (double*)malloc(len * sizeof(double));
	
	for (int i = 0; i < len; i++) {
		sfrc = aveEdgeTmp[i];
		//(*freq)[i] = ((double)i / (double)size_x) / pixel / cos(theta);// 每个点对应的频率
		if (MTFPostProcessorConfig::anglecorrect) {
			pipeLineOutPut->Freq[i] = ((double)i / (double)(lsf_len)) / crossPixel * ALPHA / cos(theta);
		}
		else {
			pipeLineOutPut->Freq[i] = ((double)i / (double)(lsf_len)) / crossPixel * ALPHA;
		}// 每个点对应的频率
		pipeLineOutPut->Mtf[i] = (double)(sfrc / aveEdgeTmp[0]);// 归一化sfr
	}
	double delta = crossPixel / ALPHA;

	if (MTFPostProcessorConfig::ccdcorrect) {
		for (int i = 1; i < len; i++) {
			double correct_sinc = sin(M_PI * pipeLineOutPut->Freq[i] * delta) / (M_PI * pipeLineOutPut->Freq[i] * delta) * sin(2.0 * M_PI * pipeLineOutPut->Freq[i] * delta * sin(theta / 2.0)) / (2 * M_PI * pipeLineOutPut->Freq[i] * delta * sin(theta / 2.0));
			pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_sinc;// 矫正CCD的MTF

		}
	}

	if (MTFPostProcessorConfig::derivationcorrect) {
		for (int i = 1; i < len; i++) {
			double correct_diff = sin(M_PI * pipeLineOutPut->Freq[i] * (crossPixel / ALPHA)) / (M_PI * pipeLineOutPut->Freq[i] * (crossPixel / ALPHA));
			pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_diff;// 矫正有限元微分的MTF
		}
	}

	if (MTFPostProcessorConfig::smoothcorrect && half_window>0) {
		for (int i = 1; i < len; i++) {
			double correct_smooth = sin(((double)half_window * 2 + 1) * M_PI * pipeLineOutPut->Freq[i] * crossPixel / ALPHA) / sin(M_PI * pipeLineOutPut->Freq[i] * crossPixel / ALPHA) / ((double)half_window * 2 + 1);
			if (pipeLineOutPut->Freq[i] < ALPHA / crossPixel / (double)(half_window * 2 + 1)) {
				pipeLineOutPut->Mtf[i] = pipeLineOutPut->Mtf[i] / correct_smooth;// 矫正平滑

			}
			else
				break;
		}
	}
	free(pipelineInput->farea_old);
	free(pipelineInput->overSample.aveEdge);
	free(pipelineInput->overSample.aveEdgeCopy);
	return true;
}