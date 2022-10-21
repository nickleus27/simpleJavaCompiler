/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"
#include "../assembly/label.h"
#include "environment1.h"
#include "type.h"
#include "environment2.h"
#include "../codegen/MachineDependent.h"
/* Use a reasonable (PRIME!) hash table size */
#define HASHTABLESIZE 503

/* Local Type definitions */

typedef struct stackElem_ *stackElem;

struct stackElem_ {
  char *key;
  stackElem next;
};

struct environment_ {
  H_hashTable table;
  stackElem stack;
  struct {
    int size_8, size_4, size_1;
  }sizes;
  int scope;
};

/* Local Prototypes */

stackElem StackElem(char *key, stackElem next);
char *Marker();

/* Static variables */

char *stackmarker = NULL;
env_sizes memTotals = NULL;

/* Function Definitions */


char *Marker() {
  if (stackmarker != NULL) {
    stackmarker = (char *) malloc(6 * sizeof(char));
    strcpy(stackmarker,"0mark");
  }
  return stackmarker;
}

void enter(environment env, char * key, envEntry entry) {
  switch(entry->kind){
    case Var_Entry:
    {
      entry->u.varEntry.scope = env->scope;
      switch(entry->u.varEntry.typ->size_type){
        case PTR:
        {
          (env->sizes.size_8)+=entry->u.varEntry.typ->size_type;
        }break;
        case INT:
        {
          (env->sizes.size_4)+=entry->u.varEntry.typ->size_type;
        }break;
        case BOOL:
        {
          (env->sizes.size_1)+=entry->u.varEntry.typ->size_type;
        }break;
        default:
        break;
      }
    }break;
    default: break;
  }
  H_insert(env->table,key,(void *) entry);
  env->stack = StackElem(key,env->stack); 
}

envEntry find(environment env, char *key) {
  return (envEntry) H_find(env->table,key);
}

void beginScope(environment env) {
  env->stack = StackElem(Marker(),env->stack);
  env->scope++;
}

void updateMemTotals(environment env){
  if (env->sizes.size_8 > memTotals->size_8 )
    memTotals->size_8 = env->sizes.size_8;
  if (env->sizes.size_4 > memTotals->size_4)
     memTotals->size_4 = env->sizes.size_4;
  if (env->sizes.size_1 > memTotals->size_1)
     memTotals->size_1 = env->sizes.size_1;
}

int endScope(environment env) {
  updateMemTotals(env);
  stackElem temp;
  while(env->stack && (env->stack->key != Marker())) {
    envEntry var = find(env, env->stack->key);
    switch(var->kind){
      case Var_Entry:
      {
        switch(var->u.varEntry.typ->size_type){
          case PTR:
          {
            (env->sizes.size_8) -= var->u.varEntry.typ->size_type;
          }break;
          case INT:
          {
            (env->sizes.size_4) -= var->u.varEntry.typ->size_type;
          }break;
          case BOOL:
          {
            (env->sizes.size_1) -= var->u.varEntry.typ->size_type;
          }break;
          default:
          break;
        }
      }break;
      default: break;
    }
    H_delete(env->table,env->stack->key);
    temp = env->stack;
    env->stack = env->stack->next;
    free(temp);
  }
  /* Remove Stack Marker */
  if (env->stack){
    temp = env->stack;
    env->stack = env->stack->next;
    free(temp);
  }
  return --env->scope;
}

void AddBuiltinTypes(environment env) {
  enter(env,"int",TypeEntry(IntegerType()));
  enter(env,"boolean",TypeEntry(BooleanType()));
  enter(env,"void",TypeEntry(VoidType()));
}

void AddBuiltinFunctions(environment env) {
  typeList formals = NULL;
  //enter(env, "Read", FunctionEntry(IntegerType(),NULL,"Read","Readend"));
  enter(env, "Println", FunctionEntry(VoidType(),NULL,"Println","Printlnend"));
  //enter(env, "Print", FunctionEntry(VoidType(), TypeList(IntegerType(), NULL),
	//			    "Print","Printend"));
}

void initMemTrackers(){
  memTotals = (env_sizes)malloc(sizeof(struct env_sizes_));
  memTotals->size_1=0;
  memTotals->size_4=0;
  memTotals->size_8=0;
}

environment Environment() {
  environment retval = (environment) malloc(sizeof(struct environment_));
  retval->table = H_HashTable(HASHTABLESIZE);
  retval->sizes.size_8 = retval->sizes.size_4 = retval->sizes.size_1 = 0;
  retval->stack = NULL;
  retval->scope = 0;
  return retval;
}


envEntry VarEntry(type typ, int offset) {
  envEntry retval = (envEntry) malloc(sizeof(struct envEntry_));
  /*TODO: MEMORY needs to be FREED*/
  retval->u.varEntry.offset  = (int*)malloc(sizeof(int));
  retval->kind = Var_Entry;
  retval->u.varEntry.typ = typ;
  *retval->u.varEntry.offset = offset;
  return retval;
}

envEntry FunctionEntry(type returntyp, typeList formals, Label startLabel, Label endLabel) {
  envEntry retval = (envEntry) malloc(sizeof(struct envEntry_));
  retval->kind = Function_Entry;
  retval->u.functionEntry.returntyp = returntyp;
  retval->u.functionEntry.formals = formals;
  retval->u.functionEntry.startLabel = startLabel;
  retval->u.functionEntry.endLabel = endLabel;
  return retval;
}

void setArgMemSize(envEntry functionEntry, int size){
  functionEntry->u.functionEntry.argMemSize = size;
}

envEntry TypeEntry(type typ) {
  envEntry retval = (envEntry) malloc(sizeof(struct envEntry_));
  retval->kind = Type_Entry;
  retval->u.typeEntry.typ = typ;
  return retval;
}

stackElem StackElem(char *key, stackElem next) {
  stackElem retval = (stackElem) malloc(sizeof(struct stackElem_));
  retval->key = key;
  retval->next = next;
  return retval;
}


env_sizes getMemTotals(){
  env_sizes retval = (env_sizes)malloc(sizeof(struct env_sizes_));
  retval->size_1 = memTotals->size_1;
  retval->size_4 = memTotals->size_4;
  retval->size_8 = memTotals->size_8;
  return retval;
}

env_sizes getEnvMemTotals(environment env){
  env_sizes retval = (env_sizes)malloc(sizeof(struct env_sizes_));
  retval->size_1 = env->sizes.size_1;
  retval->size_4 = env->sizes.size_4;
  retval->size_8 = env->sizes.size_8;
  return retval;
}

int getScope(environment env){
  return env->scope;
 }

int envSize(environment env){
  int total = env->sizes.size_1 + env->sizes.size_4 + env->sizes.size_8;
  int i = 0;
  while(total % 16){
    total++;
    i++;
  }
  return total;
}

void resetMemTotals(){
  memTotals->size_1=0;
  memTotals->size_4=0;
  memTotals->size_8=0;
}