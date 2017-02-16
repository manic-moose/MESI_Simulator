#ifndef BUSNODE_H
#define BUSNODE_H

#include "BusRequest.h"

class BusNode {

public:
    
    // Called by bus controller such that BusNode receieves
    // a transaction that has been addressed to it
    virtual void acceptBusTransaction(BusRequest* d) = 0;  
    
    // Called by bus controller such that this node
    // will send a bus transaction if any are pending
    virtual BusRequest* initiateBusTransaction(void) = 0;
    
    // Indicates that this node has a pending transaction to send
    virtual bool requestsTransaction() =0;
    
    // Returns the address of this node
    unsigned int getAddress(void);
    void setAddress(unsigned int address);
    
private:
    
    unsigned int address;
    
    
};

#endif //BUSNODE_H

