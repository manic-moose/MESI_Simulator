#include "MI_Controller.h"
#include "Instruction.h"
#include "BusRequest.h"
#include "BusProtocol.h"
#include <iostream>

using namespace std;

int main (void) {
    
    MI_Controller* c = new MI_Controller;
    
    cout << "Has Pending Instruction: " << c->hasPendingInstruction() << endl;
    cout << "Requests Transaction: " << c->requestsTransaction() << endl;
    
    Instruction* i0 = new Instruction;
    
    i0->OPCODE = LOAD_CMD;
    i0->ADDRESS = 12345;
    
    c->Tick();
    
    c->Tick();
    
    c->Tick();

    c->handleMemoryAccess(i0);
    
    c->Tick();
    c->Tick();
    c->Tick();

    BusRequest* r = new BusRequest();
    r->commandCode = DATA_RETURN_MEMORY;
    r->targetAddress = BROADCAST_ADX;
    r->sourceAddress = 2;
    r->payload = 12345;

    BusRequest* k = new BusRequest();
    k->commandCode = DATA_RETURN_MEMORY;
    k->targetAddress = BROADCAST_ADX;
    k->sourceAddress = 2;
    k->payload = 12346;

    BusRequest* gotit = c->initiateBusTransaction();
    c->acceptBusTransaction(k);
    c->Tick();

    c->acceptBusTransaction(r);

    c->Tick();
    c->Tick();

    BusRequest* p = new BusRequest();
    p->commandCode = BUSREAD;
    p->targetAddress = BROADCAST_ADX;
    p->sourceAddress = 2;
    p->payload = 12345;

    c->Tick();
    c->acceptBusTransaction(p);
    c->Tick();
    c->Tick();
    
 
    return 0;
}
