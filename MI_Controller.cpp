#include "MI_Controller.h"

MI_Controller::STATES MI_Controller::getNextState(void) {
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
        case UPDATE_CACHE_ST_STATE:
            return UpdateCacheStore_Transition();
    }
    return IDLE_STATE;
}

void MI_Controller::reportState(void) {
    switch(currentState) {
        case IDLE_STATE:
            cout << "MI_Controller Idle State...";
            break;
        case CHECK_CACHE_LD_STATE:
            cout << "MI_Controller CheckCacheLd State  Instruction Address: "<< cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
        case ISSUE_READ_STATE:
            cout << "MI_Controller Read State  Instruction Address: " << cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
        case UPDATE_CACHE_LD_STATE:
            cout << "MI_Controller UpdateCacheLd State  Instruction Address: "<< cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
        case CHECK_CACHE_ST_STATE:
            cout << "MI_Controller CheckCacheSt State  Instruction Address: "<< cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
        case ISSUE_READX_STATE:
            cout << "MI_Controller Readx State  Instruction Address: "<< cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
        case UPDATE_CACHE_ST_STATE:
            cout << "MI_Controller UpdateCacheSt State  Instruction Address: "<< cache->getLineAlignedAddress(currentInstruction->ADDRESS);
            break;
    }
}

void MI_Controller::callActionFunction(void) {
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
        case UPDATE_CACHE_ST_STATE:
            UpdateCacheStore_Action();
            break;
    }
}

void MI_Controller::transitionState(void) {
    //cout << getAddress() << " Transition  ";
    //reportState();
    STATES nextState = getNextState();
    currentState = nextState;
    //cout << getAddress() << " ";
    //reportState();
    //cout << endl;
    callActionFunction();
}

//Define state transition functions
MI_Controller::STATES MI_Controller::Idle_Transition(void) {
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

MI_Controller::STATES MI_Controller::CheckCacheLoad_Transition(void) {
    unsigned long long address = currentInstruction->ADDRESS;
    if (cache->contains(address)) {
        // Cache Hit - Line must be in one of M, E, or S. Since
        // this is only a load, the cache line state does not
        // need to be updated except for LRU.
        load_hits++;
        return UPDATE_CACHE_LD_STATE;
    } else {
        load_misses++;
        // Cache Miss - a Bus read will need to be issued.
        return ISSUE_READ_STATE;
    }
}

MI_Controller::STATES MI_Controller::IssueRead_Transition(void) {
    if (awaitingBusRead || awaitingDataLocal) {
        return ISSUE_READ_STATE;   
    } else {
        return UPDATE_CACHE_LD_STATE;
    }
}

MI_Controller::STATES MI_Controller::UpdateCacheLoad_Transition(void) {
    // No branches here
    return IDLE_STATE;
}

MI_Controller::STATES MI_Controller::CheckCacheStore_Transition(void) {
    unsigned long long address = currentInstruction->ADDRESS;
    if (cache->contains(address)) {
        store_hits++;
        // This item is in the cache - need to check MESI bits to determine
        // next state.
        
        // Assertion - this should never be true here, and if
        // it is, there is a bug.
        assert(!cache->isInvalid(address));
        bool isExclusive = cache->isExclusive(address);
        bool isModified = cache->isModified(address);
        bool isShared   = cache->isShared(address);
        
        // assertion to ensure the "de-feature" of the MESI controller worked properly
        assert(isModified && !isShared && !isExclusive);
        return UPDATE_CACHE_ST_STATE;
    } else {
        store_misses++;
        // Cache state is invalid, need to issue a READX (read exclusive)
        return ISSUE_READX_STATE;
    }
}

MI_Controller::STATES MI_Controller::IssueReadX_Transition(void) {
    if (awaitingBusRead || awaitingDataLocal) {
        return ISSUE_READX_STATE;
    } else {
        return UPDATE_CACHE_ST_STATE;
    }
}

MI_Controller::STATES MI_Controller::UpdateCacheStore_Transition(void) {
    return IDLE_STATE;
}

//Define state action functions
void MI_Controller::Idle_Action(void) {
    // Reset boolean flags
    gotDataReturnFromBusRead_Memory    = false;
    gotDataReturnFromBusRead_Processor = false;
    snoopedDataReturnFromWriteback     = false;
    awaitingBusRead                    = false;
    pendingInstructionFlag             = false;
    queuedBusRead                      = false;
    sawBusReadToMyIncomingAddress      = false;
    sawBusReadXToMyIncomingAddress     = false;
    sawInvalidateToMyIncomingAddress   = false;
}

void MI_Controller::CheckCacheLoad_Action(void) {
    transitionState();
}

void MI_Controller::IssueRead_Action(void) {
    if (!queuedBusRead) {
        // If we have done so yet in this state, queue the bus read,
        // otherwise, we just wait in the state without performing any
        // action
        unsigned long long address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
        queueBusCommand(BUSREADX, address);
        queuedBusRead = true;
        awaitingDataLocal = true;
        awaitingDataLocal_Address = address;
    }
}

void MI_Controller::UpdateCacheLoad_Action(void) {
    // When updating the cache for a load command, it will
    // either be transitioned to E or S, depending on 
    // the state of other caches. If the data comes
    // from a cache-to-cache transfer, then we know
    // that at least one other cache contains
    // a copy of the shared data.
    unsigned long long address = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    if (sawBusReadXToMyIncomingAddress) {
        // While waiting for data, observed a bus request for the same address.
        // Rather than updating the cache, the data would just be forwarded to the
        // processor.
        return;   
    }
    if (!cache->contains(address)) {
        // We didn't already have the data, so it must have come from the bus
        assert(gotDataReturnFromBusRead_Memory || gotDataReturnFromBusRead_Processor || snoopedDataReturnFromWriteback);
        // First check that the cache has an empty space, and if not, we need to evict a line
        // and potentially write it back out.
        if (cache->isFull(address)) {
            CacheLine* evictedLine = cache->evictLineInSet(address);
            if (evictedLine->isModified()) {
                // Evicted line is modified, so time to write back out
                unsigned int setNumber      = evictedLine->getSetNumber();
                unsigned long long tag            = evictedLine->getTag();
                unsigned long long evictedLineAdx = cache->getAddress(setNumber, tag);
                queueMaxPriorityBusCommand(BUSWRITE,evictedLineAdx);
            }
        }
        cout << "Controller: " << getAddress() << " Code: CACHE_INSERT_MODIFIED  Payload: " << address << endl;
        cache->insertLine(address);
        cache->setModified(address);
    }
    cache->updateLRU(address);
}

void MI_Controller::CheckCacheStore_Action(void) {
    // Nothing occurs in this state except cache checks,
    // which only determine state transition (see
    // CheckCacheStore_Transition())
    transitionState();
}

void MI_Controller::IssueReadX_Action(void) {
    if (!queuedBusRead) {
        // If we have done so yet in this state, queue the bus read,
        // otherwise, we just wait in the state without performing any action
        unsigned long long address       = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
        queueBusCommand(BUSREADX, address);
        queuedBusRead = true;
        awaitingDataLocal = true;
        awaitingDataLocal_Address = address;
    }
}

void MI_Controller::UpdateCacheStore_Action(void) {
    // In this state, the cache line is being updated
    // to MODIFIED.
    unsigned long long address = cache->getLineAlignedAddress(currentInstruction->ADDRESS);
    if (sawBusReadToMyIncomingAddress || sawBusReadXToMyIncomingAddress || sawInvalidateToMyIncomingAddress) {
        // Saw an operation to this address while waiting for data to return. Instead of updating the cache,
        // a write request is issued to memory immediately. No actionst to take here
        queueMaxPriorityBusCommand(BUSWRITE,address);
        return;
    }
    if (!cache->contains(address)) {
        // First check that the cache has an empty space, and if not, we need to evict a line
        // and potentially write it back out.
        if (cache->isFull(address)) {
            CacheLine* evictedLine = cache->evictLineInSet(address);
            if (evictedLine->isModified()) {
                // Evicted line is modified, so time to write back out
                unsigned int setNumber      = evictedLine->getSetNumber();
                unsigned long long tag            = evictedLine->getTag();
                unsigned long long evictedLineAdx = cache->getAddress(setNumber, tag);
                queueMaxPriorityBusCommand(BUSWRITE,evictedLineAdx);
            }
        }
        cout << "Controller: " << getAddress() << " Code: CACHE_INSERT_MODIFIED  Payload: " << address << endl;
        cache->insertLine(address);   
        cache->setModified(address);
        assert(cache->isModified(address));
    } else {
        if (!cache->isModified(address)) {
            cout << "Controller: " << getAddress() << " Code: CACHE_UPGRADE_MODIFIED  Payload: " << address << endl;
        }
    }
    cache->updateLRU(address);
}

void MI_Controller::acceptBusTransaction(BusRequest* d) {
    switch(d->commandCode) {
        case BUSREAD:
            handleBusRead(d);
            break;
        case BUSREADX:
            handleBusReadX(d);
            break;
        case BUSWRITE:
            handleBusWrite(d);
            break;
        case DATA_RETURN_MEMORY:
            handleDataReturnMemory(d);
            break;
        case DATA_RETURN_PROCESSOR:
            handleDataReturnProcessor(d);
            break;
        case NULL_BURST:
            break;
    }
}

void MI_Controller::handleBusRead(BusRequest* d) {
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
    unsigned long long address = d->payload;
    cout << "Detected Illegal Bus Command (all bus commands in MI should be exclusive)" << endl;
    assert(0);
    if (cache->contains(address)) {
        if (cache->isExclusive(address) || cache->isShared(address)) {
            queueMaxPriorityBusCommand(DATA_RETURN_PROCESSOR, address);
        } else if (cache->isModified(address)) {
            queueMaxPriorityBusCommand(BUSWRITE, address);
        }
        if (!cache->isShared(address)) {
            cout << "Controller: " << getAddress() << " Code: CACHE_DOWNGRADE_SHARED_1  Payload: " << address << endl;
        }
        cache->setShared(address);
    } else if (awaitingDataLocal && (awaitingDataLocal_Address == address)) {
        // We've queued up a read command to that address...
        sawBusReadToMyIncomingAddress = true;
    }
}

void MI_Controller::handleBusReadX(BusRequest* d) {
    // If we have this item in the cache, then it must be invalidated.
    // If we currently have a modified copy of it, we must issue a BUSWRITE.
    // If we have a copy of the line, but it is not modified, we will issue a
    // DATA_RETURN_PROCESSOR to perform a cache to cache transfer.
    // After any required issues are complete, the line must be transitioned.
    // to invalid.
    unsigned long long address = d->payload;
    if (cache->contains(address)) {
        if (cache->isExclusive(address) || cache->isShared(address)) {
            queueMaxPriorityBusCommand(DATA_RETURN_PROCESSOR, address);
        } else if (cache->isModified(address)) {
            queueMaxPriorityBusCommand(BUSWRITE, address);
        }
        cout << endl << "Controller: " << getAddress() << " Code: CACHE_INVALIDATE_0  Payload: " << address << endl;
        cache->invalidate(address);
        assert(!cache->contains(address));
    } else if (awaitingDataLocal && (awaitingDataLocal_Address == address)) {
        // We've queued up a read command to that address...
        sawBusReadXToMyIncomingAddress = true;
    }
}

void MI_Controller::handleBusWrite(BusRequest* d) {
    // BUSWRITES are only issued when a modified line is being written back
    // to memory. Since we aren't currently waiting for any data and just observed
    // a BUSWRITE, it is safe to assume that this cache does not contain the line
    // and can ignore this. assert that this data is not present in our cache
    // just to help catch bugs
    unsigned long long address = d->payload;
    if (awaitingDataRemote(address)) {
        snoopedDataReturnFromWriteback = true;
        awaitingBusRead                = false;
        awaitingDataLocal              = false;
    }
}

void MI_Controller::handleDataReturnMemory(BusRequest* d) {
    unsigned long long address = d->payload;
    if (awaitingDataRemote(address)) {
        gotDataReturnFromBusRead_Memory = true;
        awaitingBusRead                 = false;
        awaitingDataLocal               = false;
    }
    cancelBusRequest(DATA_RETURN_PROCESSOR,address);
}

void MI_Controller::handleDataReturnProcessor(BusRequest* d) {
    // If we've previously queued a DATA_RETURN_PROCESSOR to the same
    // address, we should cancel this request since it was serviced by
    // another cache controller
    unsigned long long address = d->payload;
    if (awaitingDataRemote(address)) {
        gotDataReturnFromBusRead_Processor = true;
        awaitingBusRead                    = false;
        awaitingDataLocal                  = false;
    }
    cancelBusRequest(DATA_RETURN_PROCESSOR,address);
}

void MI_Controller::queueBusCommand(unsigned int command, unsigned long long payload, unsigned int targetAdx) {
    BusRequest* r    = new BusRequest;
    r->commandCode   = command;
    r->payload       = cache->getLineAlignedAddress(payload);
    r->targetAddress = targetAdx;
    r->sourceAddress = getAddress();
    addNewBusRequest(r);
}

void MI_Controller::queueMaxPriorityBusCommand(unsigned int command, unsigned long long payload, unsigned int targetAdx) {
    BusRequest* r    = new BusRequest;
    r->commandCode   = command;
    r->payload       = cache->getLineAlignedAddress(payload);
    r->targetAddress = targetAdx;
    r->sourceAddress = getAddress();
    addNewMaxPriorityBusRequest(r);
}

bool MI_Controller::awaitingDataRemote(unsigned long long address) {
    if (dispatchedBusRead == NULL) {
        return false;
    } else {
        return (((cache->getLineAlignedAddress(currentInstruction->ADDRESS)) == address) && awaitingBusRead);
    }
}