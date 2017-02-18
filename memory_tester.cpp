#include "Memory.h"
#include "BusRequest.h"
#include <iostream>

using namespace std;

int main (void) {
 
    
    Memory* m = new Memory(3);
    cout << "Created new Memory" << endl;
    
    BusRequest* write = new BusRequest;
    
    write->commandCode = BUSWRITE;
    write->targetAddress = BROADCAST_ADX;
    write->sourceAddress = 1;
    write->payload = 857676;
    
    m->acceptBusTransaction(write);
    
    
    m->Tick();
    m->Tick();
    m->Tick();
    m->Tick();
    cout << "Requests transaction 1? = " << m->requestsTransaction() << endl;
    
    BusRequest* read = new BusRequest;
    
    read->commandCode = BUSREAD;
    read->targetAddress = BROADCAST_ADX;
    read->sourceAddress = 1;
    read->payload = 84578657;
    
    m->acceptBusTransaction(read);
    cout << "Requests transaction 2? = " << m->requestsTransaction() << endl;
    m->Tick();
    cout << "Requests transaction 3? = " << m->requestsTransaction() << endl;
    m->Tick();
    cout << "Requests transaction 4? = " << m->requestsTransaction() << endl;
    m->Tick();
    cout << "Requests transaction 5? = " << m->requestsTransaction() << endl;
    m->Tick();
    BusRequest* returnReq = m->initiateBusTransaction();
    cout << "Return command payload = " << returnReq->payload << " and OP TYPE = " << returnReq->commandCode << endl;
    cout << "Requests transaction 6? = " << m->requestsTransaction() << endl;
    m->Tick();
    cout << "Requests transaction 7? = " << m->requestsTransaction() << endl;
    m->Tick();
    
    
    
    
    return 0;
}