#ifndef CACHE_H
#define CACHE_H

#include <iterator>
#include <math.h>
#include <vector>
#include "CacheSet.h"

/* @c Cache
 *    Encapsulates the logic for the cache. Includes a container
 *    of CacheSet objects. Responds to commands for read and write
 *    and passes them to proper CacheSet. Maintains statistics about
 *    the cache and provides methods for reporting those statistics.
 */
class Cache {
    
  
private:
    // Container for CacheSet objects
    vector<CacheSet*> setArry;

    unsigned int addressLen;
    unsigned int tagSize;
    unsigned int paragraphBitSize;
    unsigned int setBitSize;
    unsigned int setBitMask;
    
    unsigned int linesPerSet;
    unsigned int bytesPerLine;
    
    unsigned int lruSize;
    
public:
    
    // Constructor
    Cache(unsigned int adxLength, unsigned int numSets, unsigned int lps, unsigned int bpl);
    
    /* getTagSize - Get the width of the tag
     * @return Returns the number of bits
     * that the tag will consume for each line.
     */
    unsigned int getTagSize (void);

    /* @c getLRUSize - Get the width of LRU bits
     * @return Returns the number of bits required
     *         for LRU.
     */
    unsigned int getLRUSize (void);
    
    /* @c getAdxSetNum - Get the set number associated
     * with a given address.
     * @param unsigned int adx
     *          The address being checked
     * @return Returns which set the address falls into.
     */
    unsigned int getAdxSetNum (unsigned int adx);
    
    /* @c getTag - Get the tag associated with an address
     * @param unsigned int adx
     *          The address being checked
     * @return Returns the value of the tag
     *         extracted from adx
     */
    unsigned int getTag (unsigned int adx);
    
    /* @c contains - Returns true of the cache contains
     * the data at address
     * @param unsigned int adx
     *          The address being queried
     * @return Returns true if the cache holds
     *         this addresses data, false otherwise.
     */
    bool contains(unsigned int adx);
    
    /* @c invalidate - Invalidates the cache line
     * at address adx, if present
     * @param adx
                The address of the item to invalidate
     * @result If the item is in the cache, it will
               be invalidated, otherwise nothing occurs.
     */
    void invalidate(unsigned int adx);
    
    /* @c isFull - Checks if the set associated with the
     *             given address is full or not
     * @param adx
     *          Address that maps to the set to check
     * @return Returns true if the set has no invalid lines
     *         available, false otherwise.
     */
    bool isFull(unsigned int adx);
    
    /* @c evictLineInSet - Invalidates a line
     * in the cache in the set mapped to by adx.
     * If there are any invalid lines in the set
     * already, nothing happens and NULL is returned.
     * @param adx
     *          The address that maps to the set where
     *          a line should be evicted.
     * @return Returns a pointer to the line that has
     *         been evicted. The pointer can be used
     *         to stream the data back to memory if needed.
     *         NULL is returned if no line was evicted.
     */
    CacheLine* evictLineInSet(unsigned int adx);
        
        
    
    /* hasCleanLines - Returns true if the set mapped by
     * the given address has at least 1 valid, non-dirty line
     * @param adx
     *          Address that maps to the desired set
     * @return Returns true if 1 valid, clean line exists,
               false otherwise
     */
    bool hasCleanLines(unsigned int adx);
    
    // MESI Accessor and Modifiers
    bool isExclusive(unsigned int adx);
    bool isModified(unsigned int adx);
    bool isShared(unsigned int adx);
    bool isInvalid(unsigned int adx);
    void setExclusive(unsigned int adx);
    void setModified(unsigned int adx);
    void setShared(unsigned int adx);
    void setInvalid(unsigned int adx);

};

#endif //CACHE_H