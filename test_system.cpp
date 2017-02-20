#include "System.h"
#include "Instruction.h"
#include <stdlib.h>

#define NUM_PROCESSORS 2
#define MEMORY_LATENCY 10
#define NUM_TICKS 100000

int main(void) {
    
    srand(time(NULL));
    
    System* s = new System(NUM_PROCESSORS, MESI_CONTROLLER_TYPE, MEMORY_LATENCY);
    
    for (int i = 0; i < NUM_TICKS; i++) {
        for (int pNum = 0; pNum < NUM_PROCESSORS; pNum++) {
            if (!(s->hasPendingInstructions(pNum))) {
                Instruction* inst = new Instruction;
                inst->OPCODE  = rand() % 3;
                inst->ADDRESS = rand() % 0xFFFFFFFF;
                if (inst->OPCODE == LOAD_CMD || inst->OPCODE == STORE_CMD) {
                    cout << "P" << pNum << " OPCODE: " << inst->OPCODE << "  Address: " << inst->ADDRESS << endl;
                }
                s->insertInstruction(inst,pNum);
            }
        }
        s->Tick();
    }        
    
    
    return 0;   
}