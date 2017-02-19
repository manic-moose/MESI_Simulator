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
        // No bus requests on the queue
        return;
    }
    // Ensure no requests are currently being waited on.
    // This function should only be called if the previous
    // bus request has gone out already
    assert(!requestsTransaction());
    nextToIssue = busReqQueue.back();
    busReqQueue.pop_back();
    pendingBusReqFlag = true;
    if ((nextToIssue->commandCode == BUSREAD) || nextToIssue->commandCode == BUSREADX) {
        dispatchedBusRead = nextToIssue;
        awaitingBusRead = true;
    }
}

BusRequest* CacheController::initiateBusTransaction(void) {
    pendingBusReqFlag = false;
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
    pendingInstructionFlag = true;
}

void CacheController::Tick(void) {
    // Transitions to the new state and calls the actions function
    transitionState();
    // Attempts to issue 
    if (!requestsTransaction()) {
        issueNextBusRequest();
    }
}
