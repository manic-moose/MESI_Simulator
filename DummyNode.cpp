#include "DummyNode.h"
#include <iostream>
using namespace std;

void DummyNode::acceptBusTransaction(BusRequest* d) {
    unsigned int commandCode = d->commandCode;
    unsigned int targetAdx = d->targetAddress;
    unsigned int sourceAdx = d->sourceAddress;
    unsigned int payload   = d->payload;
    
    std::cout << " Got Request to my address: " << getAddress() << " From Source: " << sourceAdx <<" with code: " << commandCode << " and payload " << payload << std::endl;
}

BusRequest* DummyNode::initiateBusTransaction(void) {
    setHasRequest(false);
    return pendingReq;
}

bool DummyNode::requestsTransaction(void) {
    return hasRequest;
}

bool DummyNode::requestsLock(void) {
    return lockMe;
}

void DummyNode::setHasRequest(bool value) {
    hasRequest = value;
}
    
void DummyNode::sendNewTransaction(unsigned int adx, unsigned int code, unsigned int payload) {
    BusRequest* r = new BusRequest;
    r->sourceAddress = getAddress();
    r->targetAddress = adx;
    r->commandCode = code;
    r->payload = payload;
    setHasRequest(true);
    pendingReq = r;
}

void DummyNode::setRequestsLock(bool l) {
    lockMe = l;
}