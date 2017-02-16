#include "MI_Controller.h"

#include <iostream>

using namespace std;

void MI_Controller::handleMemoryAccess(Instruction* i) {
    assert(!hasPendingInstruction());
    currentInstruction = i;
    pendingInstructionFlag = 1;
}

void MI_Controller::acceptBusTransaction(BusRequest* d) {
    unsigned int cmd       = d->commandCode;
    unsigned int target    = d->targetAddress;
    unsigned int source    = d->sourceAddress;
    unsigned int memoryAdx = d->payload;
    cout << "Accepted BUS Transaction for " << memoryAdx;
    if (awaitingBusRead) {
        if (dispatchedBusRead != NULL && IS_DATA_RETURN(cmd) && (dispatchedBusRead->payload == memoryAdx)) {
            // The received bus comm was a data return and the address
            // matched the address we were waiting for
            cout << " Got the data we were waiting for" << endl;
            awaitingBusRead = 0;
        } else {
            if (cmd == BUSREAD) {
                if (dispatchedBusRead != NULL && (dispatchedBusRead->payload == memoryAdx)) {
                    // This was a read to the same address that we are waiting for.
                    // This means that when we finally get the data value returned,
                    // we will need to not insert it into the cache
                    cout << " Saw another read for address we want " << endl;
                    noCacheUpdateOnRead = true;
                } else if (cache->contains(memoryAdx)) {
                    // There was a read to an address that we maintain in the cache, and
                    // it must be invalidated. If it is dirty, it must be streamed out
                    cout << "Invalidating " << endl;
                    invalidateCacheItem(memoryAdx);
                }
            } else {
                cout << " Ignored " << endl;
                // We can ignore the bus traffic
            }
        }
    } else if (cache->contains(memoryAdx)) {
        cout << " Invalidating " << endl;
        // There was a read to an address that we maintain in the cache, and
        // it must be invalidated. If it is dirty, it must be streamed out
        invalidateCacheItem(memoryAdx);
    } else {
        cout << " Ignored " << endl;
        // We can ignore the bus traffic
    }
}
    
void MI_Controller::Idle_Action(void) {
    cout << "MI Controller Idle Action" << endl;
}

void MI_Controller::CheckCache_Action(void) {
    cout << "MI Controller CheckCache Action" << endl;
}

void MI_Controller::BusRead_Action(void) {
    cout << "MI Controller BusRead Action" << endl;
    if (awaitingBusRead) {
        
    } else {
        BusRequest* readRequest = new BusRequest;
        readRequest->commandCode   = BUSREAD;
        readRequest->targetAddress = BROADCAST_ADX;
        readRequest->sourceAddress = getAddress();
        readRequest->payload       = currentInstruction->ADDRESS;
        addNewBusRequest(readRequest);
        awaitingBusRead = 1;
    }
}

void MI_Controller::UpdateCache_Action(void) {
    if (noCacheUpdateOnRead) {
        // Indicates that a bus read occurred to the same
        // address while this node was waiting for data
        noCacheUpdateOnRead = false;
    } else {
        unsigned int adx = currentInstruction->ADDRESS;
        if (cache->isFull(adx)) {
            // The set this address belongs to is full,
            // so a line will need to be evicted and
            // potentially streamed back out to memory
            CacheLine* evictedLine = cache->evictLineInSet(adx);
            if (evictedLine->isDirty()) {
                // Need to write this line back to memory
            }
        }
        cache->insertLine(adx);
    }
}

void MI_Controller::Complete_Action(void) {
    pendingInstructionFlag = false;
}

MI_Controller::STATES MI_Controller::Idle_Transition(void) {
    if (hasPendingInstruction()) {
        return CHECKCACHE_STATE;
    } else {
        return IDLE_STATE;
    }
}

MI_Controller::STATES MI_Controller::CheckCache_Transition (void) {
    assert(currentInstruction != NULL);
    unsigned int adx = currentInstruction->ADDRESS;
    bool inCache = cache->contains(adx);
    if (inCache) {
        // Cache Hit
        return IDLE_STATE;
    } else {
        // Cache Miss
        return BUSREAD_STATE;
    }
}

MI_Controller::STATES MI_Controller::BusRead_Transition (void) {
    if (awaitingBusRead) {
        return BUSREAD_STATE;
    } else {
        assert(currentInstruction != NULL);
        return UPDATECACHE_STATE;
    }
}
    
MI_Controller::STATES MI_Controller::UpdateCache_Transition (void) {
    return COMPLETE_STATE;
}

MI_Controller::STATES MI_Controller::Complete_Transition (void) {
    return IDLE_STATE;
}

void MI_Controller::Tick(void) {
    transitionState();
    callActionFunction();
}

MI_Controller::STATES MI_Controller::getNextState(void) {
    switch(currentState) {
        case IDLE_STATE:
            return Idle_Transition();
        case CHECKCACHE_STATE:
            return CheckCache_Transition();
        case BUSREAD_STATE:
            return BusRead_Transition();
        case UPDATECACHE_STATE:
            return UpdateCache_Transition();
        case COMPLETE_STATE:
            return Complete_Transition();
    }
}

void MI_Controller::transitionState(void) {
    STATES nextState = getNextState();
    currentState = nextState;
    issueNextBusRequest();
}

void MI_Controller::callActionFunction(void) {
    switch(currentState) {
        case IDLE_STATE:
            Idle_Action();
            break;
        case CHECKCACHE_STATE:
            CheckCache_Action();
            break;
        case BUSREAD_STATE:
            BusRead_Action();
            break;
        case UPDATECACHE_STATE:
            UpdateCache_Action();
            break;
        case COMPLETE_STATE:
             Complete_Action();
             break;
    }
}

