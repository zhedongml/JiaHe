#include "preprocessor.h"
#include "../ml_math/SlantedCross.h"
#include "../ml_math/SlantedEdge.h"
bool PreProcessorconfig::isDarkNoise = true;
bool PreProcessorconfig::isPixelization = true;
int PreProcessorconfig::meanWinSize = 2;
bool PreProcessorconfig::hammingWind = true;
bool PreProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
    return false;
}
bool PreProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{   //zzz ÎŞmean win fit
    if(SlantedEdge::check_roi(pipelineInput->roi,pipeLineOutPut->direction))
	{
		pipelineInput->dataInfo.size_x = pipelineInput->roi.cols;
		pipelineInput->dataInfo.size_y = pipelineInput->roi.rows;
		pipelineInput->dataInfo.length = pipelineInput->dataInfo.size_x * pipelineInput->dataInfo.size_y;
		pipelineInput->farea = (double*)malloc(pipelineInput->dataInfo.length * sizeof(double));
		SlantedEdge::roi_normilized(pipelineInput->roi, pipelineInput->farea, pipelineInput->roi.cols, pipelineInput->roi.rows, pipelineInput->dataInfo.length);
		
		
		pipelineInput->centroids.shifts = (double*)malloc(pipelineInput->dataInfo.size_y * sizeof(double));
		pipelineInput->centroids.temp = (double*)malloc(pipelineInput->dataInfo.size_y * sizeof(double));
		bool errnumber = SlantedEdge::locate_centroids_new(pipelineInput->farea, pipelineInput->centroids.temp, pipelineInput->centroids.shifts,
			pipelineInput->dataInfo.size_x, pipelineInput->dataInfo.size_y, &(pipelineInput->centroids.offset),PreProcessorconfig::hammingWind);
		if (errnumber != 0) {
			pipeLineOutPut->errNum = 2;
			return false;
		}
		return true;
	}
	else {
		pipeLineOutPut->errNum = -1;
		return false;
	}
}

bool PreProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
	if (SlantedCross::check_roi(pipelineInput->roi, pipeLineOutPut->direction))
	{
		pipelineInput->dataInfo.size_x = pipelineInput->roi.cols;
		pipelineInput->dataInfo.size_y = pipelineInput->roi.rows;
		pipelineInput->dataInfo.length = pipelineInput->dataInfo.size_x * pipelineInput->dataInfo.size_y;
		pipelineInput->farea = (double*)malloc(pipelineInput->dataInfo.length * sizeof(double));
		SlantedCross::roi_normilized(pipelineInput->roi, pipelineInput->farea, pipelineInput->roi.cols, pipelineInput->roi.rows, pipelineInput->dataInfo.length);
	    
		pipelineInput->centroids.shifts = (double*)malloc(pipelineInput->dataInfo.size_y * sizeof(double));
		pipelineInput->centroids.temp = (double*)malloc(pipelineInput->dataInfo.size_y * sizeof(double));
		bool errnumber = SlantedCross::locate_centroids(pipelineInput->farea, pipelineInput->centroids.temp, pipelineInput->centroids.shifts, pipelineInput->dataInfo.size_x, pipelineInput->dataInfo.size_y, &(pipelineInput->centroids.offset),PreProcessorconfig::hammingWind);
		if (errnumber != 0) {
			pipeLineOutPut->errNum = 2;
			return false;
		}
		return true;
	}
	else {
		pipeLineOutPut->errNum = -1;
		return false;
	}
}

