#ifndef CACHESET_H
#define CACHESET_H


#include <iterator>
#include <math.h>
#include <vector>
#include "CacheLine.h"
#include <assert.h>
#include <map>

using namespace std;

/* @c CacheSet
 *    Class encapsulates a set in a cache, and contains a container
 *    of CacheLine objects. Provides algorithms for reading and writing
 *    to lines in the set, and required algorithms for selecting
 *    cache lines for eviction. Maintains history and statistics
 *    for the cache.
 */
class CacheSet {
 
private:
    vector<CacheLine*> lineArry;
    unsigned int setNumber;
    
    public:
    CacheSet (unsigned int setNum, unsigned int linesPerSet, unsigned int tagSize);
    
    /* @c getSetNumber
     * @returns Returns this set's number
     */
    unsigned int getSetNumber(void);
    
   
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
    
    /* @c contains - Checks this set for the associated address
     * @param tag
     *          The tag being queried
     * @result Will return true if this set contains the tag
     *         and false otherwise
     */
    bool contains(unsigned int tag);
    
    /* @c evict - Selects a line for eviction and invalidates it
     * @result Line will be selected using LRU policy and evicted.
     *         Non-dirty lines will be considered before dirty lines.
     *         If any invalid lines are in the set already, no action
     *         is taken. If a dirty line must be evicted. Returns
     *         the evicted line, or NULL if none evicted.
     */
     CacheLine* evict(void);
    
    /* @c selectLineForEviction - Picks a line to evict
     * @return Returns the selected line. If any valid lines
     * are in the set, NULL is returned.
     */
     CacheLine* selectLineForEviction(void);
    
    /* @c invalidate - Invalidates the line with the given tag
     * @param tag
     *          The tag associated with line to invalidate
     * @result The selected line will be invalidated.
     */
    void invalidate(unsigned int tag);
    
    /* @c invalidate - Invalidates the line with the given tag
     * @param line
     *          The line to invalidate
     * @result The selected line will be invalidated.
     */ 
    void invalidate(CacheLine* line);
    
    /* @c getLine - Gets the line associated with the tag
     * @param tag
     *          The tag to get
     * @returns Returns associated line or fatal error
     *          if line is not present
     */
    CacheLine* getLine(unsigned int tag);
    
    /* checkAndFixLRU - Verifies that the LRU bits
     * for valid lines are monotonically increasing
     * from 0 to n, where n is the number of valid lines.
     * This is useful if an external event requires
     * a line is invalidated that is not the least
     * recently used.
     */
    void checkAndFixLRU(void);
    
    /* hasCleanLines - Returns true if this
     * set has at least 1 valid, non-dirty line
     * @return Returns true if 1 valid, clean line exists,
               false otherwise
     */
    bool hasCleanLines(void);
    
    /* isFull - Returns true if there are no invalid
     *          lines in the set.
     * @return Returns false if all lines are valid, true
     *         otherwise
     */
    bool isFull(void);
     
    /* @c findLRU - Method to determine which line
     * is the least recently used from a list of candidate
     * lines. Assumes at least one line is on the candidate list.
     * @param vector<CacheLine*>&
     *          Reference to the vector containing
     *          candidate lines.
     * @return Returns the least recently used line in the list
     */
    CacheLine* findLRU (vector<CacheLine*>* candidates);
    
    // MESI Accessor and Modifiers
    bool isExclusive(unsigned int tag);
    bool isModified(unsigned int tag);
    bool isShared(unsigned int tag);
    bool isInvalid(unsigned int tag);
    void setExclusive(unsigned int tag);
    void setModified(unsigned int tag);
    void setShared(unsigned int tag);
    void setInvalid(unsigned int tag);

};
#endif //CACHESET_H