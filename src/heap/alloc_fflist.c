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

//#include <stdio.h>
#include <stdlib.h>
#define MAX_HEAP_SIZE 8176000 /* 8176000 == 8176 kb */

/**
 * @brief https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for
 *  use this for compiling assembly without call frame information unwinding directives
 * 
 * TODO: Also get rid of library includes up top, and the exit call...hand write in the exit to assembly
 */

void* free_list = 0;

void* allocate(int size) {
    /**
     * TODO: do i add +8 or +1 to offset 8 bytes with long uint?
     */
    
    //+8 to store size tag, and +8 for storing next tag will be applied by the size % 8
    // for example size == 1, 1+8 ==9, while(size % 8) size++ == 16
    size += 8; //+8 for store size tag
    while (size % 8) { //8 byte aligned
        size++;
    }
    /* initialize heap pointer*/
    if (!free_list) {
        //ret = &free_list + 1; //add 1 == 8 bytes
        //free_list = ret + size;
        //return ret;
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
    if ( !*next_ptr ) { //free_list is pointing to end/beginning of free list
        if ( size <= (*(int*)prev_ptr) ) {
            ret = next_ptr;
            free_list = (char*)(next_ptr) + size; //take address of next and add size to that; divide size by 8 because compiler multiplies integer by 8 for void pointer arithmetic
            int free_size = (*(int*)prev_ptr);
            (*(int*)prev_ptr) = size;
            prev_ptr = free_list;
            (*(int*)prev_ptr) = free_size - size;
            return ret;
        }
        return NULL;
    }

/**
 * TODO: Need to move if statement up here to check for free_list->next_ptr == null;
 *      if so free_list points to beginning of free block and can go ahead
 *      and allocate here and return. otherwise move down the chain below
 * 
 */


/**
 * TODO: need to rewrite while loop for moving down chain of free blocks
 * 
 */




    // search free list for big enough free space for allocation 
    // list goies free_list[0]==size, free_list[1] == next
    ret = free_list;
    while ( *ret && (*(int*)ret) < size ) {
        ret = *(ret+1);/*<-------------------------------###*/
        //ret = *ret;
    }

    // found a big enough free space or at at end of free space
    // check to make sure block is big enough in case we are at the end
    if ( (*(int*)ret) < size ) {
        exit(2); //not sure if I want to import a library for this...
    }


/*

    int free_size = (*(int*)ret); //size tag of free block
    void** prev_ptr = ret; //pointer to update
    void** next_ptr = ret+1;

 // TODO: need to move prev_ptr and next_ptr before while loop and use to move down chain






     // TODO: Problem right here. updating size and then overwriting below 

    (*(int*)ret) = size; //update allocated block to store size of aloc block at block[-1]
    ret = ret +1; //move block[0] one space ahead of size tag

     // check if there is a next pointer if so then we are not at end of list
    if (*next_ptr && free_size == size) { //not at end of free block

        // TODO: Problem right here. overwriting alloc block size

        *prev_ptr = *next_ptr; // free block all used up, just point to next free block;
        // check if free_list pointer is pointing to this block
        if (free_list == prev_ptr) {
            free_list = *prev_ptr; //update heap pointer to point to new free block
        }
        return ret;
    }

    // check if there is a next pointer if so then we are not at end of list
    if ( *next_ptr && size < free_size ) {

        // TODO: Problem right here. overwriting alloc block size

        *prev_ptr = (char*)(*next_ptr) + size; //update pointer
        // check if free_list pointer is pointing to this block
        if (free_list == prev_ptr) {
            free_list = *prev_ptr; //update heap pointer to point to new free block
        }
        (*(int*)(*prev_ptr)) = free_size - size; //update new free block size
        //printf("the size is %d\n", (*(int*)(prev_ptr+1)));
        return ret;
    }

    // at end of list and using last bit of free heap space
    if (!*next_ptr && free_size == size) { //not at end of free block
        *prev_ptr = (char*)(&(*next_ptr)) + size; // free block all used up, just point to next free block; divide size by 8 because compiler multiplies integer by 8 for void pointer arithmetic
        // check if free_list pointer is pointing to this block
        if (free_list == prev_ptr) {
            free_list = *prev_ptr; //update heap pointer to point to new free block
        }
        (*(int*)(*prev_ptr)) = free_size - size;
        return ret;
    }

    // check if at end of free list and alloc block is smaller then free space left
    if ( !*next_ptr &&  size < free_size){
         // TODO: Problem right here. overwriting alloc block size
        *prev_ptr = (char*)(&(*next_ptr)) + size; //take address of next and add size to that; divide size by 8 because compiler multiplies integer by 8 for void pointer arithmetic
        // check if free_list pointer is pointing to end of allocated block
        if (free_list == prev_ptr) {
            free_list = *prev_ptr; //update heap pointer to point at end of list
        }
        (*(int*)(*prev_ptr)) = free_size - size;
        //printf("the size is %d\n", (*(int*)(*prev_ptr)));
        return ret;
    }
*/
    /**
     * @brief need to have final return statement. Do i need the last check or is that guaranteed to be true
     * at that point?
     * 
     */
    return NULL;
}
