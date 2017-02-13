#include <assert.h>
#include "Processor.h"
#include "BusRequest.h"

void Processor::Tick(void) {
    BEGIN_TRANSITION_MAP                         // - Current State -
        TRANSITION_MAP_ENTRY(ST_CHECK_OP)        // Idle
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)      // Check Operation Type
        TRANSITION_MAP_ENTRY(ST_PROCESS_MEM_OP)  // Processing Memory Operation
        TRANSITION_MAP_ENTRY(ST_IDLE)            // Processing Done Wait
    END_TRANSITION_MAP(pData)
}

void Processor::MemReturn(void) {
    BEGIN_TRANSITION_MAP                           // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Idle
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Check Operation Type
        TRANSITION_MAP_ENTRY(ST_PROCESS_DONE_WAIT) // Processing Memory Operation
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Processing Done Wait
    END_TRANSITION_MAP(NULL)
}

void Processor::Reset(void) {
    BEGIN_TRANSITION_MAP                   // - Current State -
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Idle
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Check Operation Type
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Processing Memory Operation
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Processing Done Wait
    END_TRANSITION_MAP(NULL)
}

void Processor::ST_Idle(void) {
    printf("Idle State Function Called\n");
}

void Processor::ST_CheckOP(ProcessorData* pData) {
    printf("CheckOp state function called\n");
    if (pData->isMemOp) {
        InternalEvent(ST_PROCESS_MEM_OP);
    } else {
        InternalEvent(ST_IDLE);
    }
}

void Processor::ST_ProcessMemOp(void) {
    printf("Process Mem Op function called\n");
}

void Processor::ST_ProcessDoneWait(void) {
    printf("Process done wait function called\n");
}

void Processor::acceptBusTransaction(BusRequest* d) {
}

BusRequest* Processor::initiateBusTransaction(void) {
}

bool Processor::requestsTransaction() {
    
}