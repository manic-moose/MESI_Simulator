#ifndef MESI_CONTROLLER_H
#define MESI_CONTROLLER_H

#include "CacheController.h"

#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32

using namespace std;

class MESI_Controller : public CacheController {
  
private:
    
    
    // Boolean flags used for state machine
    // behavior:
    
    // Indicates that data was returned from
    // a bus read operation (either from memory
    // or a processor, which impacts how MESI
    // bits are updated)
    bool gotDataReturnFromBusRead_Memory;
    bool gotDataReturnFromBusRead_Processor;
    
    typedef enum E_STATES {
        IDLE_STATE,
        CHECK_CACHE_LD_STATE,
        ISSUE_READ_STATE,
        UPDATE_CACHE_LD_STATE,
        CHECK_CACHE_ST_STATE,
        ISSUE_READX_STATE,
        ISSUE_INVALIDATE_STATE,
        UPDATE_CACHE_ST_STATE
    } STATES;
    
    STATES currentState;
    
    STATES getNextState(void);
    void callActionFunction(void);
    void transitionState(void);
    
    //Define state transition functions
    STATES Idle_Transition(void);
    STATES CheckCacheLoad_Transition(void);
    STATES IssueRead_Transition(void);
    STATES UpdateCacheLoad_Transition(void);
    STATES CheckCacheStore_Transition(void);
    STATES IssueReadX_Transition(void);
    STATES IssueInvalidate_Transition(void);
    STATES UpdateCacheStore_Transition(void);
    
    //Define state action functions
    void Idle_Action(void);
    void CheckCacheLoad_Action(void);
    void IssueRead_Action(void);
    void UpdateCacheLoad_Action(void);
    void CheckCacheStore_Action(void);
    void IssueReadX_Action(void);
    void IssueInvalidate_Action(void);
    void UpdateCacheStore_Action(void);
   
    
public:
    
    MESI_Controller() : CacheController(ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE) {
        gotDataReturnFromBusRead_Memory = false;
        gotDataReturnFromBusRead_Processor = false;
    }
    
};


#endif //MESI_CONTROLLER_H