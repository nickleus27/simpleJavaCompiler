/**
 * @file heap_ff_mmap.h
 * @author Nick Anderson (nickleus27@gmail.com)
 * @brief This is a heap implementation that uses first fit strategy
 * when searching the free list. The heap maintains a singly linked 
 * list, and worst case search is linear.
 * @version 0.1
 * @date 2023-03-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */

typedef struct _node_t node_t;

void* allocate(int size);
extern void* free_list_start;