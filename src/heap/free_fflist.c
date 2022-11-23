/**
 * @file free_fflist.c
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

#include "alloc_fflist.h"
/**
 * 1. free blocks at the end of chain
 *      free list points to end of chain
 *      
 * 2. blocks in the middle
 *      and free_list points to middle block
 * 
 * 3. blocks that point to adjacent free block
 */
void delete(void* toDelete) {
    if (!free_list) {
        //exit
        return;
    }
    void** prev_ptr = free_list; //points to first free block
    void** next_ptr = prev_ptr+1;
    toDelete = (char*)toDelete -8; //now points to size of block to deallocate, cast to char (1byte) and move 8 bytes to back

    /* check if prev_pointer points beyond toDelete. If so toDelete is in between free_list pointer and prev_pointer */
    if ( toDelete < (void*)prev_ptr ) {
        prev_ptr = toDelete; // point to block to be freed
        free_list = prev_ptr; // update free list to point to first free block
        prev_ptr++; //next pointer
        *prev_ptr = (char*)next_ptr-8; //cast to char (1byte) move 8 bytes back, point to next free block
        /* check for adjacent free block */
        if (*prev_ptr == (char*)prev_ptr + (*(int*)toDelete)) {
            void** updateSize = toDelete;
            void** nextSize = *prev_ptr;

            int size1 = (*(int*)nextSize);
            int size2 = (*(int*)toDelete);

            *(int*)updateSize = size1 + size2; //update the free block size to combine two adjacent blocks

            next_ptr = next_ptr-1;
            //next_ptr = (*(char*)prev_ptr)+8;


            *prev_ptr = next_ptr; //update the next pointer to point to next block from the second adjacent block
        }
        return;
    }
    /* move down the chain until prev_ptr is the link behind block to dealloc*/
    while ( *next_ptr && *next_ptr < toDelete ) {
        prev_ptr = *next_ptr;
        next_ptr = (prev_ptr+1);
    }
    /**
     * TODO: when free blocks are in front of toDelete
     * 
     */

}
