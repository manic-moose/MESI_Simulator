#include <iostream>
#include "Interconnect.h"
#include "DummyNode.h"

using namespace std;

int main (void) {
 
    Interconnect* i = new Interconnect;
    
    
    DummyNode* node1 = new DummyNode;
    DummyNode* node2 = new DummyNode;
    DummyNode* node3 = new DummyNode;
    
    cout << "Dummy nodes made" << endl;
    
    i->addNode(node1, 1);
    i->addNode(node2, 2);
    i->addNode(node3, 3);
    
    cout << "Dummy nodes hooked up to interconnect" << endl;
    
    i->Tick();
    
    node1->sendNewTransaction(2,10,6);
    
    i->Tick();
    i->Tick();
    node1->sendNewTransaction(2,4,22);
    i->Tick();
    node2->sendNewTransaction(1,5,14);
    i->Tick();
    i->Tick();
    i->Tick();
    cout << "Broadcast" << endl;
    node1->sendNewTransaction(0,3,34);
    i->Tick();
    i->Tick();
    
    return 0;
}