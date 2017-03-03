#include "CacheController.h"

void CacheController::addNewBusRequest(BusRequest* r) {
    busReqQueue->insert(busReqQueue->begin(), r);   
}

void CacheController::addNewMaxPriorityBusRequest(BusRequest* r) {
    maxPriorityQueue->insert(maxPriorityQueue->begin(), r);   
}

bool CacheController::hasQueuedBusRequest(void) {
    return (busReqQueue->size() > 0) || (maxPriorityQueue->size() > 0);
}

bool CacheController::hasBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload) {
    BusRequest* r = getBusRequestWithParams(code,target,source,payload);
    return (r != NULL);
}

BusRequest* CacheController::getBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            return r;   
        }
    }
    return NULL;
}

void CacheController::deleteBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            busReqQueue->erase(it);
        }
    }
}

bool CacheController::requestsTransaction(void) {
    return ((busReqQueue->size() > 0) || (maxPriorityQueue->size() > 0));
}

bool CacheController::requestsLock(void) {
    return bursting;
    //return requestsTransaction();   
}

BusRequest* CacheController::initiateBusTransaction(void) {
    bool maxPriorityTrans = false;
    if (maxPriorityQueue->size() > 0) {
        nextToIssue = maxPriorityQueue->back();
        maxPriorityTrans = true;
    } else {
        nextToIssue = busReqQueue->back();
    }
    unsigned int code = nextToIssue->commandCode;
    unsigned long long payload = nextToIssue->payload;
    if (bursting) {
        if (hasLock()) {
            burstCounter++;   
        }
        if (burstCounter == burstLen) {
            if (maxPriorityTrans) {
                maxPriorityQueue->pop_back();
            } else {
                busReqQueue->pop_back();
            }
            burstCounter = 0;
        } else {
            BusRequest* nullBurst = new BusRequest;
            nullBurst->commandCode = NULL_BURST;
            nullBurst->targetAddress = BROADCAST_ADX;
            nullBurst->sourceAddress = getAddress();
            nullBurst->payload = payload;
            nextToIssue = nullBurst;
        }
    } else {
        if (maxPriorityTrans) {
            maxPriorityQueue->pop_back();
        } else {
            busReqQueue->pop_back();
        }
    }
    code = nextToIssue->commandCode; // Update in case next is actually a null burst
    switch (code) {
        case BUSREAD:
            cout << "Controller: " << getAddress() << " Code: BUSREAD                Payload: " << payload << endl;
            break;
        case BUSREADX:
            cout << "Controller: " << getAddress() << " Code: BUSREADX               Payload: " << payload << endl;
            break;
        case BUSWRITE:
            cout << "Controller: " << getAddress() << " Code: BUSWRITE               Payload: " << payload << endl;
            break;
        case DATA_RETURN_MEMORY:
            cout << "Controller: " << getAddress() << " Code: DATA_RETURN_MEMORY     Payload: " << payload << endl;
            break;
        case DATA_RETURN_PROCESSOR:
            cout << "Controller: " << getAddress() << " Code: DATA_RETURN_PROCESSOR  Payload: " << payload << endl;
            break;
        case INVALIDATE:
            cout << "Controller: " << getAddress() << " Code: INVALIDATE             Payload: " << payload << endl;
            break;
        case NULL_BURST:
            cout << "Controller: " << getAddress() << " Code: NULL_BURST             Payload: " << payload << endl;
            break;
    }
    if ((nextToIssue->commandCode == BUSREAD) || nextToIssue->commandCode == BUSREADX) {
        dispatchedBusRead = nextToIssue;
        awaitingBusRead = true;
    }
    if (nextToIssue->commandCode == outgoingCommandWaitCode) {
        outgoingCommandWaitFlag = false;
    }
    return nextToIssue;
}

bool CacheController::hasPendingInstruction(void) {
    return pendingInstructionFlag;   
}

void CacheController::invalidateCacheItem(unsigned long long memoryAdx) {
    assert(cache->contains(memoryAdx));
    CacheLine* line = cache->invalidate(memoryAdx);
    if (line->isDirty()) {
        BusRequest* streamOut = new BusRequest;
        streamOut->commandCode   = BUSWRITE;
        streamOut->targetAddress = BROADCAST_ADX;
        streamOut->sourceAddress = getAddress();
        streamOut->payload       = memoryAdx;
        addNewBusRequest(streamOut);
    }
}

bool CacheController::requestsMaxPriority (void) {
    return (maxPriorityQueue->size() > 0);
}

void CacheController::handleMemoryAccess(Instruction* i) {
    assert(!hasPendingInstruction());
    
    currentInstruction = i;
    unsigned long long address = cache->getLineAlignedAddress(i->ADDRESS);
    if (i->OPCODE == STORE_CMD) {
        cout << "Controller: " << getAddress() << " Code: INSTRUCTION STORE  Payload: " << address << endl;
    } else if (i->OPCODE == LOAD_CMD) {
        cout << "Controller: " << getAddress() << " Code: INSTRUCTION LOAD  Payload: " << address << endl;
    }
    pendingInstructionFlag = true;
}

void CacheController::updateBusBurstRequest(void) {
    if (maxPriorityQueue->size() > 0) {
        BusRequest* next = maxPriorityQueue->back();
        unsigned int code = next->commandCode;
        if (code == BUSWRITE || code == DATA_RETURN_PROCESSOR) {
            bursting = true;
        } else {
            bursting = false;
        }
    } else if (busReqQueue->size() > 0) {
        BusRequest* next = busReqQueue->back();
        unsigned int code = next->commandCode;
        if (code == BUSWRITE || code == DATA_RETURN_PROCESSOR) {
            bursting = true;
        } else {
            bursting = false;
        }
    } else {
        bursting = false;   
    }
}

void CacheController::Tick(void) {
    // Transitions to the new state and calls the actions function
    transitionState();
    updateBusBurstRequest();
}

void CacheController::cancelBusRequest(unsigned int commandCode, unsigned long long payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if ((r->commandCode == commandCode) && (r->payload == payload)) {
            switch (commandCode) {
                case BUSREAD:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_BUSREAD                Payload: " << payload << endl;
                    break;
                case BUSREADX:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_BUSREADX               Payload: " << payload << endl;
                    break;
                case BUSWRITE:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_BUSWRITE               Payload: " << payload << endl;
                    break;
                case DATA_RETURN_MEMORY:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_DATA_RETURN_MEMORY     Payload: " << payload << endl;
                    break;
                case DATA_RETURN_PROCESSOR:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_DATA_RETURN_PROCESSOR  Payload: " << payload << endl;
                    break;
                case INVALIDATE:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_INVALIDATE             Payload: " << payload << endl;
                    break;
                case NULL_BURST:
                    cout << "Controller: " << getAddress() << " Code: CANCEL_NULL_BURST             Payload: " << payload << endl;
                    break;
                case SHAREME:
                    cout << "Controller: " << getAddress() << " Code: SHAREME                       Payload: " << payload << endl;
                    break;
            }
            busReqQueue->erase(it);
            return;
        }
    }
}

unsigned long CacheController::getStoreHits(void) {
    return store_hits;    
}

unsigned long CacheController::getStoreMisses(void) {
    return store_misses;
}

unsigned long CacheController::getLoadHits(void) {
    return load_hits;
}

unsigned long CacheController::getLoadMisses(void) {
    return load_misses;
}
