#include "CacheSet.h"
   
/* @c getSetNumber
 * @returns Returns this set's number
 */
unsigned int CacheSet::getSetNumber(void) {
    return setNumber;
}

/* @c getStreamOuts
 * @returns Returns total number of stream outs
 *          caused by dirty line eviction
 */
unsigned long int const CacheSet::getStreamOuts(void) {
    return streamOuts;
}

/* @c getStreamIns
 * @returns Returns total number of stream ins
 *          on the set.
 */
unsigned long int const CacheSet::getStreamIns(void) {
    return streamIns;
}

/* @c getReadMisses
 * @returns Returns total read misses for set
 */
unsigned long int const CacheSet::getReadMisses(void) {
    return readMisses;
}

/* @c getReadHits
 * @returns Returns total read hits for set
 */
unsigned long int const CacheSet::getReadHits(void) {
    return readHits;
}

/* @c getWriteMisses
 * @returns Returns total write misses for set
 */
unsigned long int const CacheSet::getWriteMisses(void) {
    return writeMisses;
}

/* @c getWriteHits
 * @return Returns total write hits for set
 */
unsigned long int const CacheSet::getWriteHits(void) {
    return writeHits;   
}

/* @c read - Performs a read operation on the set, 
 * including any required stream outs's and in's
 * @param unsigned int tag
 *          The tag being read from
 * @return Returns the number of clock cycles
 *         required for the operation.
 *         Results in the cache line tag and status
 *         bits being updated as required.
 */
unsigned int CacheSet::read(unsigned int tag) {
    int clkCycles = 1;

    // First loop over each line in the set to see if
    // a valid line exists with the given tag. If so, this is a hit
    // and the operation takes 1 clock cycle
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isValid() && line->tagMatches(tag)) {
            readHits++;
            updateLRU(line);
            if (dumpMode) {
                dumper->record(buildDumperString("r", "hit", line));
            }
            return clkCycles;
        }
    }
    // We will need to stream in, so add STREAM_PENALTY to clock count
    streamIns++;
    clkCycles += STREAM_PENALTY;
    // If we are here, a miss has occurred because there are
    // no valid lines with a matching tag.
    // Now we must stream in the selected paragraph to one of the
    // lines. We loop through each line. If a line is not valid, we use it.
    readMisses++;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (!(line->isValid())) {
            streamIn(line,tag);
            if (dumpMode) {
                dumper->record(buildDumperString("r", "miss", line));
            }
            return clkCycles;
        }
    }
    // All ines are valid, so we now try and find a line that does not
    // need a stream out (ie, the line is not dirty).
    vector<CacheLine*> dirtyLines;
    vector<CacheLine*> cleanLines;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isDirty()) {
            dirtyLines.push_back(line);
        } else {
            cleanLines.push_back(line);
        }
    }
    if (cleanLines.size() > 0) {
        CacheLine* selectedLine = findLRU(&cleanLines);
        streamIn(selectedLine, tag);
        if (dumpMode) {
            dumper->record(buildDumperString("r", "miss", selectedLine));
        }
        return clkCycles;
    }
    if (dirtyLines.size() > 0) {
        // All lines are valid and dirty, a line must be selected for stream out
        CacheLine* dirtyLine = findLRU(&dirtyLines);
        // Additional STREAM_PENALTY cycles for stream out of dirty line
        streamOuts += 1;
        clkCycles += STREAM_PENALTY;
        streamIn(dirtyLine, tag);
        if (dumpMode) {
            dumper->record(buildDumperString("r", "miss", dirtyLine));
        }
        return clkCycles;
    } else {
        cout << "-E- Error during read." << endl;
        return 0;
    }
}

/* @c write - Performs a write operation on the set, 
 * including any required stream outs's and in's
 * @param unsigned int tag
 *          The tag being write from
 * @return Returns the number of clock cycles
 *         required for the operation.
 *         Results in the cache line tag and status
 *         bits being updated as required.
 */
unsigned int CacheSet::write(unsigned int tag) {
    int clkCycles = 1;
    // First loop over each line in the set to see if
    // a valid line exists with the given tag. If so, this is a hit
    // and the operation takes 1 clock cycle
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isValid() && line->tagMatches(tag)) {
            writeHits++;
            updateLRU(line);
            // Need to set dirty bit since this line is now
            // modified locally in cache.
            line->setDirty(true);
            if (dumpMode) {
                dumper->record(buildDumperString("w", "hit", line));
            }
            return clkCycles;
        }
    }
    // We will need to stream in, so add STREAM_PENALTY to clock count
    streamIns++;
    clkCycles += STREAM_PENALTY;
    // If we are here, a miss has occurred because there are
    // no valid lines with a matching tag.
    // Now we must stream in the selected paragraph to one of the
    // lines. We loop through each line. If a line is not valid, we use it.
    writeMisses++;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (!(line->isValid())) {
            streamIn(line,tag);
            if (dumpMode) {
                dumper->record(buildDumperString("w", "miss", line));
            }
            return clkCycles;
        }
    }
    // All ines are valid, so we now try and find a line that does not
    // need a stream out (ie, the line is not dirty).
    vector<CacheLine*> dirtyLines;
    vector<CacheLine*> cleanLines;
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line = (*it);
        if (line->isDirty()) {
            dirtyLines.push_back(line);
        } else {
            cleanLines.push_back(line);
        }
    }
    if (cleanLines.size() > 0) {
        CacheLine* selectedLine = findLRU(&cleanLines);
        streamIn(selectedLine, tag);
        selectedLine->setDirty(true);
        if (dumpMode) {
            dumper->record(buildDumperString("w", "miss", selectedLine));
        }
        return clkCycles;
    }
    // All lines are valid and dirty, a line must be selected for stream out
    if (dirtyLines.size() > 0) {
        CacheLine* dirtyLine = findLRU(&dirtyLines);
        // Additional STREAM_PENALTY cycles for stream out of dirty line
        streamOuts += 1;
        clkCycles += STREAM_PENALTY;
        streamIn(dirtyLine, tag);
        dirtyLine->setDirty(true);
        if (dumpMode) {
            dumper->record(buildDumperString("w", "miss", dirtyLine));
        }
        return clkCycles;
    } else {
        cout << "-E- Error during write" << endl;
        return 0;
    }
}

/* @c streamIn - Updates tag and status bits
* for the line and LRU bits for the set
* @param CacheLine* line
*           The line to stream into.
* @param unsigned int tag
*           Tag being streamed in
* @result All status and control bits for the
*         set are updated.
*/
void CacheSet::streamIn (CacheLine* line, unsigned int tag) {
    line->setValid(true);
    line->setDirty(false);
    line->setTag(tag);
    updateLRU(line);
}

/* @c updateLRU - Updates the LRU bits for each line
 * in the set.
 * @param CacheLine* line
 *          The line being accessed most recently.
 * @result Will update the LRU bits, with the parameter line
 * being the newest use in the set.
 */
void CacheSet::updateLRU(CacheLine* line) {
    unsigned int oldLRU = line->getLRU();
    line->setLRU(0);
    for (vector<CacheLine*>::iterator it = lineArry.begin(); it < lineArry.end(); it++) {
        CacheLine* line0 = (*it);
        if (line0 != line && line0->isValid()) {
            unsigned int line0LRU = line0->getLRU();
            if (line0LRU < oldLRU) {
                line0->setLRU(line0LRU + 1);   
            }
        }
    }
}

/* @c findLRU - Static method to determine which line
 * is the least recently used from a list of candidate
 * lines. Assumes at least one line is on the candidate list.
 * @param vector<CacheLine*>&
 *          Reference to the vector containing
 *          candidate lines.
 * @return Returns the least recently used line in the list
 */
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