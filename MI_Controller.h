#ifndef MI_CONTROLLER_H
#define MI_CONTROLLER_H

#include "CacheController.h"
#include "StateMachine.h"

#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32
#define STREAM_PENALTY 50

using namespace std;

class MI_Controller : public CacheController {
    
private:

    // Flag used to prevent a cache update on a bus read
    // if, while waiting for data to return, another bus read
    // from a different node was broadcast for the same address
    bool noCacheUpdateOnRead;

    typedef enum E_STATES {
        IDLE_STATE,
        CHECKCACHE_STATE,
        BUSREAD_STATE,
        UPDATECACHE_STATE,
        COMPLETE_STATE
    } STATES;

    STATES currentState;
    
    STATES getNextState(void);
    void callActionFunction(void);
    void transitionState(void);
    
    // Define state transitions
    STATES Idle_Transition(void);
    STATES CheckCache_Transition(void);
    STATES BusRead_Transition(void);
    STATES UpdateCache_Transition(void);
    STATES Complete_Transition(void);
    
    // Functions executed during each tick while in
    // the associated state
    void Idle_Action(void);
    void CheckCache_Action(void);
    void BusRead_Action(void);
    void UpdateCache_Action(void);
    void Complete_Action(void);
    
public:
    
    MI_Controller() : CacheController(ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE) {
        noCacheUpdateOnRead = false;
    }
    
    
    void handleMemoryAccess(Instruction* i);  
    void acceptBusTransaction(BusRequest* d);
    
    void Tick(void);
    
};

#endif //MI_CONTROLLER_H
