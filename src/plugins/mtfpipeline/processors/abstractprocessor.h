#pragma once
#include "parameters.h"
class AbstractProcessor {
public:
	virtual bool process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) = 0;
	virtual bool processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) = 0;
	virtual bool processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut) = 0;
};