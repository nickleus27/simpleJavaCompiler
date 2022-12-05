#include <stdio.h>
#include "alloc_fflist.c"
#include "free_fflist.c"

int main(){
    int size = 16;
    int* arr = (int*)allocate(size);
    int* arr2 = (int*)allocate(size);
    int* arr3 = (int*)allocate(size);

    for (int i = 0; i < size/4; i++) {
        arr[i]=i;
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr[%d] == %d\n", i, arr[i]);
    }

    //arr2 = (int*)allocate(size);
    for (int i = 0; i < size/4; i++) {
        arr2[i]=i*2;
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr2[%d] == %d\n", i, arr2[i]);
    }
    for (int i = 0; i < size/4; i++) {
        arr3[i]=i*3;
    }
    for (int i = 0; i < size/4; i++) {
        printf("This is arr2[%d] == %d\n", i, arr3[i]);
    }
    delete(arr);
    delete(arr3);
    delete(arr2);
    arr = (int*)allocate(size);
}
