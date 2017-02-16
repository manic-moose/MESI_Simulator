#include "MI_Controller.h"
#include "Instruction.h"
#include <iostream>

using namespace std;

int main (void) {
    
    MI_Controller* c = new MI_Controller;
    c->init();
    
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    cout << "Requests Transaction: " << c->requestsTransaction() << endl;
    
    Instruction* i0 = new Instruction;
    
    i0->OPCODE = LOAD_CMD;
    i0->ADDRESS = 12345;
    
    c->Tick();
    
    c->Tick();
    
    c->Tick();
    
    cout << "Inserting memory LOAD" << endl;
    c->handleMemoryAccess(i0);
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    cout << "Requests Bus Transaction: " << c->requestsTransaction() << endl;
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    c->Tick();
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    c->Tick();
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
 
    return 0;
}