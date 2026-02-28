#include "dftprocessor.h"
#include "../ml_math/SlantedCross.h"
#include "../ml_math/SlantedEdge.h"
bool DFTProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
    return false;
}
bool DFTProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {

	double* lsf_cutted = NULL;

	int lsf_len = pipeLineOutPut->bin_len;
	pipeLineOutPut->bin_len = lsf_len / 2;
	double tmp = 1.0;
	double tmp2 = 1.0 / ((double)lsf_len);
	double* AveEdge = pipelineInput->overSample.aveEdge;
	double* aveEdgeTmp = pipelineInput->overSample.aveEdgeCopy;
	(void)SlantedEdge::ftwos(lsf_len, tmp, AveEdge, lsf_len / 2, tmp2, aveEdgeTmp);
    return true;
}
bool DFTProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	double* lsf_cutted = NULL;

	int lsf_len = pipeLineOutPut->bin_len;
	pipeLineOutPut->bin_len = lsf_len / 2;
	double tmp = 1.0;
	double tmp2 = 1.0 / ((double)lsf_len);
	double* AveEdge = pipelineInput->overSample.aveEdge;
	double* aveEdgeTmp = pipelineInput->overSample.aveEdgeCopy;
	(void)SlantedCross::ftwos(lsf_len, tmp, AveEdge, lsf_len / 2, tmp2, aveEdgeTmp);
    return true;
}