#pragma once
#include "abstractprocessor.h"
#include "parameters.h"
#include "processorconfig.h"

class PreProcessor :public AbstractProcessor
{
public:
	bool process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
	bool processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
	bool processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
};

