#include "Cache.h"

/* @c Cache
 *    Encapsulates the logic for the cache. Includes a container
 *    of CacheSet objects. Responds to commands for read and write
 *    and passes them to proper CacheSet. Maintains statistics about
 *    the cache and provides methods for reporting those statistics.
 */
class Cache {
    
    // Container for CacheSet objects
    vector<CacheSet*> setArry;
    
    // Retains command history
    CmdDumper dumper;
    
    unsigned int addressLen;
    unsigned int tagSize;
    unsigned int paragraphBitSize;
    unsigned int setBitSize;
    unsigned int setBitMask;
    
    unsigned int linesPerSet;
    unsigned int bytesPerLine;
    
    unsigned long int totalClocks;
    unsigned long int totalClocksNoCache;
    
    unsigned int lruSize;
    
    public:
    Cache (unsigned int adxLength, unsigned int numSets, unsigned int lps, unsigned int bpl) {
        int i;
        linesPerSet = lps;
        bytesPerLine = bpl;
        totalClocks = 0;
        totalClocksNoCache = 0;
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

    /* getTagSize - Get the width of the tag
     * @return Returns the number of bits
     * that the tag will consume for each line.
     */
    unsigned int const getTagSize (void) {
        return tagSize;
    }

    /* @c getLRUSize - Get the width of LRU bits
     * @return Returns the number of bits required
     *         for LRU.
     */
    unsigned int const getLRUSize (void) {
        return lruSize;
    }
    
    /* @c getAdxSetNum - Get the set number associated
     * with a given address.
     * @param unsigned int adx
     *          The address being checked
     * @return Returns which set the address falls into.
     */
    unsigned int const getAdxSetNum (unsigned int adx) {
        adx = adx >> paragraphBitSize;
        return adx & setBitMask;
    }
    
    /* @c getTag - Get the tag associated with an address
     * @param unsigned int adx
     *          The address being checked
     * @return Returns the value of the tag
     *         extracted from adx
     */
    unsigned int const getTag (unsigned int adx) {
        return (adx >> (addressLen - tagSize));
    }
    
    /* @c read - Read from a particular address in memory
     *           subsystem.
     * @param unsigned int adx
     *          Memory address to read from
     * @returns
     *      Returns number of cycles required to 
     *      perform read.
     */
    unsigned int read(unsigned int adx) {
        unsigned int setNumber = getAdxSetNum(adx);
        unsigned int tag = getTag(adx);
        if (setNumber < setArry.size()) {
            CacheSet* selectedSet = setArry.at(setNumber);
            unsigned int clkCycles = selectedSet->read(tag);
            totalClocks += clkCycles;
            totalClocksNoCache += STREAM_PENALTY;
            return clkCycles;
        } else {
            cout << "-E- Read from address: " << adx << " with set number: "<< setNumber << " failed." << endl;
            cout << "-E- Set does not exist in cache." << endl;
            return 0;
        }
    }
    
    /* @c write - Write to a particular address in memory
     *           subsystem.
     * @param unsigned int adx
     *          Memory address to write to
     * @returns
     *      Returns number of cycles required to 
     *      perform write.
     */
    unsigned int write(unsigned int adx) {
        unsigned int setNumber = getAdxSetNum(adx);
        unsigned int tag = getTag(adx);
        if (setNumber < setArry.size()) {
            CacheSet* selectedSet = setArry.at(setNumber);
            unsigned int clkCycles = selectedSet->write(tag);
            totalClocks += clkCycles;
            totalClocksNoCache += STREAM_PENALTY;
            return clkCycles;
        } else {
            cout << "-E- Write from address: " << adx << " with set number: "<< setNumber << " failed." << endl;
            cout << "-E- Set does not exist in cache." << endl;
            return 0;
        }
    }