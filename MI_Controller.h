#ifndef MI_CONTROLLER_H
#define MI_CONTROLLER_H

#include "CacheController.h"

#define _MI_CONTROLLER_STATES_ 3


#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32
#define STREAM_PENALTY 50

class MI_Controller : public CacheController {
    
public:
    
    MI_Controller() : CacheController(_MI_CONTROLLER_STATES_,ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE) {}
    
    void handleMemoryAccess(Instruction i);  
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction(void);
    
    
private:
    
    void ST_Idle(void);
    void Foo(void);
    void Bar(void);
    
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&MI_Controller::ST_Idle)
        STATE_MAP_ENTRY(&MI_Controller::Foo)
        STATE_MAP_ENTRY(&MI_Controller::Bar)
    END_STATE_MAP
        
    enum E_States {
        ST_IDLE = 0,
        ST_FOO,
        ST_BAR
    };
    
};

#endif //MI_CONTROLLER_H