#ifndef MI_CONTROLLER_H
#define MI_CONTROLLER_H

#include "CacheController.h"
#include "StateMachine.h"
#include <string>

#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32
#define STREAM_PENALTY 50

using namespace std;

class MI_Controller : public CacheController {
    
private:
    
    // Define state transitions
    string Idle_Transition(void);
    string CheckCache_Transition(void);
    string BusRead_Transition(void);
    string UpdateCache_Transition(void);
    string Complete_Transition(void);
    
    // Functions executed during each tick while in
    // the associated state
    void Idle_Action(void);
    void CheckCache_Action(void);
    void BusRead_Action(void);
    void UpdateCache_Action(void);
    void Complete_Action(void);
    
public:
    
    MI_Controller() : CacheController(ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE) {
    }
    
    void init (void);
    
    void handleMemoryAccess(Instruction* i);  
    void acceptBusTransaction(BusRequest* d);
    
    void Tick(void);
    
};

#endif //MI_CONTROLLER_H