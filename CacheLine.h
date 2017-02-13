#ifndef CACHELINE_H
#define CACHELINE_H

/* @c CacheLine
 *    Class encapsulates a line in the cache,
 *    and stores the tag, LRU, dirty, MESI, and valid bits for the line.
 *    Provides methods for reading and setting these bits.
 */
class CacheLine {
    
    unsigned int tag;
    unsigned int tagMask;
    unsigned int lru_bits;
    unsigned int lruMask;
    unsigned int lineNum;
    unsigned int mesi_bits;
    bool dirty_bit;
    bool valid_bit;
    
    enum MESI_States {
        MODIFIED = 0,
        EXCLUSIVE,
        SHARED,
        INVALID
    };
    
    public:
    CacheLine (unsigned int tagLen, unsigned int lruLen, unsigned int lineNumber) {
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
        dirty_bit = 0;
        valid_bit = 0;
        mesi_bits = INVALID;
    }
    
    bool isValidMESI(unsigned int value) const;
    
    void setMESI(unsigned int value);
    
    unsigned int getMESI() const;
    
    bool isExclusive() const;
    
    bool isModified() const;
    
    bool isShared() const;
    
    bool isInvalid() const;
    
    /* @c getLineNumber
     * @returns Returns the line number
     */
    unsigned int getLineNumber() const;
    
    /* @c getTag
     * @return Returns tag bits for line
     */
    unsigned int getTag() const;

    /* @c setTag
     * @result Sets the tag bits for the line
     */
    void setTag (unsigned int t);
    
    /* @c getLRU
     * @return Returns LRU bits for line
     */
    unsigned int getLRU () const;
    
    /* @c setLRU
     * @result Sets LRU bits for line
     */
    void setLRU (unsigned int l);
    
    /* @c isDirty
     * @return Returns state of dirty bit
     */
    bool isDirty () const;
    
    /* @c isValid
     * @return Returns state of valid bit
     */
    bool isValid () const;
    
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