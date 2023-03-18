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


#define MAX_HEAP_SIZE 8159232 /* getconf PAGESIZE == 16384 (* 498 = 8159232), ulimit -s == 8176 kb */
#define METADATA 16  /* size needed for size tag and next link */
/**
 * @brief Each free block has these 2 values followed by a block of free memory
 *        [0, 1,...]
 *        [(0) size of block, (1) points to next free block, ...(free space)...] 
 */
#define FIRST_FREE_BLOCK(size, first, prev, next, ret) { \
    prev = first; \
    size = (*(int*)prev); \
    next = first; \
    next++; \
    ret = next; \
}
#define NEXT_FREE_BLOCK(size, prev, curr_size, next, ret) { \
    prev = next; \
    next = *next; \
    curr_size = next; \
    next++; \
    ret = next; \
    size = (*(int*)curr_size); \
}
#define MOVE_FREE_LIST_START_PTR(next, size) free_list_start = (char*)(next) + size;
#define HAS_NEXT_FREE_BLOCK *next_ptr
#define SET_BLOCK_SIZE(pointer, size) (*(int*)pointer) = size;
#define UPDATE_LINKS(prev, next, size) *prev = (char*)(next) + size; next = *prev;

/**
 * @brief https://stackoverflow.com/questions/2529185/what-are-cfi-directives-in-gnu-assembler-gas-used-for
 *  use this for compiling assembly without call frame information unwinding directives
 * 
 * TODO: Also get rid of library includes up top, and the exit call...hand write in the exit to assembly
 */

void * free_list_start = 0;

void* allocate(int size) {
    // inline assembly to save argument passed in at proper location expected
    // for sjava language. run make assembly
    #if !defined(DEBUG)
    __asm__ ( /* inline assembly statement. need to recheck stack offset */
        "\tldr\tw0, [sp, #96] //save size arg into this frames memory"
    );
    #endif
    //+8 to store size tag, and +8 for storing next tag will be applied by the size % 8
    // for example size == 1, 1+8 ==9, while(size % 8) size++ == 16
    size += 8; //+8 for store size tag
    while (size % 8) { //8 byte aligned
        size++;
    }
    void** tmp_first_ptr = &free_list_start;
    /* initialize heap pointer */
    if (!*tmp_first_ptr) {
            __asm__ (
       "mov x0, 0           // start address\n"
        "mov x1, 4096        // length\n"
        "mov x2, 3           // rw- PROT_READ | PROT_WRITE\n"
        "mov x3, 0x1001      // flags MAP_ANON | MAP_SHARED\n"
        "mov x4, -1          // file descriptor\n"
        "mov x5, 0           // offset\n"
        "mov x16, 197        // mmap\n"
        "svc #0x80\n"
        "mov x8, x0\n"
        "str x8, [sp, #56]"
    );
        free_list_start = tmp_first_ptr;  // point static pointer to new alloc space
        //*free_list = free_list + 1; //add 1 == 8 bytes ->first free space
        /* 8176000 == 8176 kb */
        (*(int*)(tmp_first_ptr)) = MAX_HEAP_SIZE - 32; //assign totals free space (-8 pointer -16 for last free block that holds size=0, next=null)        
    } else {
        tmp_first_ptr = *tmp_first_ptr;
    }
    int free_block_size;
    void** prev_ptr;
    void** next_ptr;
    void** ret;
    FIRST_FREE_BLOCK(free_block_size, tmp_first_ptr, prev_ptr, next_ptr, ret)

    /**
     * @brief (free_list pointer) points to a end of free chain
     */
    if ( !HAS_NEXT_FREE_BLOCK ) { //free_list is pointing to end/beginning of free list
        if ( size <= free_block_size ) {
            MOVE_FREE_LIST_START_PTR(next_ptr, size) // move free_list beyond allocated block
            SET_BLOCK_SIZE(prev_ptr, size) // set allocated block size
            next_ptr = free_list_start;
            SET_BLOCK_SIZE(next_ptr, free_block_size - size) // set new free block size
                    
            return ret;
        }
        return 0;
    }
    /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        free_list_start = *next_ptr;
        return ret;
    }

        /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * (free_list pointer) points to a big enough chunk of memory
     */
    if ( free_block_size - size >= METADATA ) { // need to make sure enough free space for size and next of new smaller free block
        MOVE_FREE_LIST_START_PTR(next_ptr, size) // move free_list beyond allocated block
        SET_BLOCK_SIZE(prev_ptr, size) // set allocated block size
        next_ptr = free_list_start;
        SET_BLOCK_SIZE(next_ptr, free_block_size - size) // set new free block size
        return ret;
    }

    // search free list for big enough free space for allocation 
    void** curr_ptr;
    NEXT_FREE_BLOCK(free_block_size, prev_ptr, curr_ptr, next_ptr, ret)
    while ( *next_ptr && free_block_size < size ) {
        NEXT_FREE_BLOCK(free_block_size, prev_ptr, curr_ptr, next_ptr, ret)
    }

    /**
     * TODO: Need to Test!
     * If at end of free chain
     */

    //pointing at end of free chain
    if (!HAS_NEXT_FREE_BLOCK) {
        /**
         * TODO: NEED TO TEST!
         */
        if ( size <= free_block_size ) {
            SET_BLOCK_SIZE(curr_ptr, size) // set allocated block size
            UPDATE_LINKS(prev_ptr, next_ptr, size) // set prev pointer beyond allocated block to next free block
            SET_BLOCK_SIZE(next_ptr, free_block_size - size) // reduce size of next free block
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
     * prev_pointer points to a an equal size chunk of memory
     * free_size does not need to be updated, just point free_list to
     */
    if ( size == free_block_size ) {
        *prev_ptr = *next_ptr;
        return ret;
    }

        /**
     * TODO: NEED TO TEST!
     * @brief if not at end of free chain but first free block and
     * prev_pointer points to a big enough chunk of memory
     */
    if ( free_block_size - size >= METADATA ) { // need to make sure enough free space for size and next of new smaller free block
        SET_BLOCK_SIZE(curr_ptr, size) // set allocated block size
        UPDATE_LINKS(prev_ptr, next_ptr, size) // set prev point beyond allocated block to next free block
        SET_BLOCK_SIZE(next_ptr, free_block_size - size) // reduce size of next free block
        return ret;
    }

    return 0;
}