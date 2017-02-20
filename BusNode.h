#ifndef BUSNODE_H
#define BUSNODE_H

#include "BusRequest.h"
#include <iostream>

class BusNode {

public:
    
    // Called by bus controller such that BusNode receieves
    // a transaction that has been addressed to it
    virtual void acceptBusTransaction(BusRequest* d) = 0;  
    
    // Called by bus controller such that this node
    // will send a bus transaction if any are pending
    virtual BusRequest* initiateBusTransaction(void) = 0;
    
    // Indicates that this node has a pending transaction to send
    virtual bool requestsTransaction(void) =0;
    
    // Indicates that this BusNode wants to lock control
    // of the bus. If true, the next time this node is
    // checked for service, it will gain a lock on the bus.
    // It will then be the only bus allowed to initiate bus transactions
    // while the lock is maintained. The lock will be maintained as long
    // as requestsLock returns true.
    virtual bool requestsLock(void) =0;
    
    // Returns the address of this node
    unsigned int getAddress(void);
    void setAddress(unsigned int address);
    
private:
    
    unsigned int address;
    
    
};

#endif //BUSNODE_H

