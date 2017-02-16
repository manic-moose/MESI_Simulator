#ifndef BUSREQUEST_H
#define BUSREQUEST_H

#include "EventData.h"

struct BusRequest {
    unsigned int commandCode;
    unsigned int targetAddress;
    unsigned int sourceAddress;
    unsigned int payload;
};

#endif //BUSREQUEST_H