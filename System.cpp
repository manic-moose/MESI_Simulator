#include "System.h"

using namespace std;

void System::Tick(void) {
    for (int i = 0; i < processors->size(); i++) {
           Processor* p = processors->at(i);
            p->Tick();
            cout << "Tick " << i << endl;
    }
    
    memory->Tick();
    cout << "Tick memory" << endl;
    interconnect->Tick();
    cout << "Tick interconnect" << endl;
}

void System::insertInstruction(Instruction* i, unsigned int processorNum) {
    assert(processorNum < processors->size());
    Processor* p = processors->at(processorNum);
    p->processInstruction(i);
}