/**
 * @file heap_fflist.c
 * @author Nick Anderson (nickleus27@gmail.com)
 * @brief This is a heap implementation that uses first fit strategy
 * when searching the free list. The heap maintains a singly linked 
 * list, and worst case search is linear.
 * @version 0.1
 * @date 2022-11-17
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * THIS IS STILL A WORK IN PROGRESS
 */


#define MAX_HEAP_SIZE 8176000 /* 8176000 == 8176 kb */

/**
 * @brief https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for
 *  use this for compiling assembly without call frame information unwinding directives
 * 
 * TODO: Also get rid of library includes up top, and the exit call...hand write in the exit to assembly
 */

void* free_list = 0;

void* allocate(int size) {
    //+8 to store size tag, and +8 for storing next tag will be applied by the size % 8
    // for example size == 1, 1+8 ==9, while(size % 8) size++ == 16
    size += 8; //+8 for store size tag
    while (size % 8) { //8 byte aligned
        size++;
    }
    /* initialize heap pointer*/
    if (!free_list) {
        free_list = &free_list + 1; //add 1 == 8 bytes ->first free space
        /* 8176000 == 8176 kb */
        /* dereference to assign value and cast to int* */
        (*(int*)free_list) = MAX_HEAP_SIZE; //assign totals free space
    }
    void** ret;
    void** prev_ptr;
    void** next_ptr;
    prev_ptr = free_list;
    next_ptr = prev_ptr+1;
    /**
     * @brief (free_list pointer) points to a end of free chain
     */
    if ( !*next_ptr ) { //free_list is pointing to end/beginning of free list
        if ( size <= (*(int*)prev_ptr) ) {
            ret = next_ptr;
            free_list = (char*)(next_ptr) + size;
            int free_size = (*(int*)prev_ptr);
            (*(int*)prev_ptr) = size;
            prev_ptr = free_list;
            (*(int*)prev_ptr) = free_size - size;
            return ret;
        }
        return 0;
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a big enough chunk of memory
     */
    if ( size < (*(int*)prev_ptr) ) {
        ret = next_ptr;
        free_list = (char*)(next_ptr) + size;
        int free_size = (*(int*)prev_ptr);
        (*(int*)prev_ptr) = size;
        prev_ptr = free_list;
        (*(int*)prev_ptr) = free_size - size;
        return ret;
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == (*(int*)prev_ptr) ) {
        ret = next_ptr;
        free_list = *next_ptr;
        return ret;
    }

    prev_ptr++; // points to next_link
    next_ptr = *next_ptr; //now pointing at size tag
    // search free list for big enough free space for allocation 
    // list goes free_list[0]==size, free_list[1] == next
    int nodeSize = (*(int*)next_ptr);
    while ( *next_ptr && nodeSize < size ) {
        next_ptr++; //now pointing at next link
        prev_ptr = next_ptr; //next link
        next_ptr = *next_ptr; //move to next node...points at size tag
        nodeSize = (*(int*)next_ptr); // store the size
    }

    /**
     * TODO: Need to Test!
     * If at end of free chain
     */
    void** next_link = next_ptr+1;
    //pointing at end of free chain
    if (!next_link) {
        /**
         * TODO: NEED TO TEST!
         */
        if ( size <= nodeSize ) {
            ret = next_link;
            int free_size = (*(int*)next_ptr);
            (*(int*)next_ptr) = size;
            *next_link = (char*)(next_link) + size;
            next_link = *next_link;
            (*(int*)next_link) = free_size - size;
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
    if ( size < nodeSize ) {
        ret = next_link;
        int free_size = (*(int*)next_ptr);
        (*(int*)next_ptr) = size;
        *prev_ptr = (char*)(next_link) + size;
        next_link = *prev_ptr;
        (*(int*)next_link) = free_size - size;
        return ret;
    }

    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == nodeSize ) {
        ret = next_link;
        *prev_ptr = *next_link;
        return ret;
    }

    return 0;
}
