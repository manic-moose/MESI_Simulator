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
    
    /* getAddress - Returns the address as derived using set num and tag.
     * @param setNum - The set number of the address to calculate
     * @param tag - The tag of the address to calculate
     * @return Returns an address that is derived from the tag and set
     *         number.
     */
    unsigned long long getAddress(unsigned int setNum, unsigned long long tag);
    
    /* getLineAlignedAddress - Given an address, returns the
     * memory address that corresponds to the beginning of
     * the cache line that the given address belongs to.
     * @param adx
     *          The non-line aligned address
     * @return Returns the line aligned memory address
     */
    unsigned long long getLineAlignedAddress(unsigned long long adx);
    
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
    unsigned int getAdxSetNum (unsigned long long adx);
    
    /* @c getTag - Get the tag associated with an address
     * @param unsigned int adx
     *          The address being checked
     * @return Returns the value of the tag
     *         extracted from adx
     */
    unsigned long long getTag (unsigned long long adx);
    
    /* @c contains - Returns true of the cache contains
     * the data at address
     * @param unsigned int adx
     *          The address being queried
     * @return Returns true if the cache holds
     *         this addresses data, false otherwise.
     */
    bool contains(unsigned long long adx);
    
    /* @c invalidate - Invalidates the cache line
     * at address adx, if present
     * @param adx
     *          The address of the item to invalidate
     * @Return If the item is in the cache, it will
     *         be invalidated, otherwise nothing occurs.
     *         A pointer to the CacheLine is returned.
     */
    CacheLine* invalidate(unsigned long long adx);
    
    /* @c isFull - Checks if the set associated with the
     *             given address is full or not
     * @param adx
     *          Address that maps to the set to check
     * @return Returns true if the set has no invalid lines
     *         available, false otherwise.
     */
    bool isFull(unsigned long long adx);
    
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
    CacheLine* evictLineInSet(unsigned long long adx);

    /* @c insertLine - Adds a line to the set
     * that adx belongs to.
     * @param adx
     *          Address that is for the set to fill
     *          the line with.
     * @result Set will be filled with line. Assumes
     *         there is space in the set.
     */
    void insertLine(unsigned long long adx);
    
    /* @c updateLRU - Updates the lru bits for the cache line
     *                at the given address.
     * @param adx
     *          Address that maps to cache line to update
     * @result LRU bits will be updated, making the line
     *         mapped to by adx the most recent to be used
     */
    void updateLRU(unsigned long long adx);
        
        
    
    /* hasCleanLines - Returns true if the set mapped by
     * the given address has at least 1 valid, non-dirty line
     * @param adx
     *          Address that maps to the desired set
     * @return Returns true if 1 valid, clean line exists,
               false otherwise
     */
    bool hasCleanLines(unsigned long long adx);
    
    // MESI Accessor and Modifiers
    bool isExclusive(unsigned long long adx);
    bool isModified(unsigned long long adx);
    bool isShared(unsigned long long adx);
    bool isInvalid(unsigned long long adx);
    void setExclusive(unsigned long long adx);
    void setModified(unsigned long long adx);
    void setShared(unsigned long long adx);
    void setInvalid(unsigned long long adx);

};

#endif //CACHE_H

