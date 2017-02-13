#include <iostream>
#include "Interconnect.h"
#include "DummyNode.h"


int main (void) {
 
    Interconnect* i = new Interconnect;
    
    DummyNode* node1 = new DummyNode;
    DummyNode* node2 = new DummyNode;
    DummyNode* node3 = new DummyNode;
    
    
    i->addNode(node1, 1);
    i->addNode(node2, 2);
    i->addNode(node3, 3);
    
    i->Tick();
    
    node1->sendNewTransaction(2,10);
    
    i->Tick();
    i->Tick();
    node1->sendNewTransaction(2,4);
    node2->sendNewTransaction(1,5);
    i->Tick();
    i->Tick();
    i->Tick();
    
    node1->sendNewTransaction(0,3);
    i->Tick();
    i->Tick();
    
    return 0;
}