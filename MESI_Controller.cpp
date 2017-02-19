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
        assert(!cache->isInvalid());
        
        // Assert that one of the other three states aret true
        bool isExlusive = cache->isExclusive(address);
        bool isModified = cache->isModified(address);
        bool isShared   = cache->isShared(address);
        
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
    // Reset boolean flags used
    gotDataReturnFromBusRead_Memory = false;
    gotDataReturnFromBusRead_Processor = false;
}

void MESI_Controller::CheckCacheLoad_Action(void) {
    
}

void MESI_Controller::IssueRead_Action(void) {
    
}

void MESI_Controller::UpdateCacheLoad_Action(void) {
    
}

void MESI_Controller::CheckCacheStore_Action(void) {
    
}

void MESI_Controller::IssueReadX_Action(void) {
    
}

void MESI_Controller::IssueInvalidate_Action(void) {
    
}

void MESI_Controller::UpdateCacheStore_Action(void) {
    
}
