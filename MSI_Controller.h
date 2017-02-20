#ifndef MSI_CONTROLLER_H
#define MSI_CONTROLLER_H

#include "CacheController.h"

#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32

using namespace std;

class MSI_Controller : public CacheController {
  
private:
    
    
    // Boolean flags used for state machine behavior:
    
    // These flags indicate that data was returned from
    // a bus read/readx operation (either from memory
    // or a processor, which impacts how MSI
    // bits are updated)
    bool data_BusRead_Memory;     // Normal Return from memory
    bool data_BusRead_Processor;  // Normal cache-to-cache transfer
    bool snoopedData_Writeback;      // Snooped data from cache writeback
    // Just an indicator that the current required bus read was already queued
    // so that in the BUSREAD/BUSREADX states, multiple reads are not issued
    bool queued_BusRead;
    
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
    
    // Broadcast a bus request with given command code and payload
    void queueBusCommand(unsigned int command, unsigned int payload);
   
    
public:
    
    MSI_Controller() : CacheController(ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE) {
        data_BusRead_Memory    = false;
        data_BusRead_Processor = false;
        queued_BusRead         = false;
        snoopedData_Writeback  = false;
    }
    
    // BusNode receiver function
    void acceptBusTransaction(BusRequest* d);
    
};

#endif //MSI_CONTROLLER_H
