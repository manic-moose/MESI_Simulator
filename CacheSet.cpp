#include "CacheSet.h"
#include <iostream>

using namespace std;
   
CacheSet::CacheSet (unsigned int setNum, unsigned int linesPerSet, unsigned int tagSize) {
    setNumber   = setNum;
    lineArry.reserve(linesPerSet);
    CacheLine* newLine = NULL;
    unsigned int lruSize = static_cast<int> (floor(ceil(log2(linesPerSet))));
    for (int i = 0; i < linesPerSet; i++) {
        newLine = new CacheLine(tagSize,lruSize,i, setNum);
        lineArry.push_back(newLine);
    }
}

unsigned int CacheSet::getSetNumber(void) {
    return setNumber;
}

bool CacheSet::contains(unsigned long long tag) {
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isValid() && line->tagMatches(tag)) {
            return true;
        }
    }
    return false;
}

void CacheSet::streamIn (CacheLine* line, unsigned long long tag) {
    line->setValid(true);
    line->setDirty(false);
    line->setTag(tag);
    updateLRU(line);
}

void CacheSet::insertLine(unsigned long long tag) {
    CacheLine* line = getEmptyLine();
    assert(line != NULL);
    streamIn(line,tag);
}

CacheLine* CacheSet::getEmptyLine(void) {
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (!line->isValid()) {
            return line;
        }
    }
    return NULL;
}

void CacheSet::updateLRU(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    updateLRU(line);
}

void CacheSet::updateLRU(CacheLine* line) {
    unsigned int oldLRU = line->getLRU();
    line->setLRU(0);
    unsigned int lastLRU = 1;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line0 = (*it);
        if (line0 != line && line0->isValid()) {
            unsigned int line0LRU = line0->getLRU();
            if (line0LRU <= oldLRU) {
                line0->setLRU(lastLRU++);   
            }
        }
    }
}

CacheLine* CacheSet::selectLineForEviction(void) {
    vector<CacheLine*>* dirtyLines = new vector<CacheLine*>;
    vector<CacheLine*>* cleanLines = new vector<CacheLine*>;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (!line->isValid()) {
            return NULL;   
        }
        if (line->isDirty()) {
            dirtyLines->push_back(line);
        } else {
            cleanLines->push_back(line);
        }
    }
    CacheLine* selectedLine;
    if (cleanLines->size() > 0) {
        selectedLine = findLRU(cleanLines);
    }
    else if (dirtyLines->size() > 0) {
        selectedLine = findLRU(dirtyLines);
    }
    assert(selectedLine != NULL);
    delete dirtyLines;
    delete cleanLines;
    return selectedLine;
}

CacheLine* CacheSet::evict(void) {
    CacheLine* line = selectLineForEviction();
    if (line != NULL) {
        invalidate(line);
    }
    return line;
}

void CacheSet::checkAndFixLRU(void) {
    map<unsigned int, CacheLine*> lruMap;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isValid()) {
            unsigned int lruBits = line->getLRU();
            lruMap[lruBits] = line;
        }
    }
    map<unsigned int, CacheLine*>::iterator it;
    unsigned int lruValue = 0;
    for (it=lruMap.begin(); it != lruMap.end(); it++) {
        unsigned int currentLRU = it->first;
        CacheLine* line = it->second;
        line->setLRU(lruValue++);
    }
}

CacheLine* CacheSet::invalidate(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    return invalidate(line);
}

CacheLine* CacheSet::invalidate(CacheLine* line) {
    line->setValid(false);
    line->setMESI(INVALID);
    checkAndFixLRU();
    return line;
}

CacheLine* CacheSet::getLine(unsigned long long tag) {
    assert(contains(tag));
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->tagMatches(tag)) {
            return line;
        }
    }
}

CacheLine* CacheSet::findLRU (vector<CacheLine*>* candidates) {
    unsigned int maxLRUValue = 0;
    CacheLine* lruLine;
    for (vector<CacheLine*>::iterator it = candidates->begin(); it < candidates->end(); it++) {
        CacheLine* line = (*it);
        unsigned int lruVal = line->getLRU();
        if (lruVal > maxLRUValue) {
            maxLRUValue = line->getLRU();
            lruLine = line;
        }
    }
    return lruLine;
}

bool CacheSet::hasCleanLines(void) {
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isValid() && !line->isDirty()) {
            return true;   
        }
    }
    return false;
}

bool CacheSet::isFull(void) {
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        bool isval = line->isValid();
        if (!line->isValid()) {
            return false;   
        }
    }
    return true;
}

bool CacheSet::isExclusive(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    return line->isExclusive();
}

bool CacheSet::isModified(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    return line->isModified();
}

bool CacheSet::isShared(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    return line->isShared();
}

bool CacheSet::isInvalid(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    line->isInvalid();
}

void CacheSet::setExclusive(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    line->setMESI(EXCLUSIVE);
}

void CacheSet::setModified(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    line->setMESI(MODIFIED);
}

void CacheSet::setShared(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    line->setMESI(SHARED);
}

void CacheSet::setInvalid(unsigned long long tag) {
    CacheLine* line = getLine(tag);
    line->setMESI(INVALID);
}
