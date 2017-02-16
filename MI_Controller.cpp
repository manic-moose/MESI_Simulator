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
    if (awaitingBusRead) {
        assert(dispatchedBusRead != NULL);
        if (IS_DATA_RETURN(cmd) && (dispatchedBusRead->payload == memoryAdx)) {
            // The received bus comm was a data return and the address
            // matched the address we were waiting for
            awaitingBusRead = 0;
        } else {
            if (cmd == BUSREAD) {
                if ((dispatchedBusRead->payload == memoryAdx)) {
                    // This was a read to the same address that we are waiting for.
                    // This means that when we finally get the data value returned,
                    // we will need to not insert it into the cache
                } else if (cache->contains(memoryAdx)) {
                    // There was a read to an address that we maintain in the cache, and
                    // it must be invalidated. If it is dirty, it must be streamed out
                    invalidateCacheItem(memoryAdx);
                }
            }
        }
    } else if (cache->contains(memoryAdx)) {
        // There was a read to an address that we maintain in the cache, and
        // it must be invalidated. If it is dirty, it must be streamed out
        invalidateCacheItem(memoryAdx);
    }
}
    

void MI_Controller::Idle_Action(void) {
    cout << "MI Controller Idle" << endl;
}

void MI_Controller::CheckCache_Action(void) {
    cout << "MI Controller CheckCache" << endl;
}

void MI_Controller::BusRead_Action(void) {
    cout << "MI Controller BusRead" << endl;
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
    cout << "MI Controller UpdateCache" << endl;
}

void MI_Controller::Complete_Action(void) {
    cout << "MI Controller Complete" << endl;
}


string MI_Controller::Idle_Transition(void) {
    cout << "IDLE TRANS FUNCTION CALLED" << endl;
    cout << "HAS PENDING??" << hasPendingInstruction() << endl;
    cout << "Flag is : " << pendingInstructionFlag << endl;
    if (hasPendingInstruction()) {
        cout << "CHECK CACHE?" << endl;
        return "CheckCache";
    } else {
        cout << "Going back to idle still..." << endl;
        return "Idle";
    }
}


string MI_Controller::CheckCache_Transition (void) {
    if (currentInstruction != NULL && cache->contains(currentInstruction->ADDRESS)) {
        // Cache Hit
        return "Complete";
    } else {
        // Cache Miss
        return "BusRead";
    }
}

string MI_Controller::BusRead_Transition (void) {
    if (awaitingBusRead) {
        return "BusRead";
    } else {
        if((currentInstruction != NULL) && (currentInstruction->OPCODE == STORE_CMD)) {
            return "UpdateCache";
        } else {
            return "Complete";
        }
    }
}
    
string MI_Controller::UpdateCache_Transition (void) {
    return "Complete";
}

string MI_Controller::Complete_Transition (void) {
    return "Idle";
}

void MI_Controller::Tick(void) {
    cout << "Tick" << endl;
    Trigger();   
}

void MI_Controller::init (void) {
    addNewState("Idle", (TransitionFunction)&MI_Controller::Idle_Transition, (ActionFunction)(NULL));   
    addNewState("CheckCache", (TransitionFunction)&MI_Controller::CheckCache_Transition, (ActionFunction)(NULL));   
    addNewState("BusRead", (TransitionFunction)&MI_Controller::BusRead_Transition, (ActionFunction)(NULL));   
    addNewState("UpdateCache", (TransitionFunction)&MI_Controller::UpdateCache_Transition, (ActionFunction)(NULL));   
    addNewState("Complete", (TransitionFunction)&MI_Controller::Complete_Transition, (ActionFunction)(NULL));   
    initialize("Idle");
}