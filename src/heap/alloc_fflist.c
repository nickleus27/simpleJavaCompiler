/**
 * @file heap_fflist.c
 * @author Nick Anderson (nickleus27@gmail.com)
 * @brief This is a heap implementation that uses first fit strategy
 * when searching the free list. The heap maintains a singly linked 
 * list, and worst case search is linear.
 * @version 0.2
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * THIS IS STILL A WORK IN PROGRESS
 */


#define MAX_HEAP_SIZE 8176000 /* 8176000 == 8176 kb */
/**
 * @brief [0, 1,...]
 *        [(0) size of block, (1) points to next free block, ...(free space)...] 
 */
#define FIRST_FREE_BLOCK(size, first, prev, next, ret) { \
    prev = *first; \
    size = (*(int*)prev); \
    next = *first; \
    next++; \
    ret = next; \
}
#define NEXT_FREE_BLOCK(size, prev, curr, next, ret) { prev = next; \
    next = *next; \
    curr = next; \
    next++; \
    ret = next; \
    size = (*(int*)curr); \
}
#define HAS_NEXT_FREE_BLOCK *next_ptr
#define SET_BLOCK_SIZE(pointer, size) { (*(int*)pointer) = size; }
#define UPDATE_LINKS(prev, next, size) { *prev = (char*)(next) + size; next = *prev; }

/**
 * @brief https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for
 *  use this for compiling assembly without call frame information unwinding directives
 * 
 * TODO: Also get rid of library includes up top, and the exit call...hand write in the exit to assembly
 */

void* free_list[MAX_HEAP_SIZE/8];

void* allocate(int size) {
    //+8 to store size tag, and +8 for storing next tag will be applied by the size % 8
    // for example size == 1, 1+8 ==9, while(size % 8) size++ == 16
    size += 8; //+8 for store size tag
    while (size % 8) { //8 byte aligned
        size++;
    }
    /* initialize heap pointer */
    if (!*free_list) {
        *free_list = free_list + 1; //add 1 == 8 bytes ->first free space
        /* 8176000 == 8176 kb */
        /* dereference to assign value and cast to int* */
        (*(int*)(free_list+1)) = MAX_HEAP_SIZE - 32; //assign totals free space (-8 pointer -16 for last free block that holds size=0, next=null)
                
    }
    int free_block_size;
    void** prev_ptr;
    void** next_ptr;
    void** ret;
    FIRST_FREE_BLOCK(free_block_size, free_list, prev_ptr, next_ptr, ret)

    /**
     * @brief (free_list pointer) points to a end of free chain
     */
    if ( !HAS_NEXT_FREE_BLOCK ) { //free_list is pointing to end/beginning of free list
        if ( size <= free_block_size ) {
            *free_list = (char*)(next_ptr) + size; // move free_list beyond allocated block
            SET_BLOCK_SIZE(prev_ptr, size) // set allocated block size
            next_ptr = *free_list;
            SET_BLOCK_SIZE(next_ptr, free_block_size - size) // set new free block size
                    
            return ret;
        }
        return 0;
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a big enough chunk of memory
     */
    if ( size < free_block_size ) {
        *free_list = (char*)(next_ptr) + size; // move free_list beyond allocated block
        SET_BLOCK_SIZE(prev_ptr, size) // set allocated block size
        next_ptr = *free_list;
        SET_BLOCK_SIZE(next_ptr, free_block_size - size) // set new free block size
        return ret;
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        *free_list = *next_ptr;
        return ret;
    }

    // search free list for big enough free space for allocation 
    void** curr_ptr;
    NEXT_FREE_BLOCK(free_block_size, prev_ptr, curr_ptr, next_ptr, ret)
    while ( *next_ptr && free_block_size < size ) {
        NEXT_FREE_BLOCK(free_block_size, prev_ptr, curr_ptr, next_ptr, ret)
    }

    /**
     * TODO: Need to Test!
     * If at end of free chain
     */

    //pointing at end of free chain
    if (!HAS_NEXT_FREE_BLOCK) {
        /**
         * TODO: NEED TO TEST!
         */
        if ( size <= free_block_size ) {
            SET_BLOCK_SIZE(curr_ptr, size) // set allocated block size
            UPDATE_LINKS(prev_ptr, next_ptr, size) // set prev pointer beyond allocated block to next free block
            SET_BLOCK_SIZE(next_ptr, free_block_size - size) // reduce size of next free block
            return ret;
        }
        /**
         * TODO: Need to test!
         * At end of free chain and no blocks were big enough
         */
        return 0; //return NULL if at the end free chain and no blocks where big enough
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a big enough chunk of memory
     */
    if ( size < free_block_size ) {
        SET_BLOCK_SIZE(curr_ptr, size) // set allocated block size
        UPDATE_LINKS(prev_ptr, next_ptr, size) // set prev point beyond allocated block to next free block
        SET_BLOCK_SIZE(next_ptr, free_block_size - size) // reduce size of next free block
        return ret;
    }

    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        *prev_ptr = *next_ptr;
        return ret;
    }

    return 0;
}
