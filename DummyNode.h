#ifndef DUMMYNODE_H
#define DUMMYNODE_H

#include "BusNode.h"
#include "BusRequest.h"

class DummyNode : public BusNode {
    
public:
    DummyNode() : BusNode() {}
    
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction(void);
    bool requestsLock(void);
    
    void sendNewTransaction(unsigned int adx, unsigned int code, unsigned int payload);
    
private:
    
    unsigned int adx;
    bool hasRequest;
    
    BusRequest* pendingReq;
    
    void setHasRequest(bool value);
    
    
};

#endif //DUMMYNODE_H