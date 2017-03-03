#include "System.h"
#include "Instruction.h"
#include <stdlib.h>

#define NUM_PROCESSORS 8
#define MEMORY_LATENCY 100
#define NUM_TICKS 1000000

int main(void) {
    
    srand(time(NULL));
    
    // Instantiate the system with system parameters parameters
    System* s = new System(NUM_PROCESSORS, MI_CONTROLLER_TYPE, MEMORY_LATENCY);
    
    // Create simulation loop. We could base this on a fixed number of ticks
    // or loop until whatever the source of the instructions are is done.
    // In this case, instructions are being randomly generated, so we just run
    // for some fixed number of simulation ticks.
    for (int i = 0; i < NUM_TICKS; i++) {
        // Loop over each processor and give it an instruction.
        for (int pNum = NUM_PROCESSORS - 1; pNum >= 0; pNum--) {
            // This "hasPendingInstructions" check isn't strictly necessary,
            // but can be used as flow control. Basically this is just checking if the processor
            // is handling an instruction currently, and if it is, there is not need to give it a new one.
            // However, the processor is capable of receieving new instructions even if there are still
            // instructions pending, and they will just get queued up and execute once the previous
            // instruction finishes.
            if (!(s->hasPendingInstructions(pNum))) {
                // Here this is just creating a new instruction with a random
                // opcode (load/store/nop) and random address (within some range).
                Instruction* inst = new Instruction;
                inst->OPCODE  = rand() % 3;
                inst->ADDRESS = rand() % 0xFFFF;
                if (inst->OPCODE == LOAD_CMD || inst->OPCODE == STORE_CMD) {
                    cout << "Instruction Insert - " << "P" << pNum << " OPCODE: " << inst->OPCODE << "  Address: " << inst->ADDRESS << endl;
                }
                // Queues up the instruction for the processor
                s->insertInstruction(inst,pNum);
            }
        }
        // System simulation tick. This will be distributed to each component
        // to advance the simulation one cycle
        s->Tick();
    }
    
    // Just some extra cycles at the end to flush anything else out. 
    for (int i = 0; i < 5; i++) {
        s->Tick();   
    }
    
    // Prints out statistics for the system.
    s->reportMemoryOpStatistics();
    
    
    return 0;   
}