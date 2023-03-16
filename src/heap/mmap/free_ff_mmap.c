/**
 * @file free_ff_mmap.c
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
 * https://stackoverflow.com/questions/2855121/what-is-the-purpose-of-using-pedantic-in-the-gcc-g-compiler
 */

#include "alloc_ff_mmap.h"
/**
 * 1. free blocks at the end of chain
 *      free list points to end of chain
 *      
 * 2. blocks in the middle
 *      and free_list points to middle block
 * 
 * 3. blocks that point to adjacent free block
 */
#define SET_FIRST_FREE_BLOCK(prev, next, toDelete) \
{ \
    /* point to block to be freed */                    prev = toDelete; \
    /* update free list to point to first free block */ free_list_start = prev; \
    /* next pointer */                                  prev++; \
    /* point to next free block */                      *prev = next; \
}

#define NEXT_LINK(prev, link, next) \
{ \
    /* prev_ptr now point at the next size tag */        prev = next; \
    /* point to next link */                             link = prev + 1; \
    /* next_ptr updated to next free block size tag */   next = *link; \
}

void delete(void* toDelete) {
    if (!free_list_start) {
        //exit
        return;
    }
    void** prev_ptr = free_list_start; //points to first free block
    void** next_ptr = prev_ptr;
    toDelete = (char*)toDelete - 8; //now points to size of block to deallocate, cast to char (1byte) and move 8 bytes to back

    /* check if prev_pointer points beyond toDelete. If so toDelete is in front of free_list pointer */
    if ( toDelete < (void*)prev_ptr ) {
        SET_FIRST_FREE_BLOCK(prev_ptr, next_ptr, toDelete)
        /* check for adjacent free block */
        if (*prev_ptr == (char*)prev_ptr + (*(int*)toDelete)) {
            void** nextSize = *prev_ptr;

            int size1 = (*(int*)nextSize);
            int size2 = (*(int*)toDelete);

            *(int*)toDelete = size1 + size2; //update the free block size to combine two adjacent blocks
            next_ptr++; // move to next pointer
            *prev_ptr = *next_ptr; //update the next pointer to point to next block from the second adjacent block
        }
        return;
    }
    /**
     * TODO: 
     * Create macro for better readability
     * 
     */
    void** next_link; // ????? why? delete this
    next_ptr++;
    next_link = next_ptr; //next_link points at the prev_ptr next link
    next_ptr = *next_ptr; //next now point to next free block size tag
    /* move down the chain until prev_ptr is the link behind block to dealloc*/
    while ( (void*)next_ptr < toDelete ) {
        prev_ptr = next_ptr; //prev_ptr now point at the next size tag
        next_link = prev_ptr + 1; //next_link points at the prev_ptr next link
        //next_ptr++; //next_ptr now a next link
        next_ptr = *next_link;//next_ptr updated to next free block size tag
    }
    int prev_size_block = (*(int*)prev_ptr); //size of previous free block
    int toDelete_size_block = (*(int*)toDelete);
    int next_size_block = (*(int*)next_ptr);
    /* +8 to offset from size block to next-link */
    /*check to see if prev_ptr free block is adjacent to toDelete block and toDelete block is adjacent to next_ptr free block*/
    if ( (char*)prev_ptr + prev_size_block +8 == toDelete && (char*)toDelete + toDelete_size_block +8 == (void*)next_ptr ) {
        (*(int*)prev_ptr) = prev_size_block + toDelete_size_block + next_size_block;
        next_ptr++; //move to next_link
        *next_link = *next_ptr; //set next link to reach over adjacent free blocks
        return;
    }
    /*check to see if prev_ptr free block is adjacent to toDelete block */
    if ( (char*)prev_ptr + prev_size_block +8 == toDelete ) {
        (*(int*)prev_ptr) = prev_size_block + toDelete_size_block; //update size of previous block
        *next_link = next_ptr; //set next link to reach over adjacent free block
        return;
    } 
    /*check to see if toDelete block is adjacent to next_ptr free block*/
    if ( (char*)toDelete + toDelete_size_block +8 == (void*)next_ptr ) {
        *next_link = toDelete;
        (*(int*)toDelete) = toDelete_size_block + next_size_block; //update size of toDelete block
        next_link = (void**)((char*)toDelete+8); // point to toDelete block next_link
        next_ptr++; // point to next_ptr blocks next_link
        *next_link = *next_ptr; //set next link to reach over adjacent free block
        return;
    } 

    /* link prev_ptr next-link to toDelete block and toDelete block to next_ptr*/
    *next_link = toDelete;
    next_link = (void**)((char*)toDelete +8); //now points to toDelete block next link
    *next_link = next_ptr;
}
