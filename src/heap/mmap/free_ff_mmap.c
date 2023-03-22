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

#include "free_list.h"
/**
 * 1. free blocks at the end of chain
 *      free list points to end of chain
 *      
 * 2. blocks in the middle
 *      and free_list points to middle block
 * 
 * 3. blocks that point to adjacent free block
 */

#define NEXT_LINK(prev, link, next) \
    /* prev_ptr now point at the next size tag */        prev = next; \
    /* point to next link */                             link = prev + 1; \
    /* next_ptr updated to next free block size tag */   next = *link;

void delete(void* del_ptr) {
    node_t* prev_ptr = free_list_start; //points to first free block
    node_t* next_ptr = prev_ptr;
    header_t* toDelete = (header_t*)((char*)del_ptr - 8); //now points to size of block to deallocate, cast to char (1byte) and move 8 bytes to back

    /* check if prev_pointer points beyond toDelete. If so toDelete is in front of free_list pointer */
    if ( (void*)toDelete < (void*)prev_ptr ) {
        #define SET_FIRST_FREE_BLOCK(prev, next, toDelete) \
    /* point to block to be freed */                    prev = toDelete; \
    /* update free list to point to first free block */ free_list_start = prev; \
    /* next pointer */                                  prev++; \
    /* point to next free block */                      *prev = next; \

        prev_ptr = (node_t*)toDelete;
        free_list_start = prev_ptr;
        prev_ptr->next = next_ptr;
        /* check for adjacent free block */
        if ((void*)next_ptr == (void*)(((char*)(&prev_ptr->next)) + toDelete->size)) {
            //void** nextSize = *prev_ptr;

            int size1 = prev_ptr->size;
            int size2 = next_ptr->size;

            prev_ptr->size = size1 + size2; //update the free block size to combine two adjacent blocks
            //next_ptr++; // move to next pointer
            prev_ptr->next = next_ptr->next; //update the next pointer to point to next block from the second adjacent block
        }
        return;
    }
    /**
     * TODO: 
     * Create macro for better readability
     * 
     */
    //void** next_link; // ????? why? delete this
    //next_ptr++;
    //next_link = next_ptr; //next_link points at the prev_ptr next link
    //next_ptr = *next_ptr; //next now point to next free block size tag
    /* move down the chain until prev_ptr is the link behind block to dealloc*/
    next_ptr = next_ptr->next;
    while ( (void*)next_ptr < (void*)toDelete ) {
        prev_ptr = next_ptr;
        next_ptr = next_ptr->next;
        //prev_ptr = next_ptr; //prev_ptr now point at the next size tag
        //next_link = prev_ptr + 1; //next_link points at the prev_ptr next link
        //next_ptr++; //next_ptr now a next link
        //next_ptr = *next_link;//next_ptr updated to next free block size tag
    }
    int prev_size_block = prev_ptr->size;//(*(int*)prev_ptr); //size of previous free block
    int toDelete_size_block = toDelete->size;//(*(int*)toDelete);
    int next_size_block = next_ptr->size; //(*(int*)next_ptr);
    /* +8 to offset from size block to next-link */
    /*check to see if prev_ptr free block is adjacent to toDelete block and toDelete block is adjacent to next_ptr free block*/
    if ((void*)((char*)(&prev_ptr->next) + prev_size_block) == (void*)toDelete && (void*)((char*)(&toDelete->ret_ptr) + toDelete_size_block) == (void*)next_ptr)
    {
        //(*(int*)prev_ptr) = prev_size_block + toDelete_size_block + next_size_block;
        //next_ptr++; //move to next_link
        //*next_link = *next_ptr; //set next link to reach over adjacent free blocks
        prev_ptr->size = prev_size_block + toDelete_size_block + next_size_block;
        prev_ptr->next = next_ptr->next;
        return;
    }
    /*check to see if prev_ptr free block is adjacent to toDelete block */
    if ( (void*)((char*)(&prev_ptr->next) + prev_size_block) == (void*)toDelete ) {
        prev_ptr->size = prev_size_block + toDelete_size_block;
        //(*(int*)prev_ptr) = prev_size_block + toDelete_size_block; //update size of previous block
        //*next_link = next_ptr; //set next link to reach over adjacent free block
        return;
    } 
    /*check to see if toDelete block is adjacent to next_ptr free block*/
    if ( (void*)((char*)(&toDelete->ret_ptr) + toDelete_size_block) == (void*)next_ptr ) {
        prev_ptr->next = (node_t*)toDelete;
        prev_ptr = (node_t*)toDelete;
        prev_ptr->size = toDelete_size_block + next_size_block;
        prev_ptr->next = next_ptr->next;
        
        //*next_link = toDelete;
        //(*(int*)toDelete) = toDelete_size_block + next_size_block; //update size of toDelete block
        //next_link = (void**)((char*)toDelete+8); // point to toDelete block next_link
        //next_ptr++; // point to next_ptr blocks next_link
        //*next_link = *next_ptr; //set next link to reach over adjacent free block
        return;
    } 

    /* link prev_ptr next-link to toDelete block and toDelete block to next_ptr*/
    //*next_link = toDelete;
    //next_link = (void**)((char*)toDelete +8); //now points to toDelete block next link
    //*next_link = next_ptr;
    prev_ptr->next = (node_t*)toDelete;
    prev_ptr = (node_t*)toDelete;
    prev_ptr->next = next_ptr;
}
