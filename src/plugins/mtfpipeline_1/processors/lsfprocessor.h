#pragma once
#include "abstractprocessor.h"
#include "processorconfig.h"

class LSFProcessor :
    public AbstractProcessor
{
public:
    bool process(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
    bool processEdge(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
    bool processCross(PipeLineInput* pipelineInput, PipeLineOutPut* pipeLineOutPut);
};

