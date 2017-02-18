#include "Processor.h"
#include "Interconnect.h"
#include "Memory.h"
#include <vector>

using namespace std;

class System {

private:
    
    vector<Processor*>* processors;
    Memory* memory;
    Interconnect* interconnect;
    
    
public:
    
    System(unsigned int numProcessors, unsigned int controllerType, unsigned int memoryLatency) {
        processors = new vector<Processor*>;
        memory = new Memory(memoryLatency);
        interconnect = new Interconnect;
        
        for(int i = 0; i < numProcessors; i++) {
            Processor* p = new Processor(controllerType);
            processors->push_back(p);
            interconnect->addNode(p, i+1);
        }
        
        interconnect->addNode(memory, numProcessors + 1);
    }
    
    void insertInstruction(Instruction* i, unsigned int processorNum);
    
    
    void Tick(void);

};