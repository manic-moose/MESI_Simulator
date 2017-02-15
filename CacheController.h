#ifndef CACHECONTROLLER_H
#define CACHECONTROLLER_H

#include "BusNode.h"
#include "StateMachine.h"
#include "Cache.h"
#include "Instruction.h"

class CacheController : public BusNode, public StateMachine {
    
public:
    
    CacheController(int stateCount, unsigned int adxLength, unsigned int setCount,
                   unsigned int linesPerSet, unsigned int bytesPerLine) : StateMachine(stateCount) {
       cache =  new Cache(adxLength,setCount,linesPerSet,bytesPerLine);
    }
    
    // Function to handle LOAD or STORE commands from the processor
    virtual void handleMemoryAccess(Instruction i) =0;
    
    // Function to indicate that the cache controller is busy with a LOAD/STORE operation
    bool memoryAccessPending(void);
        
private:
    
    Cache* cache;
    
    bool pendingMemoryAccess; // Flag to indicate that a LOAD/STORE is pending for processor
    void setPendingMemoryAccess(bool value);
    
};

#endif