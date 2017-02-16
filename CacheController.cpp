#include "CacheController.h"

void CacheController::addNewBusRequest(BusRequest* r) {
    busReqQueue.insert(busReqQueue.begin(), r);   
}

bool CacheController::hasQueuedBusRequest(void) {
    return (busReqQueue.size() > 0);
}

bool CacheController::hasBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    BusRequest* r = getBusRequestWithParams(code,target,source,payload);
    return (r != NULL);
}

BusRequest* CacheController::getBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue.begin(); it < busReqQueue.end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            return r;   
        }
    }
    return NULL;
}

void CacheController::deleteBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned int payload) {
    for (vector<BusRequest*>::iterator it = busReqQueue.begin(); it < busReqQueue.end(); it++) {
        BusRequest* r = (*it);
        if (r->commandCode == code && r->targetAddress == target && r->sourceAddress == source && r->payload == payload) {
            busReqQueue.erase(it);
        }
    }
}

bool CacheController::requestsTransaction(void) {
    return pendingBusReqFlag;
}

void CacheController::issueNextBusRequest(void) {
    if(!hasQueuedBusRequest()) {
        return;
    }
    nextToIssue = busReqQueue.back();
    busReqQueue.pop_back();
    pendingBusReqFlag = 1;
    if (nextToIssue->commandCode == BUSREAD) {
        dispatchedBusRead = nextToIssue;
        awaitingBusRead = 1;
    }
}

BusRequest* CacheController::initiateBusTransaction(void) {
    pendingBusReqFlag = 0;
    return nextToIssue;
}

bool CacheController::hasPendingInstruction(void) {
    return pendingInstructionFlag;   
}

void CacheController::invalidateCacheItem(unsigned int memoryAdx) {
    CacheLine* line = cache->evictLineInSet(memoryAdx);
    if (line->isDirty()) {
        BusRequest* streamOut = new BusRequest;
        streamOut->commandCode   = BUSWRITE;
        streamOut->targetAddress = BROADCAST_ADX;
        streamOut->sourceAddress = getAddress();
        streamOut->payload       = memoryAdx;
        addNewBusRequest(streamOut);
    }
}

