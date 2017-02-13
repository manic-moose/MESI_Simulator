#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "StateMachine.h"
#include "BusRequest.h"
#include "BusNode.h"
#include <assert.h>
#include <map>

using namespace std;

#define _INTERCONNECT_STATE_COUNT_ 3

#define BROADCAST_ADX 0

class Interconnect : public StateMachine {
    
public:
    Interconnect() : StateMachine(_INTERCONNECT_STATE_COUNT_) {}
    
    // External Event Inputs
    void Tick(void);
    
    void addNode(BusNode* n, unsigned int address);
    void deleteNode(unsigned int address);
    bool hasNode(unsigned int address);
    BusNode* getNode(unsigned int address);
    
private:
    
    // Map of each address to a BusNode
    map <unsigned int, BusNode*> nodes;
    
    unsigned int adx_to_process;
    
    void ST_Idle(void);  // Idle waiting for requests
    void ST_Check(void); // Checking for pending requests on tick
    void ST_Busy(void);  // Busy processing request
    
    BEGIN_STATE_MAP
        STATE_MAP_ENTRY(&Interconnect::ST_Idle)
        STATE_MAP_ENTRY(&Interconnect::ST_Check)
        STATE_MAP_ENTRY(&Interconnect::ST_Busy)
    END_STATE_MAP
        
    enum E_States {
        ST_IDLE = 0,
        ST_CHECK,
        ST_BUSY
    };

};

#endif //INTERCONNECT_H