#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "BusNode.h"
#include "CacheController.h"
#include "BusRequest.h"
#include "Instruction.h"
#include "MI_Controller.h"
#include <assert.h>
#include <queue>

using namespace std;

#define MI_CONTROLLER_TYPE  0
#define MSI_CONTROLLER_TYPE 1
#define MESI_CONTROLLE_TYPE 2


class Processor : public BusNode {

private:
    
    CacheController* cacheController;
    queue<Instruction*>* instrQueue;
    
    typedef enum E_STATES {
        IDLE_STATE,
        CHECKOPTYPE_STATE,
        MEMORYRETURNWAIT_STATE
    } STATES;
    
    STATES currentState;
    void callActionFunction(void);
    void transitionState(void);
    STATES getNextState(void);
    
    // State Machine Transition Functions
    STATES Idle_Transition(void);
    STATES CheckOpType_Transition(void);
    STATES MemoryReturnWait_Transition(void);
    
    void Idle_Action(void);
    void CheckOpType_Action(void);
    void MemoryReturnWait_Action(void);
       
    // Other private methods
    
    // Returns the next instruction from the queue
    // and removes it from the queue
    Instruction getNextInstruction(void);
    
    bool nextInstIsMemoryOp(void);


public:
    Processor(unsigned int controllerType) {
        if (controllerType == MI_CONTROLLER_TYPE) {
            cacheController = new MI_Controller;
        } else if (controllerType == MSI_CONTROLLER_TYPE) {
            cout << "MSI Not yet enabled" << endl;
            exit(1);
        } else if (controllerType == MESI_CONTROLLER_TYPE) {
            cout << "MESI Not yet enabled" << endl;
            exit(1);
        } else {
            cout << "Invalid controller type. Must be one of MI_CONTROLLER_TYPE, MSI_CONTROLLER_TYPE, or MESI_CONTROLLER_TYPE." << endl;
            exit(1);
        }
    }

    // External Event Inputs
    void Tick(void);             // System Clock Tick
    
    // Bus Node Methods
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction();

    // Other Public Methods
    bool isIdle(void); // Indicates the processor is read to process the next instruction
    
    // Pushes a new instruction onto the queue.
    void insertInstruction(Instruction* i);
    
    // Returns the number of instructions currently queued
    unsigned int getInstructionCount(void);
    
    // Returns true if there are >0 queued instructions
    bool hasPendingInstructions(void);
    
};
#endif //PROCESSOR_H