#include "BusNode.h"
#include "BusRequest.h"
#include "BusProtocol.h"
#include <map>
#include <queue>

using namespace std;

class Memory : public BusNode {
    
    struct MemoryOperation {
        unsigned int address;
        unsigned int age;
    };
    
private:
    
    unsigned int readLatency;
    vector<MemoryOperation*>* memTracker;
    queue<BusRequest*>* busReqQueue;
    
    void updateReadAges(void);
    
    void queueDataReturns(void);
    
public:
    
    Memory(unsigned int latency) {
        readLatency = latency;
        memTracker = new vector<MemoryOperation*>;
        busReqQueue = new queue<BusRequest*>;
    }
    
    void acceptBusTransaction(BusRequest* d);
        
    BusRequest* initiateBusTransaction(void);

    bool requestsTransaction(void);
    bool requestsLock(void);
    
    void Tick(void);

};