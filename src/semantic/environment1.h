/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef ENV_H1
#define ENV_H1
#include <stdbool.h>
typedef struct environment_ *environment;
typedef struct envEntry_ *envEntry;
typedef struct env_sizes_ *env_sizes;

struct env_sizes_{
  int size_8, size_4, size_1;
};

environment Environment();

void AddBuiltinTypes(environment env);
void AddBuiltinFunctions(environment env);
void initMemTrackers();

void beginScope(environment env);
int endScope(environment env);
/* this function does not decrement env->size */
void endBlockScope(environment env);

void enter(environment env, char * key, envEntry entry);
envEntry find(environment env, char *key);

int envSize(environment env);

/* functions to keep track of total size needed for a stack frame include block statements */
/*
void resetStackFrameSize(void);
void addBlockScopeSize(int size);
int getStackFrameSize(void);
*/

int getScope(environment env);
env_sizes getMemTotals();
/* call this after function analysis ends */
void resetMemTotals();
//env_sizes getBlockTotals();

#endif