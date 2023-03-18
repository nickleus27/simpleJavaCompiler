#include <stdio.h>
#include <assert.h>
#include "alloc_ff_mmap.c"
#include "free_ff_mmap.c"

int main(){
    
    //int* test1 = (int*)allocate(8175960);
    /*
    int i;
    for(i=0; i<8175960/4; i++) {
        arr1[i] = i;
        printf("%d\n", arr1[i]);
    }
    */
    //assert(test1);
    //int* test2 = (int*)allocate(1);
    //assert(!test2);
    //delete(test1);
    
   
    int size = 16;
    int* arr = (int*)allocate(size);
    int* arr2 = (int*)allocate(size);
    int* arr3 = (int*)allocate(24);
    int* arr4 = (int*)allocate(size);
    int* arr5 = (int*)allocate(size);


    for (int i = 0; i < size/4; i++) {
        arr[i]=i;
    }
        //arr2 = (int*)allocate(size);
    for (int i = 0; i < size/4; i++) {
        arr2[i]=i*2;
    }
        for (int i = 0; i < size/4; i++) {
        arr3[i]=i*3;
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr[%d] == %d\n", i, arr[i]);
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr2[%d] == %d\n", i, arr2[i]);
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr2[%d] == %d\n", i, arr3[i]);
    }
    delete(arr);
    delete(arr3);
    arr3 = (int*)allocate(24);
    if(!arr3){
        printf("null, did not allocate\n");
    }
    arr = (int*)allocate(8);
    int* arr6 = (int*)allocate(size);
    
}
