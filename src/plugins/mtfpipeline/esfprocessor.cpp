#include "esfprocessor.h"
#include "SlantedCross.h"
#include "SlantedEdge.h"
int ESFProcessorConfig::meanWindow = -1;
bool ESFProcessorConfig::reverse = true;
bool ESFProcessor::process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut)
{
	return false;
}

bool ESFProcessor::processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int half_window = ESFProcessorConfig::meanWindow; //zzz meanwin?
	int bin_len = pipeLineOutPut->bin_len;
	//·­×ªESF
	double* AveEdge=pipelineInput->overSample.aveEdge;
	if (AveEdge[0] > AveEdge[bin_len - 1]) {
		std::reverse(AveEdge, AveEdge + bin_len);
	}
	if(PreProcessorconfig::isPixelization)
	  SlantedEdge::reduce_pixelization(AveEdge, bin_len);
	if(half_window > 0)
	 SlantedEdge::smooth(AveEdge, half_window, bin_len);


	pipelineInput->overSample.aveEdgeCopy = (double*)malloc(bin_len * sizeof(double));
	double* aveEdgeTmp = pipelineInput->overSample.aveEdgeCopy;

	memcpy(aveEdgeTmp, AveEdge, sizeof(double) * bin_len);


	pipeLineOutPut->Esf = (double*)malloc(bin_len * sizeof(double));

	//¿½±´ESF
	double max_esf = *std::max_element(aveEdgeTmp, aveEdgeTmp + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		aveEdgeTmp[i] = aveEdgeTmp[i] / max_esf;
		pipeLineOutPut->Esf[i] = aveEdgeTmp[i];
	}
	//ESFÆ½»¬

	//SlantedEdge::smooth(pipeLineOutPut->Esf, half_window, bin_len);
	SlantedEdge::smooth(pipeLineOutPut->Esf, 2, bin_len);

	max_esf = *std::max_element(pipeLineOutPut->Esf, pipeLineOutPut->Esf + bin_len);
	double min_esf = *min_element(pipeLineOutPut->Esf, pipeLineOutPut->Esf + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Esf[i] = (pipeLineOutPut->Esf[i] - min_esf) / (max_esf - min_esf);
	}

	return true;
}
bool ESFProcessor::processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) {
	int bin_len = pipeLineOutPut->bin_len;
	int half_window = ESFProcessorConfig::meanWindow;
	pipeLineOutPut->Esf = (double*)malloc(bin_len * sizeof(double));
	SlantedCross::calculate_esf(bin_len, pipeLineOutPut->Esf, pipeLineOutPut->Lsf);

	//¿½±´ESF
	double max_esf = *max_element(pipelineInput->overSample.aveEdgeCopy, pipelineInput->overSample.aveEdgeCopy + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		pipelineInput->overSample.aveEdgeCopy[i] = pipelineInput->overSample.aveEdgeCopy[i] / max_esf;
		pipeLineOutPut->Esf[i] = pipelineInput->overSample.aveEdgeCopy[i];
		//cout << (*esf)[i] << endl;
	}

	//ESFÆ½»¬

	//SlantedCross::smooth(pipeLineOutPut->Esf, half_window, bin_len);
	SlantedCross::smooth(pipeLineOutPut->Esf, 2, bin_len);

	max_esf = *max_element(pipeLineOutPut->Esf, pipeLineOutPut->Esf + bin_len);
	for (int i = 0; i < bin_len; i++)
	{
		pipeLineOutPut->Esf[i] = pipeLineOutPut->Esf[i] / max_esf;
		//cout << pipeLineOutPut->Esf[i] << endl;
	}
	return true;
}
