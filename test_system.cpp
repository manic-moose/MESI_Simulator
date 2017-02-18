#include "System.h"

int main(void) {
    
    System* s = new System(2, MI_CONTROLLER_TYPE, 4);
    
    s->Tick();
    
    return 0;   
}