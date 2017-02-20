#ifndef DUMMYNODE_H
#define DUMMYNODE_H

#include "BusNode.h"
#include "BusRequest.h"

class DummyNode : public BusNode {
    
public:
    DummyNode() : BusNode() {
        lockMe = false;
    }
    
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction(void);
    bool requestsLock(void);
    
    void sendNewTransaction(unsigned int adx, unsigned int code, unsigned int payload);
    
    void setRequestsLock(bool l);
    
private:
    
    unsigned int adx;
    bool hasRequest;
    
    bool lockMe;
    
    BusRequest* pendingReq;
    
    void setHasRequest(bool value);
    
    
};

#endif //DUMMYNODE_H