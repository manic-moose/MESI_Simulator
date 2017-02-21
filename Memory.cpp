#include "Memory.h"
#include <iostream>
using namespace std;

void Memory::acceptBusTransaction(BusRequest* d) {
    if (d->commandCode == BUSREAD || d->commandCode == BUSREADX) {
        MemoryOperation* o = new MemoryOperation;
        o->address = d->payload;
        o->age = 0;
        o->returnAddress = d->sourceAddress;
        memTracker->push_back(o);
    } // Writes can be ignored - nothing to actually do.
    
    // Still may want to handle cancelling memory operations
    // that are serviced by cache to cache transfers
    
}

BusRequest* Memory::initiateBusTransaction(void) {
    BusRequest* nextRequest = busReqQueue->front();
    cout << "MEMORY Code: DATA_RETURN_MEMORY  Payload: " << nextRequest->payload << endl;
    busReqQueue->pop();
    memSendCounts++;
    return nextRequest;
}

bool Memory::requestsTransaction(void) {
    return (busReqQueue->size() > 0);
}

bool Memory::requestsLock(void) {
    return bursting;   
}

void Memory::Tick(void) {
    cout << "Current Memory Ops Count " << (memTracker->size()) << endl;
    if (bursting) {
        if (burstCounter == burstLen) {
            busReqQueue->push(burstRequest);
            bursting = false;
            burstCounter = 0;
        }
        if (hasLock()) {
            // Only increment if the lock has been granted
            // to simulate bursting out the memory
            burstCounter++;   
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
    unsigned int oldestIdx = getOldestOp();
    if (memTracker->size() > 0) {
        MemoryOperation* p = memTracker->at(oldestIdx);
        unsigned int oldestAge = p->age;
        if (oldestAge >= readLatency) {
            unsigned int adx = p->address;
            // This read operation is ready to send
            BusRequest* dataReturn    = new BusRequest;
            dataReturn->commandCode   = DATA_RETURN_MEMORY;
            dataReturn->targetAddress = p->returnAddress;
            dataReturn->sourceAddress = getAddress();
            dataReturn->payload       = adx;
            burstRequest = dataReturn;
            memTracker->erase(memTracker->begin() + oldestIdx);
            bursting = true;
            return;
        }
    }
}

unsigned int Memory::getOldestOp(void) {
    unsigned int oldestAge = 0;
    unsigned int index = 0;
    for (unsigned int i = 0; i < memTracker->size(); i++) {
        MemoryOperation* p = memTracker->at(i);
        if (p->age > oldestAge) {
            oldestAge = p->age;
            index = i;
        }
    }
    return index;
}

unsigned long Memory::getMemCount(void) {
    return memSendCounts;
}

