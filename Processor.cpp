#include "Processor.h"


void Processor::Tick(void) {
    BEGIN_TRANSITION_MAP                         // - Current State -
        TRANSITION_MAP_ENTRY(ST_CHECK_OP_TYPE)   // Idle
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)      // Check Operation Type
        TRANSITION_MAP_ENTRY(ST_IDLE)            // Processing Done Wait
    END_TRANSITION_MAP(NULL)
}

void Processor::MemReturn(void) {
    BEGIN_TRANSITION_MAP                           // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Idle
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Check Operation Type
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)        // Processing Done Wait
    END_TRANSITION_MAP(NULL)
}

void Processor::Reset(void) {
    BEGIN_TRANSITION_MAP                   // - Current State -
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Idle
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Check Operation Type
        TRANSITION_MAP_ENTRY(ST_IDLE)      // Processing Done Wait
    END_TRANSITION_MAP(NULL)
}

void Processor::ST_Idle(void) {
    printf("Idle State Function Called\n");
}

void Processor::ST_CheckOpType(void) {
    printf("CheckOp state function called\n");
    
    if (hasPendingInstructions()) {
        Instruction i = getNextInstruction();
        if (i.OPCODE == LOAD_CMD || i.OPCODE == STORE_CMD) {
            InternalEvent(ST_MEMORY_RETURN_WAIT);
        } else {
            InternalEvent(ST_IDLE);
        }
    } else {
        InternalEvent(ST_IDLE);
    }
}

void Processor::ST_MemoryReturnWait(void) {
    printf("Process done wait function called\n");
}

void Processor::acceptBusTransaction(BusRequest* d) {
    cacheController->acceptBusTransaction(d);
}

BusRequest* Processor::initiateBusTransaction(void) {
    return cacheController->initiateBusTransaction();
}

bool Processor::requestsTransaction(void) {
    return cacheController->requestsTransaction();
}

bool Processor::isIdle(void) {
    return (currentState == ST_IDLE);
}

void Processor::insertInstruction(Instruction i) {
    instrQueue->push(i);
}

unsigned int Processor::getInstructionCount(void) {
    return instrQueue->size();
}

Instruction Processor::getNextInstruction(void) {
    Instruction i = instrQueue->front();
    instrQueue->pop();
    return i;
}

bool Processor::hasPendingInstructions(void) {
    return (getInstructionCount() > 0);
}