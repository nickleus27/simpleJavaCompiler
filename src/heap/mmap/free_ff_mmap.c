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

/**
 * TODO: Add munmap? Check to see if a node is the size of a page and free with munmap?
 * if freeing in this way, update list linkage appropriately. more meta data would be needed
 * to determine if memory location is a boundary that can be freed. 
 * that is address (mmap_base + 0 + pagesize) could be freed but 
 * address (mmap_base + 1 + pagesize) should not be freed
 */
#include <assert.h>
void delete(void* del_ptr) {
    node_t* prev_ptr = free_list_start; //points to first free block
    node_t* next_ptr = prev_ptr;
    header_t* toDelete = (header_t*)((char*)del_ptr - METADATA); //now points to size of block to deallocate, cast to char (1byte) and move 16 bytes to back

    /**
     * TODO: add assembly to exit without assert included
     * find assembly to jump to exit point
     */
    assert(toDelete->magic_number == MAGIC_NUMBER);

    /* check if prev_pointer points beyond toDelete. If so toDelete is in front of free_list pointer */
    if ( (void*)toDelete < (void*)prev_ptr ) {
        prev_ptr = (node_t*)toDelete;
        free_list_start = prev_ptr;
        prev_ptr->next = next_ptr;
        /* check for adjacent free block */
        if ((void*)next_ptr == (void*)(((char*)(prev_ptr)) + toDelete->size)) {
            int size1 = prev_ptr->size;
            int size2 = next_ptr->size;

            prev_ptr->size = size1 + size2; //update the free block size to combine two adjacent blocks
            prev_ptr->next = next_ptr->next; //update the next pointer to point to next block from the second adjacent block
        }
        return;
    }
    
    /* move down the chain until prev_ptr is the link behind block to dealloc*/
    next_ptr = next_ptr->next;
    while ( (void*)next_ptr < (void*)toDelete ) {
        prev_ptr = next_ptr;
        next_ptr = next_ptr->next;
    }
    int prev_size_block = prev_ptr->size;
    int toDelete_size_block = toDelete->size;
    int next_size_block = next_ptr->size;
    /*check to see if prev_ptr free block is adjacent to toDelete block and toDelete block is adjacent to next_ptr free block*/
    if ((void*)((char*)(prev_ptr) + prev_size_block) == (void*)toDelete && (void*)((char*)(toDelete) + toDelete_size_block) == (void*)next_ptr)
    {
        prev_ptr->size = prev_size_block + toDelete_size_block + next_size_block;
        prev_ptr->next = next_ptr->next;
        return;
    }
    /*check to see if prev_ptr free block is adjacent to toDelete block */
    if ( (void*)((char*)(prev_ptr) + prev_size_block) == (void*)toDelete ) {
        prev_ptr->size = prev_size_block + toDelete_size_block;
        return;
    } 
    /*check to see if toDelete block is adjacent to next_ptr free block*/
    if ( (void*)((char*)(toDelete) + toDelete_size_block) == (void*)next_ptr ) {
        prev_ptr->next = (node_t*)toDelete;
        prev_ptr = (node_t*)toDelete;
        prev_ptr->size = toDelete_size_block + next_size_block;
        prev_ptr->next = next_ptr->next;
        return;
    } 

    prev_ptr->next = (node_t*)toDelete;
    prev_ptr = (node_t*)toDelete;
    prev_ptr->next = next_ptr;
}
