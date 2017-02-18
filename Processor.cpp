#include "Processor.h"

void Processor::Tick(void) {
    // Propagate clock to the cache controller
    cacheController->Tick();
    cout << "Cache Controller Tick" << endl;
    transitionState();
}

void Processor::Idle_Action(void) {
    // No actions required
}

void Processor::CheckOpType_Action(void) {
    Instruction* i = getNextInstruction();
    if (i->OPCODE == LOAD_CMD || i->OPCODE == STORE_CMD) {
        // Send the memory instruction to the cache
        // controller for processing.
        cacheController->handleMemoryAccess(i);
    }
}

void Processor::MemoryReturnWait_Action(void) {
    // This function is used to keep track of how
    // long each memory operation takes
}

Processor::STATES Processor::Idle_Transition(void) {
    if (hasPendingInstructions()) {
        return CHECKOPTYPE_STATE;
    } else {
        return IDLE_STATE;   
    }
}

Processor::STATES Processor::CheckOpType_Transition(void) {
    if (nextInstIsMemoryOp()) {
        return MEMORYRETURNWAIT_STATE;
    } else {
        return IDLE_STATE;
    }
}

Processor::STATES Processor::MemoryReturnWait_Transition(void) {
    if (cacheController->hasPendingInstruction()) {
        // Controller is still busy handling instruction
        return MEMORYRETURNWAIT_STATE;   
    } else {
        return IDLE_STATE;   
    }
}

bool Processor::nextInstIsMemoryOp(void) {
    return (instrQueue->front()->OPCODE == LOAD_CMD || instrQueue->front()->OPCODE == STORE_CMD); 
}

Processor::STATES Processor::getNextState(void) {
    cout << "In getNextState()" << endl;
    switch(currentState) {
        case IDLE_STATE:
            cout << "Got Idle()" << endl;
            return Idle_Transition();
        case CHECKOPTYPE_STATE:
            return CheckOpType_Transition();
        case MEMORYRETURNWAIT_STATE:
            return MemoryReturnWait_Transition();
    }
    // Default
    return IDLE_STATE;
}

void Processor::callActionFunction(void) {
    switch(currentState) {
        case IDLE_STATE:
            Idle_Action();
            break;
        case CHECKOPTYPE_STATE:
            CheckOpType_Action();
            break;
        case MEMORYRETURNWAIT_STATE:
            MemoryReturnWait_Action();
            break;
    }
}

void Processor::transitionState(void) {
    // Update to the next state
    cout << "In transition state" << endl;
    STATES nextState = getNextState();
    cout << "Got next state" << endl;
    currentState = nextState;
    // Call the action function for the current state
    cout << "State transition. " << endl;
    callActionFunction();
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
    return (currentState == IDLE_STATE);
}

void Processor::insertInstruction(Instruction* i) {
    instrQueue->push(i);
}

unsigned int Processor::getInstructionCount(void) {
    return instrQueue->size();
}

Instruction* Processor::getNextInstruction(void) {
    Instruction* i = instrQueue->front();
    instrQueue->pop();
    return i;
}

bool Processor::hasPendingInstructions(void) {
    return (getInstructionCount() > 0);
}

void Processor::processInstruction(Instruction* i) {
    if (i->OPCODE == LOAD_CMD || i->OPCODE == STORE_CMD) {
        insertInstruction(i);
    }
}