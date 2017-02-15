#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "StateMachine.h"
#include "BusNode.h"
#include "CacheController.h"
#include "BusRequest.h"
#include "Instruction.h"
#include "MI_Controller.h"
#include <assert.h>
#include <queue>

using namespace std;

#define _PROCESSOR_STATE_COUNT_ 4


class Processor : public StateMachine, public BusNode {

public:
    Processor() : StateMachine(_PROCESSOR_STATE_COUNT_) {
        cacheController = new MI_Controller;
    }

    // External Event Inputs
    void Tick(void);      // System Clock Tick
    void MemReturn(void);                 // Memory Operation Completed
    void Reset(void);                     // System Reset
    
    // Bus Node Methods
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction();

    // Other Public Methods
    bool isIdle(void); // Indicates the processor is read to process the next instruction
    
    //
    void insertInstruction(Instruction i);
    unsigned int getInstructionCount(void);
    bool hasPendingInstructions(void);
    
private:
    
    CacheController* cacheController;
    
    queue<Instruction>* instrQueue;
    
    // State Machine State Functions
    void ST_Idle(void);
    void ST_CheckOpType(void);
    void ST_MemoryReturnWait(void);
    
    // State Map to Define State Function Order
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&Processor::ST_Idle)
        STATE_MAP_ENTRY(&Processor::ST_CheckOpType)
        STATE_MAP_ENTRY(&Processor::ST_MemoryReturnWait)
    END_STATE_MAP
        
    enum E_States {
        ST_IDLE = 0,
        ST_CHECK_OP_TYPE,
        ST_MEMORY_RETURN_WAIT
    };
    
    // Other private methods
    Instruction getNextInstruction(void);

};
#endif //PROCESSOR_H