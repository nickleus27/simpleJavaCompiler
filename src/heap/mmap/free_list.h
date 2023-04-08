/**
 * @file free_list.h
 * @author Nick Anderson (nickleus27@gmail.com)
 * @brief This file is the header for first fit free list allacator
 * @version 0.1
 * @date 2023-03-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef FF_FREE_LIST_H
#define FF_FREE_LIST_H

#define MAGIC_NUMBER 0x8267C39848DB8687
#define MAX_HEAP_SIZE 8176000 /* ulimit -s == 8176 kb */
#define PAGESIZE 16384 /* getconf PAGESIZE == 16384 */
#define METADATA 16  /* size needed for size tag and next link */
#define SIZETAG 8   /* each allocated block needs to store the size of the block */

typedef struct _header_t {
    unsigned long size;
    unsigned long magic_number;
    void* ret_ptr;
}header_t;

typedef struct _node_t {
    unsigned long size;
    struct _node_t* next;
}node_t;

extern void* free_list_start;

void* allocate(int size);
void delete(void* toDelete);

#endif /* end of FF_FREE_LIST_H */
