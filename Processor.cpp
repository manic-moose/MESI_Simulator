#include "Processor.h"

void Processor::Tick(void) {
    // Propagate clock to the cache controller
    //reportState();
    cacheController->Tick();
    transitionState();
}

void Processor::reportState(void) {
   switch(currentState) {
        case IDLE_STATE:
            cout << "Current State Idle...";
            break;
        case CHECKOPTYPE_STATE:
            cout << "Current State CheckOpType...";
            break;
        case MEMORYRETURNWAIT_STATE:
            cout << "Current State MemoryReturnWait...";
            break;
    }
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
        numberMemoryOperations++;
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
    if (cacheController->hasPendingInstruction()) {
        return MEMORYRETURNWAIT_STATE;
    } else if (hasPendingInstructions() && nextInstIsMemoryOp()) {
        return CHECKOPTYPE_STATE;
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
    Instruction* nextInstruction = instrQueue->front();
    return (nextInstruction->OPCODE == LOAD_CMD || nextInstruction->OPCODE == STORE_CMD); 
}

Processor::STATES Processor::getNextState(void) {
    switch(currentState) {
        case IDLE_STATE:
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
    STATES nextState = getNextState();
    currentState = nextState;
    // Call the action function for the current state
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
    return (instrQueue->size() > 0);
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
    unsigned int opcode = i->OPCODE;
    assert((opcode == LOAD_CMD) || (opcode == STORE_CMD) || (opcode == NOP_CMD));
    if (i->OPCODE == LOAD_CMD || i->OPCODE == STORE_CMD) {
        insertInstruction(i);
    }
}

bool Processor::requestsLock(void) {
    return cacheController->requestsLock();
}

void Processor::setAddress(unsigned int address) {
    nodeAddress = address;
    cacheController->setAddress(address);
}

unsigned long Processor::getTotalMemoryOps(void) {
    return numberMemoryOperations;
}