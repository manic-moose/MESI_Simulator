#include "Cache.h"
#include <assert.h>
#include <iostream>
using namespace std;

Cache::Cache (unsigned int adxLength, unsigned int numSets, unsigned int lps, unsigned int bpl) {
    int i;
    linesPerSet = lps;
    bytesPerLine = bpl;
    addressLen = adxLength;
    setArry.reserve(numSets);
    lruSize          = static_cast<int> (floor(ceil(log2(lps))));
    paragraphBitSize = static_cast<int> (floor(ceil(log2(bpl))));
    setBitSize       = static_cast<int> (floor(ceil(log2(numSets))));
    setBitMask = 0;
    for (i = 0; i < setBitSize; i++) {
        setBitMask |= (1 << i); 
    }
    tagSize = adxLength - paragraphBitSize - setBitSize;
    CacheSet* newSet = NULL;
    for (i = 0; i < numSets; i++) {
        newSet = new CacheSet(i,lps,tagSize);
        setArry.push_back(newSet);
    }
}

unsigned int Cache::getTagSize (void) {
    return tagSize;
}

unsigned int Cache::getLRUSize (void) {
    return lruSize;
}

unsigned int Cache::getAdxSetNum (unsigned int adx) {
    adx = adx >> paragraphBitSize;
    return adx & setBitMask;
}

unsigned int Cache::getLineAlignedAddress(unsigned int adx) {
    return getAddress(getAdxSetNum(adx),getTag(adx));
}

unsigned int Cache::getTag (unsigned int adx) {
    return (adx >> (addressLen - tagSize));
}

unsigned int Cache::getAddress(unsigned int setNum, unsigned int tag) {
    unsigned int adx;
    adx = (setNum << paragraphBitSize) | (tag << (addressLen - tagSize));
    return adx;
}

CacheLine* Cache::evictLineInSet(unsigned int adx) {
    unsigned int setNumber = getAdxSetNum(adx);
    CacheSet* set = setArry.at(setNumber);
    CacheLine* line = set->evict();
    return line;
}

bool Cache::isFull(unsigned int adx) {
    unsigned int setNumber = getAdxSetNum(adx);
    CacheSet* set = setArry.at(setNumber);
    return set->isFull();
}

bool Cache::hasCleanLines(unsigned int adx) {
    unsigned int setNumber = getAdxSetNum(adx);
    CacheSet* set = setArry.at(setNumber);
    return set->hasCleanLines();
}

bool Cache::contains(unsigned int adx) {
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    return set->CacheSet::contains(tag);
}

CacheLine* Cache::invalidate(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    return set->invalidate(tag);
}

// MESI Accessor and Modifiers
bool Cache::isExclusive(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->isExclusive(tag);
}

bool Cache::isModified(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->isModified(tag);
}

bool Cache::isShared(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->isShared(tag);
}

bool Cache::isInvalid(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->isInvalid(tag);
}

void Cache::setExclusive(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->setExclusive(tag);
}

void Cache::setModified(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->setModified(tag);
}

void Cache::setShared(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->setShared(tag);
}

void Cache::setInvalid(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->setInvalid(tag);
}

void Cache::insertLine(unsigned int adx) {
    assert(!isFull(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->insertLine(tag);
}

void Cache::updateLRU(unsigned int adx) {
    assert(contains(adx));
    unsigned int setNumber = getAdxSetNum(adx);
    unsigned int tag = getTag(adx);
    CacheSet* set = setArry.at(setNumber);
    set->updateLRU(tag);
}

