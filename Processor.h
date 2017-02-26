#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "BusNode.h"
#include "CacheController.h"
#include "BusRequest.h"
#include "Instruction.h"
#include "MI_Controller.h"
#include "MSI_Controller.h"
#include "MESI_Controller.h"
#include <assert.h>
#include <queue>
#include <iostream>

using namespace std;

#define MI_CONTROLLER_TYPE  0
#define MSI_CONTROLLER_TYPE 1
#define MESI_CONTROLLER_TYPE 2


class Processor : public BusNode {

private:
    
    CacheController* cacheController;
    queue<Instruction*>* instrQueue;
    
    unsigned long numberMemoryOperations;
    unsigned long totalWaitCycles;
    
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
    Instruction* getNextInstruction(void);
    
    bool nextInstIsMemoryOp(void);
    
    void reportState(void);


public:
    Processor(unsigned int controllerType) {
        instrQueue = new queue<Instruction*>;
        if (controllerType == MI_CONTROLLER_TYPE) {
            cacheController = new MI_Controller;
        } else if (controllerType == MSI_CONTROLLER_TYPE) {
            cacheController = new MSI_Controller;
        } else if (controllerType == MESI_CONTROLLER_TYPE) {
            cacheController = new MESI_Controller;
        } else {
            cout << "Invalid controller type. Must be one of MI_CONTROLLER_TYPE, MSI_CONTROLLER_TYPE, or MESI_CONTROLLER_TYPE." << endl;
        }
        numberMemoryOperations = 0;
    }

    // External Event Inputs
    void Tick(void);             // System Clock Tick
    
    // Bus Node Methods
    void acceptBusTransaction(BusRequest* d);
    BusRequest* initiateBusTransaction(void);
    bool requestsTransaction();
    bool requestsLock(void);
    void setAddress(unsigned int address);
    unsigned int getAddress(void);
    bool hasLock(void);
    void grantLock(void);
    void releaseLock(void);
    bool requestsMaxPriority(void);
    

    // Other Public Methods
    bool isIdle(void); // Indicates the processor is read to process the next instruction
    
    // Pushes a new instruction onto the queue.
    void insertInstruction(Instruction* i);
    
    // Returns the number of instructions currently queued
    unsigned int getInstructionCount(void);
    
    // Returns true if there are >0 queued instructions
    bool hasPendingInstructions(void);
    
    // Function to call to give the processor a new instruction. If
    // the instruction is not a memory instruction, then it is discarded
    void processInstruction(Instruction* i);
    
    unsigned long getTotalMemoryOps(void);
    
    double getAverageMemoryLatency(void);
    
};
#endif //PROCESSOR_H