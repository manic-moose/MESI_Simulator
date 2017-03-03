#ifndef CACHECONTROLLER_H
#define CACHECONTROLLER_H

#include "BusNode.h"
#include "BusProtocol.h"
#include "Cache.h"
#include "Instruction.h"
#include "BusRequest.h"
#include <vector>
#include <assert.h>

using namespace std;

class CacheController : public BusNode {
    
public:
    
    CacheController(unsigned int adxLength, unsigned int setCount,
                   unsigned int linesPerSet, unsigned int bytesPerLine, unsigned int burstLength = 8) {
        cache =  new Cache(adxLength,setCount,linesPerSet,bytesPerLine);
        pendingInstructionFlag = false;
        awaitingBusRead = 0;
        busReqQueue = new vector<BusRequest*>;
        maxPriorityQueue = new vector<BusRequest*>;
        outgoingCommandWaitFlag = false;
        outgoingCommandWaitCode = 0;
        burstLen = burstLength;
        burstCounter = 0;
        bursting = false;
    }
    
    // Function to handle LOAD or STORE commands from the processor
    void handleMemoryAccess(Instruction* i);
        
    // BusNode functions
    bool requestsTransaction(void);
    bool requestsLock(void);
    BusRequest* initiateBusTransaction(void);
    bool requestsMaxPriority(void);
    
    
    bool hasPendingInstruction(void);
    
    // Tick for the cache controller. May be overridden
    // in derived classes if needed.
    // This implementation increments the current state
    // and issues any queued bus transactions if possible
    virtual void Tick(void);
    
    // State transition functions that must be implemented in
    // derived class
    virtual void callActionFunction(void) =0;
    virtual void transitionState(void) =0;
    
protected:
    
    // The cache the controller interfaces to
    Cache* cache;
    // Cache controlling methods
    
    bool outgoingCommandWaitFlag;
    unsigned int outgoingCommandWaitCode;
    
    // This will invalidate a line in the cache that maps
    // to memoryAdx. If the line is dirty, it will also
    // issue a bus write for the memory address.
    void invalidateCacheItem(unsigned long long memoryAdx);
    
    bool bursting;
    unsigned int burstLen;
    unsigned int burstCounter;
    
    
    Instruction* currentInstruction;  // The current LOAD/STORE instruction being handled
    bool pendingInstructionFlag;      // Indicates that the controller is busy with an instruction
    
    vector<BusRequest*>* busReqQueue;  // Queue of bus communications to transmit
    vector<BusRequest*>* maxPriorityQueue; // Queue for maximum priority requests
    
    BusRequest* nextToIssue;          // Holds the very next bus communication
    bool awaitingBusRead;             // Indicates that a bus read was issued, and data has not returned
    BusRequest* dispatchedBusRead;    // A copy of the bus read sent out so that the response can be recognized on return
    
    bool hasQueuedBusRequest(void);  // Indicates there are BusRequests waiting in the bus request queue for issue to the bus
    
    // Checks the bus request queue for any BusRequests that have the given properties
    bool hasBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload);
    
    // Returns the bust request from the bus request queue for any instructions with the given properties
    BusRequest* getBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload);
    
    // Deletes any bus requests from the bus request queue with the given properties
    void deleteBusRequestWithParams(unsigned int code, unsigned int target, unsigned int source, unsigned long long payload);
    
    // Adds new bus request to the end of the bus request queue
    void addNewBusRequest(BusRequest* r);
    void addNewMaxPriorityBusRequest(BusRequest* r);
    
    // Searches through the bus request queue/next to issue and cancels
    // the first request it find matching the given
    // command code and payload.
    void cancelBusRequest(unsigned int commandCode, unsigned long long payload);
    
   
    void updateBusBurstRequest(void);
    
};

#endif

