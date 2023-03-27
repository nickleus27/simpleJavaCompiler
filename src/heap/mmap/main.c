#include <stdio.h>
#include <assert.h>
#include "free_list.h"

int main(){
    int max = 20000; //16384 -8 -8 -16; // -8 for size tag, -8 for -1 because address starts at 0, -16 for last size and next pointer
    int* test1 = (int*)allocate(max);
    assert(test1);

    int i;
    for(i=0; i<max/4; i++) {
        test1[i] = i;
        printf("%d\n", test1[i]);
    }
    
    int* test2 = (int*)allocate(16384);
    assert(test2);
    delete(test1);
    
   
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
    //delete(arr2);
    delete(arr3);
    //delete(arr2);
    arr3 = (int*)allocate(32);
    if(!arr3){
        printf("null, did not allocate\n");
    }
    arr = (int*)allocate(8);
    int* arr6 = (int*)allocate(size);
    int* arr7 = (int*)allocate(size);
    return 0;
}
