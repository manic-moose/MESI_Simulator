#include "Memory.h"
#include <iostream>
using namespace std;

void Memory::acceptBusTransaction(BusRequest* d) {
    if (d->commandCode == BUSREAD) {
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

bool Memory::requestsTransaction() {
    return (busReqQueue->size() > 0);
}

void Memory::Tick(void) {
    updateReadAges();
    queueDataReturns();
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
            busReqQueue->push(dataReturn);
            // Erase this operation from the tracker
            memTracker->erase(memTracker->begin() + i);
        }
    }
}