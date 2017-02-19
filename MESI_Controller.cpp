#include "MESI_Controller.h"

MESI_Controller::STATES MESI_Controller::getNextState(void) {
    switch(currentState) {
        case IDLE_STATE:
            return Idle_Transition();
        case CHECK_CACHE_LD_STATE:
            return CheckCacheLoad_Transition();
        case ISSUE_READ_STATE:
            return IssueRead_Transition();
        case UPDATE_CACHE_LD_STATE:
            return UpdateCacheLoad_Transition();
        case CHECK_CACHE_ST_STATE:
            return CheckCacheStore_Transition();
        case ISSUE_READX_STATE:
            return IssueReadX_Transition();
        case ISSUE_INVALIDATE_STATE:
            return IssueInvalidate_Transition();
        case UPDATE_CACHE_ST_STATE:
            return UpdateCacheStore_Transition();
    }
    return IDLE_STATE;
}

void MESI_Controller::callActionFunction(void) {
    switch(currentState) {
        case IDLE_STATE:
            Idle_Action();
            break;
        case CHECK_CACHE_LD_STATE:
            CheckCacheLoad_Action();
            break;
        case ISSUE_READ_STATE:
            IssueRead_Action();
            break;
        case UPDATE_CACHE_LD_STATE:
            UpdateCacheLoad_Action();
            break;
        case CHECK_CACHE_ST_STATE:
            CheckCacheStore_Action();
            break;
        case ISSUE_READX_STATE:
            IssueReadX_Action();
            break;
        case ISSUE_INVALIDATE_STATE:
            IssueInvalidate_Action();
            break;
        case UPDATE_CACHE_ST_STATE:
            UpdateCacheStore_Action();
            break;
    }
}

void MESI_Controller::transitionState(void) {
    STATES nextState = getNextState();
    currentState = nextState;
    callActionFunction();
}

//Define state transition functions
MESI_Controller::STATES MESI_Controller::Idle_Transition(void) {
    if (hasPendingInstruction()) {
        unsigned int opcode = currentInstruction->OPCODE;
        if (opcode == LOAD_CMD) {
            return CHECK_CACHE_LD_STATE;
        } else if (opcode == STORE_CMD) {
            return CHECK_CACHE_ST_STATE;
        } else {
            return IDLE_STATE;   
        }
    } else {
        return IDLE_STATE;   
    }
}

MESI_Controller::STATES MESI_Controller::CheckCacheLoad_Transition(void) {
    unsigned int address = currentInstruction->ADDRESS;
    if (cache->contains(address)) {
        // Cache Hit - Line must be in one of M, E, or S. Since
        // this is only a load, the cache line state does not
        // need to be updated except for LRU.
        return UPDATE_CACHE_LD_STATE;
    } else {
        // Cache Miss - a Bus read will need to be issued.
        return ISSUE_READ_STATE;
    }
}

MESI_Controller::STATES MESI_Controller::IssueRead_Transition(void) {
    if (gotDataReturnFromBusRead_Memory || gotDataReturnFromBusRead_Processor) {
        // Data was returned from the bus
        return UPDATE_CACHE_LD_STATE;
    } else {
        // Still waiting on data to return from the bus read
        return ISSUE_READ_STATE;   
    }
}

MESI_Controller::STATES MESI_Controller::UpdateCacheLoad_Transition(void) {
    // No branches here
    return IDLE_STATE;
}

MESI_Controller::STATES MESI_Controller::CheckCacheStore_Transition(void) {
    unsigned int address = currentInstruction->ADDRESS;
    if (cache->contains(address)) {
        // This item is in the cache - need to check MESI bits to determine
        // next state.
        
        // Assertion - this should never be true here, and if
        // it is, there is a bug.
        assert(!cache->isInvalid(address));
        
        bool isExclusive = cache->isExclusive(address);
        bool isModified = cache->isModified(address);
        bool isShared   = cache->isShared(address);
        
        // Assert that one of the other three states are true
        assert(isExclusive || isModified || isShared);
        
        if (isModified || isExclusive) {
            // We have the only copy, so we can freely update
            // the cache and transition to MODIFIED
            return UPDATE_CACHE_ST_STATE;
        } else {
            // Shared - need to invalidate other cache's copies
            return ISSUE_INVALIDATE_STATE;
        }
        
    } else {
        // Cache state is invalid, need to issue a READX (read exclusive)
        return ISSUE_READX_STATE;
    }
}

MESI_Controller::STATES MESI_Controller::IssueReadX_Transition(void) {
    if (gotDataReturnFromBusRead_Memory || gotDataReturnFromBusRead_Processor) {
        // Data was returned from the bus
        return UPDATE_CACHE_ST_STATE;
    } else {
        // Still waiting on data to return from the bus read
        return ISSUE_READX_STATE;   
    }
}

MESI_Controller::STATES MESI_Controller::IssueInvalidate_Transition(void) {
    // Message is sent to invalidate other copies
    return UPDATE_CACHE_ST_STATE;
}

MESI_Controller::STATES MESI_Controller::UpdateCacheStore_Transition(void) {
    return IDLE_STATE;
}

//Define state action functions
void MESI_Controller::Idle_Action(void) {
    // Reset boolean flags
    gotDataReturnFromBusRead_Memory    = false;
    gotDataReturnFromBusRead_Processor = false;
    snoopedDataReturnFromWriteback     = false;
    awaitingBusRead                    = false;
    pendingInstructionFlag             = false;
    queuedBusRead                      = false;
}

void MESI_Controller::CheckCacheLoad_Action(void) {
    // Nothing occurs in this state except cache checks,
    // which only determine state transition (see
    // CheckCacheLoad_Transition())
}

void MESI_Controller::IssueRead_Action(void) {
    if (!queuedBusRead) {
        // If we have done so yet in this state, queue the bus read,
        // otherwise, we just wait in the state without performing any
        // action
        unsigned int address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
        BusRequest* readRequest    = new BusRequest;
        readRequest->commandCode   = BUSREAD;
        readRequest->payload       = address;
        readRequest->targetAddress = BROADCAST_ADX;
        readRequest->sourceAddress = getAddress();
        addNewBusRequest(readRequest);
        queuedBusRead = true;
    }
}

void MESI_Controller::UpdateCacheLoad_Action(void) {
    // When updating the cache for a load command, it will
    // either be transitioned to E or S, depending on 
    // the state of other caches. If the data comes
    // from a cache-to-cache transfer, then we know
    // that at least one other cache contains
    // a copy of the shared data.
    unsigned int address = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    if (cache->contains(address)) {
        // If we already had the data, this means it was a cache hit. We do not need
        // to update the state of the cache line except for LRU bits
    } else {
        // We didn't already have the data, so it must have come from the bus
        assert(gotDataReturnFromBusRead_Memory || gotDataReturnFromBusRead_Processor || snoopedDataReturnFromWriteback);
        cache->insertLine(address);
        if (gotDataReturnFromBusRead_Memory) {
            cache->setExclusive(address);
        } else if (gotDataReturnFromBusRead_Processor) {
            cache->setShared(address);
        } else {
            //snoopedDataReturnFromWriteback
            cache->setShared(address);
        }
    }
}

void MESI_Controller::CheckCacheStore_Action(void) {
    // Nothing occurs in this state except cache checks,
    // which only determine state transition (see
    // CheckCacheStore_Transition())
}

void MESI_Controller::IssueReadX_Action(void) {
    if (!queuedBusRead) {
        // If we have done so yet in this state, queue the bus read,
        // otherwise, we just wait in the state without performing any
        // action
        unsigned int address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
        BusRequest* readRequest    = new BusRequest;
        readRequest->commandCode   = BUSREADX;
        readRequest->payload       = address;
        readRequest->targetAddress = BROADCAST_ADX;
        readRequest->sourceAddress = getAddress();
        addNewBusRequest(readRequest);
        queuedBusRead = true;
    }
}

void MESI_Controller::IssueInvalidate_Action(void) {
    unsigned int address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    BusRequest* invalidate    = new BusRequest;
    invalidate->commandCode   = INVALIDATE;
    invalidate->payload       = address;
    invalidate->targetAddress = BROADCAST_ADX;
    invalidate->sourceAddress = getAddress();
    addNewBusRequest(invalidate);
}

void MESI_Controller::UpdateCacheStore_Action(void) {
    // In this state, the cache line is being updated
    // to MODIFIED.
    unsigned int address = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    if (!cache->contains(address)) {
        cache->insertLine(address);   
    }
    cache->setModified(address);
}
