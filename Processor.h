#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "StateMachine.h"

#define _PROCESSOR_STATE_COUNT_ 4

class Processor : public StateMachine, public BusNode {

public:
    Processor() : StateMachine(_PROCESSOR_STATE_COUNT_) {}

    // External Event Inputs
    void Tick(void);      // System Clock Tick
    void MemReturn(void);                 // Memory Operation Completed
    void Reset(void);                     // System Reset
    
    // Bus Node Methods
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction();
    
private:
    
    // State Machine State Functions
    void ST_Idle(void);
    void ST_CheckOP(void);
    void ST_ProcessMemOp(void);
    void ST_ProcessDoneWait(void);
    
    // State Map to Define State Function Order
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&Processor::ST_Idle)
        STATE_MAP_ENTRY(&Processor::ST_CheckOP)
        STATE_MAP_ENTRY(&Processor::ST_ProcessMemOp)
        STATE_MAP_ENTRY(&Processor::ST_ProcessDoneWait)
    END_STATE_MAP
        
    enum E_States {
        ST_IDLE = 0,
        ST_CHECK_OP,
        ST_PROCESS_MEM_OP,
        ST_PROCESS_DONE_WAIT
    };

};
#endif //PROCESSOR_H