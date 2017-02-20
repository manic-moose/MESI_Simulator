#include "Interconnect.h"

#include <iostream>

void Interconnect::Tick(void) {
    if (nodeLocked) {
        serviceBusNode(lockedAddress);
    } else {
        serviceBusNodes();
        updatePriorityQueue();
    }
}

void Interconnect::serviceBusNodes(void) {
    for(vector<unsigned int>::iterator it = priorityQueue->begin(); it < priorityQueue->end(); it++) {
        unsigned int address = (*it);
        // Only handle a single bus communication in each service
        // cycle (one per clock tick)
        if (serviceBusNode(address)) {
            return;
        }
    }
}

bool Interconnect::serviceBusNode(unsigned int address) {
    BusNode* txNode = getNode(address);
    if (txNode->requestsLock()) {
        lockedAddress = address;
        nodeLocked = true;
    } else {
        nodeLocked = false;
    }
    if (txNode->requestsTransaction()) {
        BusRequest* request = txNode->initiateBusTransaction();
        unsigned int requestAdx  = request->targetAddress;
        if (requestAdx == BROADCAST_ADX) {
            broadcastBusRequest(request);
        } else {
            BusNode* receiverNode = getNode(requestAdx);
            receiverNode->acceptBusTransaction(request);
        }
        // We're now done with the BusRequest, so lets delete it.
        delete request;
        return true;
    }
    return false;
}

void Interconnect::broadcastBusRequest(BusRequest* r) {
    unsigned int txAddress = r->sourceAddress;
    map<unsigned int, BusNode*>::iterator it;
    for (it=nodes.begin(); it != nodes.end(); it++) {
        unsigned int nodeAddress = it->first;
        if (!(nodeAddress == txAddress)) {
            // Only send if this node is not the
            // transmitter node
            BusNode* receiverNode = it->second;
            receiverNode->acceptBusTransaction(r);
        }
    }
}

void Interconnect::addNode(BusNode* n, unsigned int address) {
    assert(!hasNode(address));
    assert(!(address==BROADCAST_ADX));
    nodes.insert(pair<unsigned int, BusNode*>(address,n));
    n->setAddress(address);
    priorityQueue->push_back(address);
}

void Interconnect::deleteNode(unsigned int address) {
    if (hasNode(address)) {
        nodes.erase(address);
    }
    for(vector<unsigned int>::iterator it = priorityQueue->begin(); it < priorityQueue->end(); it++) {
        unsigned int pAdx = (*it);   
        if (pAdx == address) {
            priorityQueue->erase(it);
        }
    }
}

void Interconnect::updatePriorityQueue(void) {
    if (getNodeCount() > 0) {
        unsigned int priorityOne = priorityQueue->at(0);
        priorityQueue->erase(priorityQueue->begin());
        priorityQueue->push_back(priorityOne);
    }
}

unsigned int Interconnect::getNodeCount(void) {
    return nodes.size();   
}

bool Interconnect::hasNode(unsigned int address) {
    return (nodes.count(address) > 0);
}

BusNode* Interconnect::getNode(unsigned int address) {
    assert(hasNode(address));
    return nodes[address];
}