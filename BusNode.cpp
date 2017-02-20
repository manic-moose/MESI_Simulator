#include "BusNode.h"

unsigned int BusNode::getAddress(void) {
    return nodeAddress;
}

void BusNode::setAddress(unsigned int adx) {
    nodeAddress = adx;
}

bool BusNode::hasLock(void) {
    return nodeIsLocked;
}

void BusNode::grantLock(void) {
    nodeIsLocked = true;
}

void BusNode::releaseLock(void) {
    nodeIsLocked = false;
}
