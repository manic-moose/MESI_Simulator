#include <stdio.h>
#include "Processor.h"
ProcessorData* buildNewData(unsigned int isMem, unsigned int adx);

int main (void) {
 
    Processor *pobj = new Processor;
    pobj->Reset();

    pobj->Tick(buildNewData(0,0));
    pobj->Tick(buildNewData(1,1));
    pobj->Tick(buildNewData(1,1));

    printf("Test program\n");
    return 0;
}

ProcessorData* buildNewData(unsigned int isMem, unsigned int adx) {
    ProcessorData *data = new ProcessorData;
    data->isMemOp = isMem;
    data->memAdx = adx;
    return data;
}