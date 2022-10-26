/**
 * File by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include "env_arm64.h"
#include <stdlib.h>
#include "../codegen/MachineDependent.h"

struct stack_{
    enum{A64_Var, A64_endScope}kind;
    union{
        struct{
            int* offset;
            stack rest;
        }var;
        struct{
            int nextScope;
            stack rest;
        }endScope;
    }u;
};
struct stack_env_
{
    int space_left;
    int size;
    stack *vector;
    env_sizes mem_sizes;
    //env_sizes block_totals;
};
/* globals */
//stack stack_8, stack_4, stack_1 = NULL;

stack_env new_stack_env(int init_size , bool functionStack)
{
    stack_env env = (stack_env)malloc(sizeof(*env));
    env->vector = (stack *)malloc(init_size * sizeof(stack *));//initialize array to size set to argument
    if(!env->vector){
        printf("Failed malloc\n");
        exit(EXIT_FAILURE);
    }
    env->size = 0;//array is empty
    env->space_left = init_size;//space left is initial size (16)
    if(functionStack){
        env->size++;
        env->space_left--;
    }
    return env;
}

stack new_stack_elem(int* data, stack first){
    stack retval = (stack)malloc(sizeof(*retval));
    //retval->u.var.offset = (int*)malloc(sizeof(int));
    retval->u.var.offset = data;
    retval->u.var.rest = first;
    return retval;
}


void enter_arm64( stack_env env, int index, int* data ){
    if(env->size <= index){
        if(env->space_left == 0 ){//if array is full
            stack *ptrTemp = NULL;
            int maxLength = env->size<<INCREMENT;//1;//double the size of array (maxLength x 2)
            ptrTemp = (stack *)realloc(env->vector, maxLength * sizeof(stack *));//have vptrTemp = realloc incase not succesful
            if( ptrTemp == NULL )
            {//exit it realloc failed
                printf("Failed realloc\n");
                exit(EXIT_FAILURE);
            }
            env->space_left = maxLength - env->size;//space left after growing
            env->vector = ptrTemp;//have env point to realloc array
        }
        stack stack_elem = new_stack_elem(data, NULL);
        env->vector[env->size++] = stack_elem;//now that we have space push data to next available spot
        env->space_left--;
    }else{
        env->vector[index] = new_stack_elem(data, stack_env_get(env, index));
    }
}

//free memory back to heap
void free_arm64_env(stack_env env)
{
    free(env->vector);
    free(env);
}

//return amount of elements in array
int stack_env_size(stack_env env)
{
    return env->size;
}

/* returns stack at index */
stack stack_env_get(stack_env env, int index )
{
    if(index >= env->size || index < 0)
    {
        printf("Error: index %d out of bounds ", index);
        free_arm64_env(env);
        exit(EXIT_FAILURE);
    }
    return env->vector[index];
}


void stack_env_clear(stack_env env, int size)
{
    while(env->size != size)
    {
        env->vector[--env->size] = NULL;
        env->space_left++;
    }
}


void addMemSizes(stack_env env, env_sizes mem){//}, env_sizes block){
    env->mem_sizes = mem;
    //env->block_totals = block;
}


void arm64endScope(stack_env env, int scope){
    if(scope){
    stack elem = (stack)malloc(sizeof(struct stack_));
    elem->u.endScope.nextScope = scope+1;
    elem->u.endScope.rest = env->vector[scope];
    }
}

/* generate offset to put actual args on stack in caller function
*   the offset will be + SP after moving stack pointer down totalArgSize*/
void nextArgElem(stack_env env, stack element, int offset_8, int offset_4, int offset_1, int totalArgSize, int savedReg){
    if (! element ) return;
    /*add element to appropriate bucket 8,4,1 */
    switch(element->kind){
        case(A64_Var):
        {
            /* arg scope will only be 0 */
            env->vector[0] = env->vector[0]->u.var.rest;
            switch(*element->u.var.offset){
                case(PTR):
                {
                    offset_8 += 8;
                    *element->u.var.offset =  (totalArgSize - offset_8)+ PTR + savedReg; //updating integer pointer
                    //env->vector[0] = element->u.var.rest;
                    nextArgElem(env, element->u.var.rest, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_8-=8; //remove offset from as leaving the scope
                }break;
                case(INT):
                {
                    offset_4 += 4;
                    *element->u.var.offset =  (totalArgSize - offset_4)+ INT + savedReg;//offset will always be negative below fp
                    //env->vector[0] = element->u.var.rest;
                    nextArgElem(env, element->u.var.rest, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_4-=4;
                }break;
                case(BOOL):
                {
                    offset_1 += 1;
                    *element->u.var.offset =  (totalArgSize - offset_1)+ BOOL + savedReg; //offset will always be negative below fp
                    //env->vector[0] = element->u.var.rest;
                    nextArgElem(env, element->u.var.rest, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_1-=1;
                }break;
            }
            free(element);
        }break;
        /*
        case(A64_endScope):
        {
            stack next = env->vector[element->u.endScope.nextScope];
            env->vector[0] = env->vector[0]->u.endScope.rest;
            nextElem(env, element->u.endScope.nextScope, next, offset_8, offset_4, offset_1);
            free(element);
        }break;
        */
        default: break;
    }

}

/* generates offsets for actual args in caller function
*   offset will be 0 after moving SP down totalArgSize 
*   offset will be offset + SP*/
int pushArgsOnStack(stack_env env, env_sizes mem_sizes){
    int totalArgSize = mem_sizes->size_1 + mem_sizes->size_4 + mem_sizes->size_8;
    int i = 0;
    while(totalArgSize%16){
        totalArgSize++;
        i++;
    }
    int offset_8 = 0,
    offset_4 = mem_sizes->size_8,
    offset_1 = offset_4 + mem_sizes->size_4;
    while(env->vector[0])//args are stored in scope0
        nextArgElem(env, env->vector[0], offset_8, offset_4, offset_1, totalArgSize, 0);

    free(mem_sizes);
   return totalArgSize;

}
/*  call this function after pushArgsOnstack 
*   call pushargsOnstack after formals or analyzed and after formals added to varEnv
*   reset memTotals after this function is called
*/
void generateArgStackMemory(stack_env env, int totalArgSize){
    int savedRegs = 4*8;
    //totalArgSize += savedRegs;
    int offset_8 = 0,
    offset_4 = env->mem_sizes->size_8,
    offset_1 = offset_4 + env->mem_sizes->size_4;
    while(env->vector[0])//args are stored in scope0
        nextArgElem(env, env->vector[0], offset_8, offset_4, offset_1, totalArgSize, savedRegs);
    free(env->mem_sizes);
}

/*follow the current scope and adjust the stack. while doing so update the integer pointers */
void nextElem(stack_env env, int index, stack element, int offset_8, int offset_4, int offset_1){
    if (! element ) return;
    /*add element to appropriate bucket 8,4,1 */
    switch(element->kind){
        case(A64_Var):
        {
            env->vector[index] = env->vector[index]->u.var.rest;
            switch(*element->u.var.offset){
                case(PTR):
                {
                    offset_8 += 8;
                    *element->u.var.offset =  0-offset_8; //updating integer pointer
                    /**
                     *  do i need this next statement:
                     * //env->vector[index] = element->u.var.rest;?????
                    */

                    //env->vector[index] = element->u.var.rest;
                    nextElem(env, index, element->u.var.rest, offset_8, offset_4, offset_1);
                    offset_8-=8; //remove offset from as leaving the scope
                }break;
                case(INT):
                {
                    offset_4 += 4;
                    *element->u.var.offset =  0-offset_4;//offset will always be negative below fp
                                     /**
                     *  do i need this next statement:
                     * //env->vector[index] = element->u.var.rest;?????
                    */
                    //env->vector[index] = element->u.var.rest;
                    nextElem(env, index, element->u.var.rest, offset_8, offset_4, offset_1);
                    offset_4-=4;
                }break;
                case(BOOL):
                {
                    offset_1 += 1;
                    *element->u.var.offset =  0-offset_1; //offset will always be negative below fp
                     /**
                     *  do i need this next statement:
                     * //env->vector[index] = element->u.var.rest;?????
                    */
                    //env->vector[index] = element->u.var.rest;
                    nextElem(env, index, element->u.var.rest, offset_8, offset_4, offset_1);
                    offset_1-=1;
                }break;
            }
            free(element);
        }break;
        case(A64_endScope):
        {
            stack next = env->vector[element->u.endScope.nextScope];
            env->vector[index] = env->vector[index]->u.endScope.rest;
            nextElem(env, element->u.endScope.nextScope, next, offset_8, offset_4, offset_1);
            free(element);
        }break;
        default: break;
    }

}

int generateStackMemory(stack_env env){
    /*calculate total memory to push on stack*/
    /*this needs to be 16 alignment */
    /*this will be a combo of local vars, saved regs, and LR, FP, SP, AccSP32, AccSP64*/
    int savedReg =  5*8; // 5 saved registers
    int localFunctionScope = env->mem_sizes->size_1 + env->mem_sizes->size_4 + env->mem_sizes->size_8;
    int localMemTotal = savedReg + localFunctionScope;
    int i = 0;
    while(localMemTotal%16){
        localMemTotal++;
        i++;
    }
    int offset_8 = 0,
        offset_4 = env->mem_sizes->size_8,
        offset_1 = offset_4 + env->mem_sizes->size_4;

    /*loop through each bucket and follow the chain until empty */
   for(int i = 1; i < env->size; i++){//start i == 1 to skip scope 0 which == args not local vars
    while(env->vector[i])
        nextElem(env, i, env->vector[i], offset_8, offset_4, offset_1);
   }
    free(env->mem_sizes);
   return localMemTotal;
}