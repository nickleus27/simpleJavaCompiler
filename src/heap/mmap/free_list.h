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

typedef struct _header_t {
    unsigned long size;
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
