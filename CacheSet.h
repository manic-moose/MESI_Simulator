#ifndef CACHESET_H
#define CACHESET_H


#include <iterator>
#include <math.h>
#include <vector>
#include "CacheLine.h"

/* @c CacheSet
 *    Class encapsulates a set in a cache, and contains a container
 *    of CacheLine objects. Provides algorithms for reading and writing
 *    to lines in the set, and required algorithms for selecting
 *    cache lines for eviction. Maintains history and statistics
 *    for the cache.
 */
class CacheSet {
 
    vector<CacheLine*> lineArry;
    unsigned int setNumber;
    unsigned long int readMisses;
    unsigned long int readHits;
    unsigned long int writeMisses;
    unsigned long int writeHits;
    unsigned long int streamOuts;
    unsigned long int streamIns;
    bool dumpMode;
    
    public:
    CacheSet (unsigned int setNum, unsigned int linesPerSet, unsigned int tagSize) {
        setNumber   = setNum;
        readMisses  = 0;
        readHits    = 0;
        writeMisses = 0;
        writeHits   = 0;
        streamOuts  = 0;
        streamIns   = 0;
        dumpMode    = 0;
        lineArry.reserve(linesPerSet);
        CacheLine* newLine = NULL;
        unsigned int lruSize = static_cast<int> (floor(ceil(log2(linesPerSet))));
        for (int i = 0; i < linesPerSet; i++) {
            newLine = new CacheLine(tagSize,lruSize,i);
            lineArry.push_back(newLine);
        }
    }
    
    /* @c getSetNumber
     * @returns Returns this set's number
     */
    unsigned int getSetNumber(void);
    
    /* @c getStreamOuts
     * @returns Returns total number of stream outs
     *          caused by dirty line eviction
     */
    unsigned long int const getStreamOuts(void);
    
    /* @c getStreamIns
     * @returns Returns total number of stream ins
     *          on the set.
     */
    unsigned long int const getStreamIns(void);
    
    /* @c getReadMisses
     * @returns Returns total read misses for set
     */
    unsigned long int const getReadMisses(void);
    
    /* @c getReadHits
     * @returns Returns total read hits for set
     */
    unsigned long int const getReadHits(void);
    
    /* @c getWriteMisses
     * @returns Returns total write misses for set
     */
    unsigned long int const getWriteMisses(void);
    
    /* @c getWriteHits
     * @return Returns total write hits for set
     */
    unsigned long int const getWriteHits(void);

    /* @c read - Performs a read operation on the set, 
     * including any required stream outs's and in's
     * @param unsigned int tag
     *          The tag being read from
     * @return Returns the number of clock cycles
     *         required for the operation.
     *         Results in the cache line tag and status
     *         bits being updated as required.
     */
    unsigned int read(unsigned int tag);

    /* @c write - Performs a write operation on the set, 
     * including any required stream outs's and in's
     * @param unsigned int tag
     *          The tag being write from
     * @return Returns the number of clock cycles
     *         required for the operation.
     *         Results in the cache line tag and status
     *         bits being updated as required.
     */
    unsigned int write(unsigned int tag);
    
    /* @c streamIn - Updates tag and status bits
    * for the line and LRU bits for the set
    * @param CacheLine* line
    *           The line to stream into.
    * @param unsigned int tag
    *           Tag being streamed in
    * @result All status and control bits for the
    *         set are updated.
    */
   void streamIn (CacheLine* line, unsigned int tag);
    
    /* @c updateLRU - Updates the LRU bits for each line
     * in the set.
     * @param CacheLine* line
     *          The line being accessed most recently.
     * @result Will update the LRU bits, with the parameter line
     * being the newest use in the set.
     */
    void updateLRU(CacheLine* line);
    
    /* @c findLRU - Static method to determine which line
     * is the least recently used from a list of candidate
     * lines. Assumes at least one line is on the candidate list.
     * @param vector<CacheLine*>&
     *          Reference to the vector containing
     *          candidate lines.
     * @return Returns the least recently used line in the list
     */
    CacheLine* findLRU (vector<CacheLine*>* candidates);

};
#endif //CACHESET_H