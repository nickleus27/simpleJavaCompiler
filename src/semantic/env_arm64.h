/* environment for ARM64 offset arrangement on the stack */
/**
 * File by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef env_arm64_h
#define env_arm64_h
#define INCREMENT 1

#include <stdio.h>
#include <stdbool.h>
#include "environment1.h"
typedef struct stack_ *stack;
typedef struct stack_env_ *stack_env;

stack_env new_stack_env(int init_size );
void enter_arm64( stack_env env, int index, int* data );
void free_arm64_env(stack_env env);
//int stack_env_size(stack_env env);
stack stack_env_get(stack_env env, int index );
void stack_env_clear(stack_env env, int size);
void addMemSizes(stack_env env, env_sizes mem);//, env_sizes block);
void arm64endScope(stack_env env, int scope);
int generateStackMemory(stack_env env);

#endif /* int_vector_h */