#include "StateMachine.h"

void StateMachine::addNewState(string stateName, TransitionFunction t, ActionFunction a) {
    tMap.emplace(stateName,t);
    aMap.emplace(stateName,a);
}

void StateMachine::Trigger(void) {
    assert(initialized);
    assert(tMap.count(currentState) > 0);
    assert(aMap.count(currentState) > 0);
    cout << "Tick ";
    
    TransitionFunction t = tMap.at(currentState);
    string nextState  = this->t();
    cout << " Next State: " << nextState << endl;
    assert(tMap.count(nextState) > 0);
    assert(aMap.count(nextState) > 0);
    
    currentState = nextState;
    
    ActionFunction a = aMap.at(currentState);
    if (a != NULL) {
        a();   
    }
}

void StateMachine::initialize(string state) {
    assert(tMap.count(state) > 0);
    assert(aMap.count(state) > 0);
    currentState = state;
    initialized = true;
}