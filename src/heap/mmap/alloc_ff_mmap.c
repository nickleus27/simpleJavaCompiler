/**
 * @file heap_ff_mmap.c
 * @author Nick Anderson (nickleus27@gmail.com)
 * @brief This is a heap implementation that uses first fit strategy
 * when searching the free list. The heap maintains a singly linked 
 * list, and worst case search is linear.
 * @version 0.1
 * @date 2023-03-12
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * THIS IS STILL A WORK IN PROGRESS
 */
#include "free_list.h"

/**
 * @brief https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for
 *  use this for compiling assembly without call frame information unwinding directives
 * 
 */

void * free_list_start = 0;

void* allocate(int size) {
    // inline assembly to save argument passed in at proper location expected
    // for sjava language. run make assembly
    #if !defined(DEBUG)
    __asm__ ( /* inline assembly statement. need to recheck stack offset */
        "\tldr\tw0, [sp, #112] //save size arg into this frames memory"
    );
    #endif
    //+8 to store size tag
    // for example size == 1, 1+8 ==9, while(size % 8) size++ == 16
    size += SIZETAG; // +8 for store size tag
    while (size % 8) { // 8 byte aligned
        size++;
    }
    node_t* tmp_first_ptr = free_list_start;
    /* initialize heap pointer */
    if (!tmp_first_ptr) {
        unsigned long mmapSize = size + METADATA;
        mmapSize = ((mmapSize/PAGESIZE) * PAGESIZE) + PAGESIZE;
        
        __asm__ (
            "mov x0, 0           // start address\n"
            "mov x1, x8          // pagesize length\n" // x8 holds mmapSize
            "mov x2, 3           // rw- PROT_READ | PROT_WRITE\n"
            "mov x3, 0x1001      // flags MAP_ANON | MAP_SHARED\n"
            "mov x4, -1          // file descriptor\n"
            "mov x5, 0           // offset\n"
            "mov x16, 197        // mmap\n"
            "svc #0x80\n"
            "mov x8, x0\n"       // save returned pointer to tmp_first_ptr
            "str x8, [sp, #72]"
        );
        if (!tmp_first_ptr) {
            return 0;
        }
        free_list_start = tmp_first_ptr;  // point static pointer to new alloc space
        tmp_first_ptr->size = mmapSize; // assign total free space
    }

    node_t* prev_ptr = tmp_first_ptr;
    node_t* next_ptr = tmp_first_ptr->next;
    header_t* ret = (header_t*)tmp_first_ptr;
    int free_block_size = prev_ptr->size;

     // if (free_list pointer) points to a end of free chain
    if ( !tmp_first_ptr->next ) { //free_list is pointing to end/beginning of free list
        if ( free_block_size - size >= METADATA ) {
            free_list_start = (char*)(tmp_first_ptr) + size; // move free_list beyond allocated block
            ret->size = size;
            tmp_first_ptr = free_list_start;
            tmp_first_ptr->size = free_block_size - size;
            if (tmp_first_ptr->next) { // shouldnt have next pointer because at end of list set back to 0 (this block has already been alloc before)
                tmp_first_ptr->next = 0;
            }
            return &ret->ret_ptr;
        }
        unsigned long mmapSize = size + METADATA;
        mmapSize = ((mmapSize/PAGESIZE) * PAGESIZE) + PAGESIZE;
        
        __asm__ (
            "mov x0, 0           // start address\n"
            "mov x1, x8          // pagesize length\n" // x8 holds mmapSize
            "mov x2, 3           // rw- PROT_READ | PROT_WRITE\n"
            "mov x3, 0x1001      // flags MAP_ANON | MAP_SHARED\n"
            "mov x4, -1          // file descriptor\n"
            "mov x5, 0           // offset\n"
            "mov x16, 197        // mmap\n"
            "svc #0x80\n"
            "mov x8, x0\n"       // save returned pointer to tmp_first_ptr
            "str x8, [sp, #72]"
        );
        if (!tmp_first_ptr) {
            return 0;
        }
        /**
         * TODO:  Need to check if address returned is below current free_list_start
         * 
         */
        // if returned mmap address is contiguous
        if ((char*)(prev_ptr) + free_block_size == (char*)tmp_first_ptr) {
            tmp_first_ptr = free_list_start;
            free_block_size = free_block_size + mmapSize;
            free_list_start = (char*)(tmp_first_ptr) + size;
            ret->size = size;
            tmp_first_ptr = free_list_start;
            tmp_first_ptr->size = free_block_size - size;
            if (tmp_first_ptr->next) { // shouldnt have next pointer because at end of list set back to 0 (this block has already been alloc before)
                tmp_first_ptr->next = 0;
            }
            return &ret->ret_ptr;
        } else { // else separate address spaces, link together
            prev_ptr->next = tmp_first_ptr;
            next_ptr = prev_ptr->next;
            tmp_first_ptr->size = mmapSize;
        }
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        free_list_start = tmp_first_ptr->next;
        return &ret->ret_ptr;
    }

        /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a big enough chunk of memory
     */
    if ( free_block_size - size >= METADATA ) { // need to make sure enough free space for size and next of new smaller free block
        free_list_start = (char*)(tmp_first_ptr) + size;
        ret->size = size;
        tmp_first_ptr = (node_t*)free_list_start;
        tmp_first_ptr->size = free_block_size - size;
        tmp_first_ptr->next = prev_ptr->next;
        return &ret->ret_ptr;
    }

    // search free list for big enough free space for allocation 
    free_block_size = next_ptr->size;
    ret = (header_t*)next_ptr;
    while ( next_ptr->next && free_block_size < size ) {
        prev_ptr= next_ptr;
        next_ptr = next_ptr->next;
        ret = (header_t*)next_ptr;
        free_block_size = next_ptr->size;
    }

    /**
     * TODO: Need to Test!
     * If at end of free chain
     */

    //pointing at end of free chain
    if (!next_ptr->next) {
        /**
         * TODO: NEED TO TEST!
         */
        if ( free_block_size - size >= METADATA ) {
            ret->size = size;
            prev_ptr->next = (node_t*)((char*)(next_ptr) + size);
            next_ptr = prev_ptr->next;
            next_ptr->size = free_block_size - size;
            if (next_ptr->next) { // shouldnt have next pointer because at end of list set back to 0 (this block has already been alloc before)
                next_ptr = 0;
            }
            return &ret->ret_ptr;
        }
        /**
        * TODO: Need to check for size to ask from mmap
        * 
        */
        unsigned long mmapSize = size + METADATA;
        return 0; //return NULL if at the end free chain and no blocks where big enough
    }

    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        prev_ptr->next = next_ptr->next;
        return &ret->ret_ptr;
    }

        /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a big enough chunk of memory
     */
    if ( free_block_size - size >= METADATA ) { // need to make sure enough free space for size and next of new smaller free block
        ret->size = size;
        prev_ptr->next = (node_t*)((char*)(next_ptr) + size);
        next_ptr = prev_ptr->next;
        next_ptr->size = free_block_size - size;
        next_ptr->next = (node_t*)ret->ret_ptr;
        return &ret->ret_ptr;
    }
    /**
     * TODO: need te re mmap again here.
     * 
     */
    unsigned long mmapSize = size + METADATA;
    return 0;
}
