#ifndef INTERCONNECT_H
#define INTERCONNECT_H

#include "BusRequest.h"
#include "BusNode.h"
#include "BusProtocol.h"
#include <assert.h>
#include <map>
#include <vector>

using namespace std;

class Interconnect {
    
public:
    Interconnect() {
        priorityQueue = new vector<unsigned int>;
    }
    
    // External Event Inputs
    void Tick(void);
    
    void addNode(BusNode* n, unsigned int address);
    void deleteNode(unsigned int address);
    bool hasNode(unsigned int address);
    BusNode* getNode(unsigned int address);
    unsigned int getNodeCount(void);
    
private:
    
    // Map of each address to a BusNode
    map <unsigned int, BusNode*> nodes;
    
    // list of the addresses to maintain priority
    vector<unsigned int>* priorityQueue;
    
    // Moves the current head of the priority
    // to the back of the priority queue
    void updatePriorityQueue(void);
    
    // Checks each bus node and grants access
    // to the first one a bus request is pending.
    void serviceBusNodes(void);
    
    // Broadcasts a message to all nodes except
    // the source of the transmission
    void broadcastBusRequest(BusRequest* r);  

};

#endif //INTERCONNECT_H