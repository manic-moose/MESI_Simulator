#ifndef BUSREQUEST_H
#define BUSREQUEST_H

#include "EventData.h"

struct BusRequest {
    unsigned int address;
    unsigned int requestCode;
    unsigned int sourceAddress;
};

#endif //BUSREQUEST_H