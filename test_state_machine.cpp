#include "StateMachine.h"
#include <iostream>
using namespace std;

string state0_t(void);
string state1_t(void);
void state0_a(void);

int main (void) {
 
    cout << "HERE 0" << endl;
    StateMachine* k = new StateMachine;
    cout << "HERE 1" << endl;
    k->addNewState("state0", state0_t, state0_a);
    cout << "HERE 2" << endl;
    k->addNewState("state1", state1_t, NULL);
    cout << "HERE 3" << endl;
    k->initialize("state0");
    cout << "HERE 4" << endl;
    k->Tick();
    cout << "HERE 5" << endl;
    
    return 0;
}


string state0_t(void) {
    return "state1";
}

string state1_t(void) {
    return "state0";
}

void state0_a(void) {
    cout << "State 0 action function" << endl;
}