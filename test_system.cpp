#include "System.h"
#include "Instruction.h"
#include <stdlib.h>

#define NUM_PROCESSORS 1

#define NUM_TICKS 1000

int main(void) {
    
    srand(time(NULL));
    
    System* s = new System(NUM_PROCESSORS, MESI_CONTROLLER_TYPE, 4);
    
    for (int i = 0; i < NUM_TICKS; i++) {
        s->Tick();
        for (int pNum = 0; pNum < NUM_PROCESSORS; pNum++) {
            Instruction* inst = new Instruction;
            inst->OPCODE  = rand() % 3;
            inst->ADDRESS = rand() % 0xFFFFFFFF;
            cout << "P" << pNum << " OPCODE: " << inst->OPCODE << "  Address: " << inst->ADDRESS << endl;
            s->insertInstruction(inst,pNum);
        }
    }        
    
    
    return 0;   
}