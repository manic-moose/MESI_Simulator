#include "Interconnect.h"

#include <iostream>

void Interconnect::Tick(void) {
    BEGIN_TRANSITION_MAP                    // - Current State -
        TRANSITION_MAP_ENTRY(ST_CHECK)      // Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // Checking For New Request
        TRANSITION_MAP_ENTRY(ST_CHECK)      // Busy Processing Transaction
    END_TRANSITION_MAP(NULL)
}

void Interconnect::ST_Idle(void) {
    std::cout << "Interconnect: IDLE" << std::endl;
    // Nothing to do in IDLE state
}

void Interconnect::ST_Check(void) {
    std::cout << "Interconnect: CHECK" << std::endl;
    map<unsigned int, BusNode*>::iterator it;
    for (it=nodes.begin(); it != nodes.end(); it++) {
        unsigned int address = it->first;
        BusNode* node = it->second;
        std::cout << "Here for node adx: " << address << std::endl;
        if (node->requestsTransaction()) {
            std::cout << "Here2 for node adx: " << address << std::endl;
            // Pending request found - go to BUSY state
            adx_to_process = address;
            InternalEvent(ST_BUSY);
            return;
        }
    }
    std::cout << "No pending requests found" << std::endl;
    // No requests found, go back to IDLE state
    InternalEvent(ST_IDLE);
}

void Interconnect::ST_Busy(void) {
    std::cout << "Interconnect: BUSY" << std::endl;
    // Handle a single pending request
    unsigned int txAddress = adx_to_process;
    BusNode* transmitterNode = getNode(txAddress);
    BusRequest* request = transmitterNode->initiateBusTransaction();
    unsigned int requestAdx  = request->address;
    if (requestAdx == BROADCAST_ADX) {
        map<unsigned int, BusNode*>::iterator it;
        for (it=nodes.begin(); it != nodes.end(); it++) {
            unsigned int nodeAddress = it->first;
            if (!(nodeAddress == txAddress)) {
                BusNode* receiverNode = it->second;
                receiverNode->acceptBusTransaction(request);
            }
        }
    } else {
        BusNode* receiverNode = getNode(requestAdx);
        receiverNode->acceptBusTransaction(request);
    }
}

void Interconnect::addNode(BusNode* n, unsigned int address) {
    assert(!hasNode(address));
    assert(!(address==BROADCAST_ADX));
    nodes.insert(pair<unsigned int, BusNode*>(address,n));
    n->setAddress(address);
}

void Interconnect::deleteNode(unsigned int address) {
    if (hasNode(address)) {
        nodes.erase(address);
    }
}

bool Interconnect::hasNode(unsigned int address) {
    return (nodes.count(address) > 0);
}

BusNode* Interconnect::getNode(unsigned int address) {
    assert(hasNode(address));
    return nodes[address];
}