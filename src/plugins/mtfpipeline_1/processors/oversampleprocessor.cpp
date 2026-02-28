#include "oversampleprocessor.h"
#include "../ml_math/SlantedCross.h"
#include "../ml_math/SlantedEdge.h"
bool OverSampleProcessorConfig::forwardProjection = true;
bool OverSampleProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
	return false;
}
bool OverSampleProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int bin_len_val = (int)(ALPHA * pipelineInput->dataInfo.size_x);
	int size_x = pipelineInput->dataInfo.size_x;
	int size_y = pipelineInput->dataInfo.size_y;
	pipeLineOutPut->bin_len = bin_len_val;
	pipelineInput->overSample.aveEdge = (double*)malloc(bin_len_val * sizeof(double));
	long* counts = (long*)malloc(bin_len_val * sizeof(long));
	double* edgex = (double*)malloc(size_y * size_x * sizeof(double));
	double* Signal = (double*)malloc(size_y * size_x * sizeof(double));

	//归一化
	double dt = 0.0;
	double dt1 = 1.0;

	long pcnt = 0;
	for (int j = 0; j < size_y; j++) {
		for (int i = 0; i < size_x; i++) {
			edgex[pcnt] = (double)i - pipelineInput->centroids.shifts[j];// 计算每个点离刀口的距离
			Signal[pcnt] = ((pipelineInput->farea[((j * (long)(size_x)) + i)]) - dt) / (dt1 - dt);// 归一化每个点的亮度
			pcnt++;
		}
	}

	int nzero =SlantedEdge::bin_to_regular_xgrid((unsigned short)ALPHA, edgex, Signal,
		pipelineInput->overSample.aveEdge, counts,
		size_x,size_y);
	free(counts);
	free(Signal);
	free(edgex);
	free(pipelineInput->centroids.shifts);

	//初始化ESF,LSF横坐标
	pipeLineOutPut->Esf_Lsf_x = (double*)malloc(2 * bin_len_val * sizeof(double));
	for (int i = 0; i < 2 * bin_len_val; i++)
	{
		pipeLineOutPut->Esf_Lsf_x[i] = i * SlantedEdge::getSlantedPixel() / ALPHA * 1000;
	}
	return true;
}
bool OverSampleProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int bin_len_val = (int)(ALPHA * pipelineInput->dataInfo.size_x);
	int size_x = pipelineInput->dataInfo.size_x;
	int size_y = pipelineInput->dataInfo.size_y;
	pipeLineOutPut->bin_len = bin_len_val;
	pipelineInput->overSample.aveEdge = (double*)malloc(bin_len_val * sizeof(double));
	long* counts = (long*)malloc(bin_len_val * sizeof(long));
	size_t size = (size_t)size_y * (size_t)size_x;
	double* edgex = (double*)malloc(size * sizeof(double));
	double* Signal = (double*)malloc(size * sizeof(double));
	
	//归一化
	double dt = 0.0;
	double dt1 = 1.0;

	long pcnt = 0;
	for (int j = 0; j < size_y; j++) {
		for (int i = 0; i < size_x; i++) {
			edgex[pcnt] = (double)i - pipelineInput->centroids.shifts[j];// 计算每个点离刀口的距离
			Signal[pcnt] = ((pipelineInput->farea[((j * (long)(size_x)) + i)]) - dt) / (dt1 - dt);// 归一化每个点的亮度
			pcnt++;
		}
	}

	int nzero = SlantedCross::bin_to_regular_xgrid((unsigned short)ALPHA, edgex, Signal,
		pipelineInput->overSample.aveEdge, counts,
		size_x, size_y);
	free(counts);
	free(Signal);
	free(edgex);
	free(pipelineInput->centroids.shifts);

	//初始化ESF,LSF横坐标
	pipeLineOutPut->Esf_Lsf_x = (double*)malloc(2 * bin_len_val * sizeof(double));
	for (int i = 0; i < 2 * bin_len_val; i++)
	{
		pipeLineOutPut->Esf_Lsf_x[i] = i * SlantedCross::getCrossPixel() / ALPHA * 1000;
	}
	return true;
}