#include <iostream>
#include "Cache.h"

#define ADX_LEN 32
#define NUM_SETS 1024
#define LINES_PER_SET 4
#define BYTES_PER_LINE 32

using namespace std;

int main (void) {

    Cache* c = new Cache(ADX_LEN,NUM_SETS,LINES_PER_SET,BYTES_PER_LINE);

    c->contains(0);
    
    unsigned int adx = 1234506720;
    
    cout << "Adx: " << adx << " Set Number: " << c->getAdxSetNum(adx) << " Tag: " << c->getTag(adx) << endl;
    
    cout << "Address Returned: " << c->getAddress(c->getAdxSetNum(adx),  c->getTag(adx)) << endl;
    cout << "Address Returned: " << c->getLineAlignedAddress(adx) << endl;

    return 0;
}
