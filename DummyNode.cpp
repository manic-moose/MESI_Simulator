#include "DummyNode.h"
#include <iostream>

void DummyNode::acceptBusTransaction(BusRequest* d) {
    unsigned int adx = d->address;
    unsigned int code = d->requestCode;
    
    std::cout << "Got Request to my address: " << getAddress() << " Message Address: " << adx << " with code: " << code << std::endl;
}

BusRequest* DummyNode::initiateBusTransaction(void) {
    setHasRequest(false);
    return pendingReq;
}

bool DummyNode::requestsTransaction() {
    return hasRequest;
}

void DummyNode::setHasRequest(bool value) {
    hasRequest = value;
}
    
void DummyNode::sendNewTransaction(unsigned int adx, unsigned int code) {
    BusRequest* r = new BusRequest;
    r->address = adx;
    r->requestCode = code;
    setHasRequest(true);
    pendingReq = r;
}