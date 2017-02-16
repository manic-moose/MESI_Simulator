#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <assert.h>
#include "StateMachine.h"

#include <string>
#include <iostream>
#include <map>

using namespace std;
 
typedef string (*TransitionFunction) (void);
typedef void   (*ActionFunction) (void);

class StateMachine {
    
public: 


    string currentState;
    map<string,TransitionFunction> tMap;
    map<string,ActionFunction> aMap;
    bool initialized;
    
    StateMachine() {
        initialized = false;
    }
    
    // Add a new state. Pass in pointers to the functions that define
    // transitions and any actions to occur each tick that the machine exists
    // in this state
    void addNewState(string stateName, TransitionFunction t, ActionFunction a);
    
    // Increments the state machine. Will first call the transition function
    // for the current state, move to whatever the selected state is,
    // and call the action function for the state, if it is not NULL
    void Trigger(void);
    
    // Initializes the state machine to an initial state.
    // State must have been defined using addNewState.
    void initialize(string state);
    
    
    
    
};
 
#endif //STATE_MACHINE_H

