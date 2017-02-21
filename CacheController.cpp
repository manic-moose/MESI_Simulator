#include "CacheController.h"

void CacheController::addNewBusRequest(BusRequest* r) {
    busReqQueue->insert(busReqQueue->begin(), r);   
}

bool CacheController::hasQueuedBusRequest(void) {
    return (busReqQueue->size() > 0);
}

bool CacheController::hasBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    BusRequest* r = getBusRequestWithParams(code,target,source,payload);
    return (r != NULL);
}

BusRequest* CacheController::getBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            return r;   
        }
    }
    return NULL;
}

void CacheController::deleteBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            busReqQueue->erase(it);
        }
    }
}

bool CacheController::requestsTransaction(void) {
    return busReqQueue->size() > 0;
}

bool CacheController::requestsLock(void) {
    return false;
    //return requestsTransaction();   
}

BusRequest* CacheController::initiateBusTransaction(void) {
    nextToIssue = busReqQueue->back();
    busReqQueue->pop_back();
    unsigned int code = nextToIssue->commandCode;
    unsigned int payload = nextToIssue->payload;
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
    }
    if ((nextToIssue->commandCode == BUSREAD) || nextToIssue->commandCode == BUSREADX) {
        dispatchedBusRead = nextToIssue;
        awaitingBusRead = true;
    }
    return nextToIssue;
}

bool CacheController::hasPendingInstruction(void) {
    return pendingInstructionFlag;   
}

void CacheController::invalidateCacheItem(unsigned int memoryAdx) {
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

void CacheController::handleMemoryAccess(Instruction* i) {
    assert(!hasPendingInstruction());
    
    currentInstruction = i;
    unsigned int address = cache->getLineAlignedAddress(i->ADDRESS);
    if (i->OPCODE == STORE_CMD) {
        cout << "Controller: " << getAddress() << " Code: INSTRUCTION STORE  Payload: " << address << endl;
    } else if (i->OPCODE == LOAD_CMD) {
        cout << "Controller: " << getAddress() << " Code: INSTRUCTION LOAD  Payload: " << address << endl;
    }
    pendingInstructionFlag = true;
}

void CacheController::Tick(void) {
    // Transitions to the new state and calls the actions function
    transitionState();
}

void CacheController::cancelBusRequest(unsigned int commandCode, unsigned int payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue->begin(); it < busReqQueue->end(); it++) {
        BusRequest* r = (*it);
        if ((r->commandCode == commandCode) && (r->payload == payload)) {
            busReqQueue->erase(it);
            return;
        }
    }
}