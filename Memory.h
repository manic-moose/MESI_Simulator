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
    
    unsigned int burstPayload;
    unsigned int burstCounter;
    bool bursting;
    unsigned int burstLen;
    
    unsigned int readLatency;
    vector<MemoryOperation*>* memTracker;
    queue<BusRequest*>* busReqQueue;
    
    void updateReadAges(void);
    
    void queueDataReturns(void);
    
public:
    
    Memory(unsigned int latency, unsigned int burstLength) {
        readLatency = latency;
        memTracker = new vector<MemoryOperation*>;
        busReqQueue = new queue<BusRequest*>;
        burstPayload = 0;
        bursting = false;
        burstLen = burstLength;
        burstCounter = 0;
    }
    
    void acceptBusTransaction(BusRequest* d);
        
    BusRequest* initiateBusTransaction(void);

    bool requestsTransaction(void);
    bool requestsLock(void);
    
    void Tick(void);

};