#include "System.h"
#include "Instruction.h"
#include <stdlib.h>

#define NUM_PROCESSORS 16
#define MEMORY_LATENCY 5
#define NUM_TICKS 100000

int main(void) {
    
    srand(time(NULL));
    
    System* s = new System(NUM_PROCESSORS, MESI_CONTROLLER_TYPE, MEMORY_LATENCY);
    
    for (int i = 0; i < NUM_TICKS; i++) {
        for (int pNum = NUM_PROCESSORS - 1; pNum >= 0; pNum--) {
            if (!(s->hasPendingInstructions(pNum))) {
                Instruction* inst = new Instruction;
                inst->OPCODE  = rand() % 3;
                inst->ADDRESS = rand() % 10000;
                if (inst->OPCODE == LOAD_CMD || inst->OPCODE == STORE_CMD) {
                    cout << "P" << pNum << " OPCODE: " << inst->OPCODE << "  Address: " << inst->ADDRESS << endl;
                }
                s->insertInstruction(inst,pNum);
            }
        }
        s->Tick();
    }
    
    for (int i = 0; i < 200; i++) {
        s->Tick();   
    }
    s->reportMemoryOpStatistics();
    
    
    return 0;   
}