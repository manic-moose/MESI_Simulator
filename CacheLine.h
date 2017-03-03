#ifndef CACHELINE_H
#define CACHELINE_H

#define MODIFIED  0
#define EXCLUSIVE 1
#define SHARED    2
#define INVALID   3

/* @c CacheLine
 *    Class encapsulates a line in the cache,
 *    and stores the tag, LRU, dirty, MESI, and valid bits for the line.
 *    Provides methods for reading and setting these bits.
 */
class CacheLine {
    
private:
    unsigned long long tag;
    unsigned int tagMask;
    unsigned int lru_bits;
    unsigned int lruMask;
    unsigned int lineNum;
    unsigned int mesi_bits;
    unsigned int setNum;
    bool dirty_bit;
    bool valid_bit;

    public:
    CacheLine (unsigned int tagLen, unsigned int lruLen, unsigned int lineNumber, unsigned int setNumber) {
        int i;
        lineNum = lineNumber;
        tagMask = 0;
        lruMask = 0;
        for (i = 0; i < tagLen; i++) {
            tagMask |= (1 << i);   
        }
        for (i = 0; i < lruLen; i++) {
            lruMask |= (1 << i);
        }
        tag       = 0;
        lru_bits  = 0;
        dirty_bit = false;
        valid_bit = false;
        mesi_bits = INVALID;
    }
    
    
    // MESI Accessors and Modifiers
    bool isValidMESI(unsigned int value);
    void setMESI(unsigned int value);   
    unsigned int getMESI(void);
    bool isExclusive(void);
    bool isModified(void);   
    bool isShared(void);
    bool isInvalid(void);
    
    /* @c getSetNumber
     * @return Returns the set number this line belongs to
     */
    unsigned int getSetNumber(void);
    
    /* @c getLineNumber
     * @returns Returns the line number
     */
    unsigned int getLineNumber(void);
    
    /* @c getTag
     * @return Returns tag bits for line
     */
    unsigned long long getTag(void);

    /* @c setTag
     * @result Sets the tag bits for the line
     */
    void setTag (unsigned long long t);
    
    /* @c getLRU
     * @return Returns LRU bits for line
     */
    unsigned int getLRU (void);
    
    /* @c setLRU
     * @result Sets LRU bits for line
     */
    void setLRU (unsigned int l);
    
    /* @c isDirty
     * @return Returns state of dirty bit
     */
    bool isDirty (void);
    
    /* @c isValid
     * @return Returns state of valid bit
     */
    bool isValid (void);
    
    /* @c setDirty
     * @param bool d
     *          Value to set bit to
     * @result dirty bit is set to value of d
     */
    void setDirty (bool d);
    
    /* @c setValid
     * @param bool v
     *          Value to set bit to
     * @result valid bit is set to value fo v
     */
    void setValid (bool v);
    
    /* @c tagMatches
     * @param unsigned int tag
     *          Tag to compare against
     * @returns Returns true if tag matches
     *          the tag assigned to the line.
     */
    bool tagMatches (unsigned int tag);

};
#endif //CACHELINE_H

