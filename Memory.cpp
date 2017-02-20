#include "Memory.h"
#include <iostream>
using namespace std;

void Memory::acceptBusTransaction(BusRequest* d) {
    if (d->commandCode == BUSREAD || d->commandCode == BUSREADX) {
        MemoryOperation* o = new MemoryOperation;
        o->address = d->payload;
        o->age = 0;
        memTracker->push_back(o);
    } // Writes can be ignored - nothing to actually do.
    
    // Still may want to handle cancelling memory operations
    // that are serviced by cache to cache transfers
    
}

BusRequest* Memory::initiateBusTransaction(void) {
    BusRequest* nextRequest = busReqQueue->front();
    busReqQueue->pop();
    return nextRequest;
}

bool Memory::requestsTransaction(void) {
    return (busReqQueue->size() > 0);
}

bool Memory::requestsLock(void) {
    return bursting;   
}

void Memory::Tick(void) {
    if (bursting) {
        if (++burstCounter == burstLen) {
            busReqQueue->push(burstRequest);
            bursting = false;
            burstCounter = 0;
        }
    } else {
        updateReadAges();
        queueDataReturns();
    }
}

void Memory::updateReadAges(void) {
    for (unsigned int i = 0; i < memTracker->size(); i++) {
        MemoryOperation* p = memTracker->at(i);
        p->age = p->age + 1;
    }
}

void Memory::queueDataReturns(void) {
    for (unsigned int i = 0; i < memTracker->size(); i++) {
        MemoryOperation* p = memTracker->at(i);
        unsigned int age = p->age;
        if (age >= readLatency) {
            unsigned int adx = p->address;
            // This read operation is ready to send
            BusRequest* dataReturn    = new BusRequest;
            dataReturn->commandCode   = DATA_RETURN_MEMORY;
            dataReturn->targetAddress = BROADCAST_ADX;
            dataReturn->sourceAddress = getAddress();
            dataReturn->payload       = adx;
            burstRequest = dataReturn;
            memTracker->erase(memTracker->begin() + i);
            bursting = true;
            return;
        }
    }
}