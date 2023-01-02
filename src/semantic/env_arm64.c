/**
 * File by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include "env_arm64.h"
#include <stdlib.h>
#include "../codegen/MachineDependent.h"

/**
 * TODO:1 Make stack markers for begin scope and end scope SINGLETON INSTANCES.
 * Will need to remove free calls to A64_newScope begin marker and A64_endScope
 * end marker in nextElem function and nextArgElem.
 * 
 * TODO:2 Document/comment how the data structure controls the memory allocation for
 * stack frames when a function is analyzed in semantic.c and traverses multiple scopes.
 * This class optimizes memory usage between different size types and different scopes
 * to use as little memory for local vars per stack frame.
 */

struct stack_{
    enum{A64_Var, A64_newScope, A64_endScope}kind;
    stack next;
    union{
        struct{
            offset_ref offset;
        }var;
        struct{
            int nextScope;
        }newScope;
    }u;
};
struct stack_queue_
{
    stack first;
    stack last;
};
typedef struct stack_queue_ *stack_queue;
struct stack_env_
{
    int space_left;
    int size;
    stack_queue *vector;
    env_sizes mem_sizes;
};

stack_queue stack_env_get(stack_env env, int index );
void enter_scope_marker_toQueue( stack_env env, int scope, bool beginScope);
void free_stack(stack element);

stack_queue new_stack_queue(){
    stack_queue retval = (stack_queue)malloc(sizeof(struct stack_queue_));
    retval->first = NULL;
    retval->last = NULL;
    return retval;
}

stack_env new_stack_env(int init_size , bool functionStack)
{
    stack_env env = (stack_env)malloc(sizeof(*env));
    env->vector = (stack_queue *)malloc(init_size * sizeof(stack_queue *));//initialize array to size set to argument
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

void add_new_stack_elem(offset_ref data, stack_queue queue){
    stack retval = (stack)malloc(sizeof(*retval));
    retval->kind = A64_Var;
    ASN_OFFSET_REF(data, retval->u.var.offset) // assign offset and increase ref count
    retval->next = NULL;
    if (!queue->first) {
        queue->first = queue->last = retval;
    } else {
        queue->last->next = retval;
        queue->last = retval;
    }
    //return retval;
}


void enter_arm64( stack_env env, int index, offset_ref data ){
    if(env->size <= index){
        if(env->space_left == 0 ){//if array is full
            stack_queue *ptrTemp = NULL;
            int maxLength = env->size<<INCREMENT;//1;//double the size of array (maxLength x 2)
            ptrTemp = (stack_queue *)realloc(env->vector, maxLength * sizeof(stack_queue *));//have vptrTemp = realloc incase not succesful
            if( ptrTemp == NULL )
            {//exit it realloc failed
                printf("Failed realloc\n");
                exit(EXIT_FAILURE);
            }
            env->space_left = maxLength - env->size;//space left after growing
            env->vector = ptrTemp;//have env point to realloc array
        }

        /* add new initial queue to new scope size*/
        env->vector[env->size++] = new_stack_queue();//now that we have space push stack queue to next available spot
        env->space_left--;
    }
    if (!stack_env_get(env, index)) {
        env->vector[index] = new_stack_queue();
    }
    add_new_stack_elem(data, stack_env_get(env, index));
}

//free memory back to heap
void free_arm64_env(stack_env env)
{
    for (int i = 0; i<env->size; i++) {
        if (env->vector[i]) {
            stack temp = env->vector[i]->first;
            while (temp) {
                stack toFree = temp;
                temp = temp->next;
                free(toFree);
            }
            free(env->vector[i]);
        }
    }
    free(env->vector);
    free(env);
}

//return amount of elements in array
int stack_env_size(stack_env env)
{
    return env->size;
}

/* returns queue at index */
stack_queue stack_env_get(stack_env env, int index )
{
    if(index >= env->size || index < 0)
    {
        printf("Error: index %d out of bounds\n", index);
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


void addMemSizes(stack_env env, env_sizes mem){
    env->mem_sizes = mem;
}

void add_new_stack_begin(stack_queue queue, int scope){
    stack retval = (stack)malloc(sizeof(*retval));
    retval->kind = A64_newScope;
    retval->u.newScope.nextScope = scope+1;
    retval->next = NULL;
    if (!queue->first) {
        queue->first = queue->last = retval;
    } else {
        queue->last->next = retval;
        queue->last = retval;
    }
}

/* this needs to be called before incrementing to next scope */
void beginScope_Arm64(stack_env env, int scope) {
    if(scope){
        enter_scope_marker_toQueue(env, scope, true);
    }
}

void add_new_stack_end(stack_queue queue){
    stack retval = (stack)malloc(sizeof(*retval));
    retval->kind = A64_endScope;
    retval->next = NULL;
    if (!queue->first) {
        queue->first = queue->last = retval;
    } else {
        queue->last->next = retval;
        queue->last = retval;
    }
}

void enter_scope_marker_toQueue( stack_env env, int scope, bool beginScope) {
    if(env->size <= scope){
        if(env->space_left == 0 ){//if array is full
            stack_queue *ptrTemp = NULL;
            int maxLength = env->size<<INCREMENT;//1;//double the size of array (maxLength x 2)
            ptrTemp = (stack_queue *)realloc(env->vector, maxLength * sizeof(stack_queue *));//have vptrTemp = realloc incase not succesful
            if( ptrTemp == NULL )
            {//exit it realloc failed
                printf("Failed realloc\n");
                exit(EXIT_FAILURE);
            }
            env->space_left = maxLength - env->size;//space left after growing
            env->vector = ptrTemp;//have env point to realloc array
        }

        /* add new initial queue to new scope size*/
        env->vector[env->size++] = new_stack_queue();//now that we have space push stack queue to next available spot
        env->space_left--;
    }
    if (!stack_env_get(env, scope)) {
        env->vector[scope] = new_stack_queue();
    }
    if (beginScope) {
        add_new_stack_begin(stack_env_get(env, scope), scope);
    } else {
        add_new_stack_end(stack_env_get(env, scope));
    }
}

void arm64endScope(stack_env env, int scope){
    if(scope){
        enter_scope_marker_toQueue(env, scope, false);
    }
}

void free_stack(stack element) {
    if (element->kind == A64_Var) {
        OFFSET_REF_DEC(element->u.var.offset)
    }
    free(element);
}

/* generate offset to put actual args on stack in caller function
*   the offset will be + SP after moving stack pointer down totalArgSize*/
void nextArgElem(stack_env env, stack_queue queue, stack element, int offset_8, int offset_4, int offset_1, int totalArgSize, int savedReg){
    if (! element ) return;
    /*add element to appropriate bucket 8,4,1 */
    switch(element->kind){
        case(A64_Var):
        {
            /* arg scope will only be 0 */
            queue->first = element->next;
            switch(element->u.var.offset->offset){
                case(PTR):
                {
                    offset_8 += 8;
                    element->u.var.offset->offset =  (totalArgSize - offset_8)+ PTR + savedReg; //updating integer pointer
                    nextArgElem(env, queue, element->next, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_8-=8; //remove offset from as leaving the scope
                }break;
                case(INT):
                {
                    offset_4 += 4;
                    element->u.var.offset->offset =  (totalArgSize - offset_4)+ INT + savedReg;//offset will always be negative below fp
                    nextArgElem(env, queue, element->next, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_4-=4;
                }break;
                case(BOOL):
                {
                    offset_1 += 1;
                    element->u.var.offset->offset =  (totalArgSize - offset_1)+ BOOL + savedReg; //offset will always be negative below fp
                    nextArgElem(env, queue, element->next, offset_8, offset_4, offset_1, totalArgSize, savedReg);
                    offset_1-=1;
                }break;
            }
            free_stack(element);
        }break;
        default: break;
    }
}

void nextInstanceElem(stack_env env, stack_queue queue, stack element, int offset_8, int offset_4, int offset_1) {
    if (! element ) return;
    /*add element to appropriate bucket 8,4,1 */
    switch(element->kind){
        case(A64_Var):
        {
            queue->first = element->next;
            switch(element->u.var.offset->offset){
                case(PTR):
                {
                    element->u.var.offset->offset = offset_8; //offset for heap memory is positive
                    offset_8 += 8;
                    nextInstanceElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_8-=8; //remove offset from as leaving the scope
                }break;
                case(INT):
                {
                    element->u.var.offset->offset = offset_4; //offset for heap memory is positive
                    offset_4 += 4;
                    nextInstanceElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_4-=4;
                }break;
                case(BOOL):
                {
                    element->u.var.offset->offset = offset_1; //offset for heap memory is positive
                    offset_1 += 1;
                    nextInstanceElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_1-=1;
                }break;
            }
            free_stack(element);
        }
        default: break;
    }
}

void generateClassMemory(stack_env env, env_sizes mem_sizes) {
    int totalArgSize = mem_sizes->size_1 + mem_sizes->size_4 + mem_sizes->size_8;
    while(totalArgSize%8) totalArgSize++; //needs to have alignment on 8 bytes guaranteed in heap
    int offset_8 = 0,
    offset_4 = mem_sizes->size_8,
    offset_1 = offset_4 + mem_sizes->size_4;
    //while(env->vector[0])//args are stored in scope0
    if ( env->vector[0] ) {
        nextInstanceElem(env, env->vector[0], env->vector[0]->first, offset_8, offset_4, offset_1);
    }
    free(mem_sizes);
}

/* generates offsets for actual args in caller function
*   offset will be 0 after moving SP down totalArgSize 
*   offset will be offset + SP*/
int pushArgsOnStack(stack_env env, env_sizes mem_sizes){
    int totalArgSize = mem_sizes->size_1 + mem_sizes->size_4 + mem_sizes->size_8;
    while(totalArgSize%16) totalArgSize++;
    int offset_8 = 0,
    offset_4 = mem_sizes->size_8,
    offset_1 = offset_4 + mem_sizes->size_4;
    //while(env->vector[0])//args are stored in scope0
    if ( env->vector[0] ) {
        nextArgElem(env, env->vector[0], env->vector[0]->first, offset_8, offset_4, offset_1, totalArgSize, 0);
    }
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
    //while(env->vector[0])//args are stored in scope0
    if (env->vector[0]) {
        nextArgElem(env, env->vector[0], env->vector[0]->first, offset_8, offset_4, offset_1, totalArgSize, savedRegs);
    }
    free(env->mem_sizes);
}

/*follow the current scope and adjust the stack. while doing so update the integer pointers */
void nextElem(stack_env env, stack_queue queue, stack element, int offset_8, int offset_4, int offset_1){
    if (! element ) return;
    /*add element to appropriate bucket 8,4,1 */
    switch(element->kind){
        case(A64_Var):
        {
            queue->first = element->next;
            switch(element->u.var.offset->offset){
                case(PTR):
                {
                    offset_8 += 8;
                    element->u.var.offset->offset =  0-offset_8; //updating integer pointer
                    nextElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_8-=8; //remove offset from as leaving the scope
                }break;
                case(INT):
                {
                    offset_4 += 4;
                    element->u.var.offset->offset =  0-offset_4;//offset will always be negative below fp
                    nextElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_4-=4;
                }break;
                case(BOOL):
                {
                    offset_1 += 1;
                    element->u.var.offset->offset =  0-offset_1; //offset will always be negative below fp
                    nextElem(env, queue, element->next, offset_8, offset_4, offset_1);
                    offset_1-=1;
                }break;
            }
            free_stack(element);
        }break;
        case (A64_newScope):
        {
            
            nextElem(env, env->vector[element->u.newScope.nextScope], env->vector[element->u.newScope.nextScope]->first, offset_8, offset_4, offset_1);
            queue->first = element->next;
            nextElem(env, queue, element->next, offset_8, offset_4, offset_1);
            free_stack(element);
        }break;
        case(A64_endScope):
        {
            queue->first = element->next;
            free_stack(element);
            return;
        }break;
        default:
        {
            printf("Error in calculating local function variables stack memory offsets");
            break;
        }
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
    for (int i = 1; i < env->size; i++) {//start i == 1 to skip scope 0 which == args not local vars
        if (env->vector[i]) {
            while(env->vector[i]->first) {
                //printf("Entering scope %d. If success the next scope will not iterate frome here.", i);
                nextElem(env, env->vector[i], env->vector[i]->first, offset_8, offset_4, offset_1);
            }
        } else {
            printf("Stack queue for stack frame memory alignment null. No variables entered");
        }
    }
    free(env->mem_sizes);
   return localMemTotal;
}