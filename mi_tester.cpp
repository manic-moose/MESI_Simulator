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
    r->payload = 12320;

    BusRequest* k = new BusRequest();
    k->commandCode = DATA_RETURN_MEMORY;
    k->targetAddress = BROADCAST_ADX;
    k->sourceAddress = 2;
    k->payload = 12800;

    BusRequest* gotit = c->initiateBusTransaction();
    c->acceptBusTransaction(k);
    c->Tick();
    cout << "Sending expected data" << endl;
    c->acceptBusTransaction(r);

    c->Tick();
    c->Tick();

    BusRequest* p = new BusRequest();
    p->commandCode = BUSREAD;
    p->targetAddress = BROADCAST_ADX;
    p->sourceAddress = 2;
    p->payload = 12320;

    c->Tick();
    c->acceptBusTransaction(p);
    c->Tick();
    c->Tick();
    c->Tick();
    c->acceptBusTransaction(p);
    c->Tick();
    
    
    Instruction* i1 = new Instruction;
    
    i1->OPCODE = STORE_CMD;
    i1->ADDRESS = 59392;
    cout << "Before" << endl;
    c->handleMemoryAccess(i1);
    cout << "After" << endl;
    
    c->Tick();
    c->Tick();
    c->Tick();
    c->Tick();
    
    BusRequest* q = new BusRequest();
    q->commandCode = BUSREAD;
    q->targetAddress = BROADCAST_ADX;
    q->sourceAddress = 2;
    q->payload = 59392;
    
    c->acceptBusTransaction(q);
    c->Tick();
    
    BusRequest* j = new BusRequest();
    j->commandCode = DATA_RETURN_MEMORY;
    j->targetAddress = BROADCAST_ADX;
    j->sourceAddress = 2;
    j->payload = 59392;
    
    c->acceptBusTransaction(j);
    c->Tick();
    c->Tick();
    c->Tick();
    c->Tick();
    
    return 0;
}
