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
        queueBusCommand(BUSREAD, address);
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
    if (!cache->contains(address)) {
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
    cache->updateLRU(address);
}

void MESI_Controller::CheckCacheStore_Action(void) {
    // Nothing occurs in this state except cache checks,
    // which only determine state transition (see
    // CheckCacheStore_Transition())
}

void MESI_Controller::IssueReadX_Action(void) {
    if (!queuedBusRead) {
        // If we have done so yet in this state, queue the bus read,
        // otherwise, we just wait in the state without performing any action
        unsigned int address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
        queueBusCommand(BUSREADX, address);
        queuedBusRead = true;
    }
}

void MESI_Controller::IssueInvalidate_Action(void) {
    unsigned int address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    queueBusCommand(INVALIDATE,address);
}

void MESI_Controller::UpdateCacheStore_Action(void) {
    // In this state, the cache line is being updated
    // to MODIFIED.
    unsigned int address = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    if (!cache->contains(address)) {
        cache->insertLine(address);   
    }
    cache->setModified(address);
    cache->updateLRU(address);
}

void MESI_Controller::acceptBusTransaction(BusRequest* d) {
    unsigned int cmd     = d->commandCode;
    unsigned int address = d->payload;
    if (awaitingBusRead) {
        assert(dispatchedBusRead != NULL);
        unsigned int dispatchedAddress = dispatchedBusRead->payload;
        unsigned int dispatchedCmd     = dispatchedBusRead->commandCode;
        switch(cmd) {
            case BUSREAD:
                // See notes below. Main difference is if the BUSREAD
                // is to the same address that this controller is currently
                // waiting for data for, then additional actions may be required
                if (cache->contains(address)) {
                    if (cache->isExclusive(address) || cache->isShared(address)) {
                        queueBusCommand(DATA_RETURN_PROCESSOR, address);
                    } else if (cache->isModified(address)) {
                        queueBusCommand(BUSWRITE, address);
                    }
                    cache->setShared(address);
                }
                if (dispatchedAddress == address) {
                    
                }
                break;
            case BUSREADX:
                // See notes below. Main difference is if the BUSREADX
                // is to the same address that this controller is currently
                // waiting for data for, then additional actions may be required
                if (cache->contains(address)) {
                    if (cache->isExclusive(address) || cache->isShared(address)) {
                        queueBusCommand(DATA_RETURN_PROCESSOR, address);
                    } else if (cache->isModified(address)) {
                        queueBusCommand(BUSWRITE, address);
                    }
                    cache->setInvalid(address);
                }
                if (dispatchedAddress == address) {
                    
                }
                break;
            case BUSWRITE:
                // if this is a BUSWRITE to the address we are waiting for, then we can snoop the data.
                if (dispatchedAddress == address) {
                    snoopedDataReturnFromWriteback = true;
                    awaitingBusRead                = false;
                }
                break;
            case DATA_RETURN_MEMORY:
                // Check if the address for the data return is the address we are waiting
                // for or not. Set appropriate flags if it is. Also see notes below.
                if (dispatchedAddress == address) {
                    gotDataReturnFromBusRead_Memory = true;
                    awaitingBusRead                 = false;
                }
                break;
            case DATA_RETURN_PROCESSOR:
                // Check if the address for the data return is the address we are waiting
                // for or not. Set appropriate flags if it is
                if (dispatchedAddress == address) {
                    gotDataReturnFromBusRead_Processor = true;
                    awaitingBusRead                    = false;
                }
                cancelBusRequest(DATA_RETURN_PROCESSOR,address);
                break;
            case INVALIDATE:
                // See notes below. Main difference is if the INVALIDATE
                // is to the same address that this controller is currently
                // waiting for data for, then additional actions may be required
                if (cache->contains(address)) {
                    assert(!cache->isModified(address));
                    cache->setInvalid(address);
                }
                if (dispatchedAddress == address) {
                    
                }
                break;
        }
    } else {
        switch(cmd) {
            case BUSREAD:
                // We have to check if we have this in the cache, and if
                // we do, update the cache line state to shared. We also
                // need to issue a DATA_RETURN_PROCESSOR or BUSWRITE with the data
                // to perform a cache to cache transfer. If there are other
                // processors on the bus, and the cache line is shared, they may also issue
                // this command. Therefore, after queueing the DATA_RETURN_PROCESSOR, if it
                // one of these is observed on the bus to the same address, that
                // means another processor beat this one, and we should cancel
                // the bus request. After the DATA_RETURN_PROCESSOR or BUSWRITE is issued,
                // the cache line should be updated to the shared state.
                if (cache->contains(address)) {
                    if (cache->isExclusive(address) || cache->isShared(address)) {
                        queueBusCommand(DATA_RETURN_PROCESSOR, address);
                    } else if (cache->isModified(address)) {
                        queueBusCommand(BUSWRITE, address);
                    }
                    cache->setShared(address);
                }
                break;
            case BUSREADX:
                // If we have this item in the cache, then it must be invalidated.
                // If we currently have a modified copy of it, we must issue a BUSWRITE.
                // If we have a copy of the line, but it is not modified, we will issue a
                // DATA_RETURN_PROCESSOR to perform a cache to cache transfer.
                // After any required issues are complete, the line must be transitioned.
                // to invalid.
                if (cache->contains(address)) {
                    if (cache->isExclusive(address) || cache->isShared(address)) {
                        queueBusCommand(DATA_RETURN_PROCESSOR, address);
                    } else if (cache->isModified(address)) {
                        queueBusCommand(BUSWRITE, address);
                    }
                    cache->setInvalid(address);
                }
                break;
            case BUSWRITE:
                // BUSWRITES are only issued when a modified line is being written back
                // to memory. Since we aren't currently waiting for any data and just observed
                // a BUSWRITE, it is safe to assume that this cache does not contain the line
                // and can ignore this. assert that this data is not present in our cache
                // just to help catch bugs
                assert(!cache->contains(address));
                break;
            case DATA_RETURN_MEMORY:
                // We aren't waiting for data, so ignore this
                break;
            case DATA_RETURN_PROCESSOR:
                // If we've previously queued a DATA_RETURN_PROCESSOR to the same
                // address, we should cancel this request since it was serviced by
                // another cache controller
                cancelBusRequest(DATA_RETURN_PROCESSOR,address);
                break;
            case INVALIDATE:
                // If we have this line in the cache, we must invalidate it.
                // If the line is modified, it would need to be streamed out.
                // Given our protocol, this should not be possible though,
                // since INVALIDATE only ever is issued when a STORE is performed
                // to an address that is in the SHARED state (which by definition
                // means no modified copies exist). Assert that our line
                // is not modified and invalidate the copy if it exists.
                if (cache->contains(address)) {
                    assert(!cache->isModified(address));
                    cache->setInvalid(address);
                }
                break;
        }
    }
}

void MESI_Controller::queueBusCommand(unsigned int command, unsigned int payload) {
    BusRequest* r    = new BusRequest;
    r->commandCode   = command;
    r->payload       = payload;
    r->targetAddress = BROADCAST_ADX;
    r->sourceAddress = getAddress();
    addNewBusRequest(r);
}
