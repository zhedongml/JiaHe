#include "fitprocessor.h"
#include "../ml_math/SlantedCross.h"
#include "../ml_math/SlantedEdge.h"
int FitProcessorConfig::polynominalOrder = 1;
bool FitProcessorConfig::distortioncorrect = false;
bool FitProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
    return false;
}
bool FitProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	bool err = 0;
	vector<double> k;
	int polyRank = FitProcessorConfig::polynominalOrder;
	SlantedEdge::polyfit(pipelineInput->centroids.temp, pipelineInput->centroids.shifts, pipelineInput->dataInfo.size_y, polyRank, k);
	pipelineInput->slope = k[1];
	pipeLineOutPut->slope = pipelineInput->slope;
	if (err != 0) {
		pipeLineOutPut->errNum = 3;
		return false; 
	}

	double cyclelimit = 5.0;
	int numcycles = 0;

	// 检查刀口斜率， 以确保后面超采样的质量
	unsigned short size_y = pipelineInput->dataInfo.size_y;
	err = SlantedEdge::check_slope(pipelineInput->slope, &size_y, &numcycles, cyclelimit, 1);
	

	int start_row, center_row;
	// 调整ROI的行
	center_row = pipelineInput->roi.rows / 2;
	start_row = center_row - size_y / 2; 
	pipelineInput->farea_old = pipelineInput->farea; //zzz farea_old 未使用
	pipelineInput->farea = pipelineInput->farea + start_row * pipelineInput->dataInfo.size_x;

	pipelineInput->dataInfo.size_y = size_y;
	if (err != 0) {
		//*pcnt2 = 2 * size_x; zzz
		pipeLineOutPut->errNum = 4;
		return false;
	}


	// 基于拟合结果更新shifts
	int col = size_y / 2;
	for (int i = 0; i < size_y; i++) {
		//pipelineInput->centroids.shifts[i] = (pipelineInput->slope) * (double)(i - col) + pipelineInput->centroids.offset;
		pipelineInput->centroids.shifts[i] = SlantedEdge::getY(pipelineInput->centroids.temp[i], k);
	}
	free(pipelineInput->centroids.temp);
	return true;
}
bool FitProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	bool err = 0;
	vector<double> k;
	int polyRank = FitProcessorConfig::polynominalOrder;
	SlantedCross::polyfit(pipelineInput->centroids.temp, pipelineInput->centroids.shifts, pipelineInput->dataInfo.size_y, polyRank, k);
	pipelineInput->slope = k[1];
	pipeLineOutPut->slope = pipelineInput->slope;
	if (err != 0) {
		pipeLineOutPut->errNum = 3;
		return false;
	}

	double cyclelimit = 5.0;
	int numcycles = 0;

	// 检查刀口斜率， 以确保后面超采样的质量
	unsigned short size_y = pipelineInput->dataInfo.size_y;
	err = SlantedCross::check_slope(pipelineInput->slope, &size_y, &numcycles, cyclelimit, 1);
	
	int start_row, center_row;
	// 调整ROI的行
	center_row = pipelineInput->roi.rows / 2;
	start_row = center_row - pipelineInput->dataInfo.size_y / 2; 
	pipelineInput->farea_old = pipelineInput->farea; //zzz farea_old 未使用
	pipelineInput->farea = pipelineInput->farea + start_row * pipelineInput->dataInfo.size_x;
	pipelineInput->dataInfo.size_y = size_y;
	if (err != 0) {
		//*pcnt2 = 2 * size_x; zzz
		pipeLineOutPut->errNum = 4;
		return false;
	}


	// 基于拟合结果更新shifts
	int col = size_y / 2;
	for (int i = 0; i < size_y; i++) {
		//pipelineInput->centroids.shifts[i] = (pipelineInput->slope) * (double)(i - col) + pipelineInput->centroids.offset;
		pipelineInput->centroids.shifts[i] = SlantedCross::getY(pipelineInput->centroids.temp[i], k);
	}
	free(pipelineInput->centroids.temp);
	return true;
}