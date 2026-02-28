#include "lsfprocessor.h"
#include "../ml_math/SlantedEdge.h"
#include "../ml_math/SlantedCross.h"

int LSFProcessorConfig::meanWindowSize = -1;
bool LSFProcessorConfig::hammingWindow = true;
bool LSFProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
	return false;
}
bool LSFProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int diff_window = 3;
	double* AveEdge = pipelineInput->overSample.aveEdge;
	int bin_len = pipeLineOutPut->bin_len;
	double centroid;
	int half_window = LSFProcessorConfig::meanWindowSize;//平滑窗口大小
// 将ESF转换为差分图LSF， 并计算LSF的质心
//calculate_derivative_old(bin_len, AveTmp, AveEdge, &centroid, 0);
	SlantedEdge::calculate_derivative_new(bin_len, AveEdge, &centroid, diff_window);

	// 寻找LSF的最大值点
	SlantedEdge::locate_max_PSF(bin_len, AveEdge, &pipeLineOutPut->center);


	double theta = fabs(atan(pipelineInput->slope));//斜边角度

	// 将LSF的最大值移到中心位置， 并加上汉明窗
	if(LSFProcessorConfig::hammingWindow)
	  SlantedEdge::apply_hamming_window((unsigned short)ALPHA, bin_len, (unsigned short)pipelineInput->dataInfo.size_x, AveEdge, &pipeLineOutPut->center);

	pipeLineOutPut->Lsf = (double*)malloc(bin_len * sizeof(double));

	if(LSFProcessorConfig::meanWindowSize>0)
		SlantedEdge::smooth(AveEdge, half_window, bin_len);
	//拷贝LSF

	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Lsf[i] = AveEdge[i];
	}
	//平滑LSF
	SlantedEdge::smooth(pipeLineOutPut->Lsf, 2, bin_len);
	//归一化LSF
	double max_lsf = *max_element(pipeLineOutPut->Lsf, pipeLineOutPut->Lsf + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Lsf[i] = pipeLineOutPut->Lsf[i] / max_lsf;
	}
	return true;

}
bool LSFProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	 int bin_len = pipeLineOutPut->bin_len;
	 int half_window = LSFProcessorConfig::meanWindowSize;//平滑窗口大小
	 double* AveEdge = pipelineInput->overSample.aveEdge;
	 pipelineInput->overSample.aveEdgeCopy = (double*)malloc(bin_len * sizeof(double));
	 SlantedCross::calculate_esf(bin_len, pipelineInput->overSample.aveEdgeCopy, AveEdge);


	// 寻找LSF的最大值点
	SlantedCross::locate_max_PSF(bin_len, AveEdge, &pipeLineOutPut->center);


	double theta = fabs(atan(pipelineInput->slope));//斜边角度

	// 将LSF的最大值移到中心位置， 并加上汉明窗
	if (LSFProcessorConfig::hammingWindow)
	  SlantedCross::apply_hamming_window((unsigned short)ALPHA, bin_len, (unsigned short)pipelineInput->dataInfo.size_x, AveEdge, &pipeLineOutPut->center);

	pipeLineOutPut->Lsf = (double*)malloc(bin_len * sizeof(double));

	//smooth(AveEdge, half_window, bin_len);
	if(half_window>0)
    	SlantedCross::smooth(AveEdge, half_window, bin_len);

	//拷贝LSF

	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Lsf[i] = AveEdge[i];
		//cout << AveEdge[i] << endl;
	}

	//平滑LSF
	SlantedCross::smooth(pipeLineOutPut->Lsf, 2, bin_len);
	//归一化LSF
	double max_lsf = *max_element(pipeLineOutPut->Lsf, pipeLineOutPut->Lsf + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Lsf[i] = pipeLineOutPut->Lsf[i] / max_lsf;

	}

	return true;
}