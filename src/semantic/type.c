/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include "environment1.h"
#include "type.h"
#include "../codegen/MachineDependent.h"
type integertypeINST = NULL;
type booleantypeINST = NULL;
type voidtypeINST = NULL;


type IntegerType() {
  if (integertypeINST == NULL) {
    integertypeINST = (type) malloc(sizeof(struct type_));
    integertypeINST->kind = integer_type;
    integertypeINST->size_type = INT;
  }
  return integertypeINST;
}

type BooleanType() {
  if (booleantypeINST == NULL) {
    booleantypeINST = (type) malloc(sizeof(struct type_));
    booleantypeINST->kind = boolean_type;
    booleantypeINST->size_type = BOOL;
  }
  return booleantypeINST;
}

type VoidType() {
  if (voidtypeINST == NULL) {
    voidtypeINST = (type) malloc(sizeof(struct type_));
    voidtypeINST->kind = void_type;
  }
  return voidtypeINST;
}
type ClassType(environment instancevars) {
  type retval = (type) malloc(sizeof(struct type_));
  retval->kind=class_type;
  retval->size_type=PTR;
  retval->u.class.instancevars = instancevars;
  return retval;
}

type ArrayType(type basetype) {
  type retval = (type) malloc(sizeof(struct type_));
  retval->kind=array_type;
  retval->size_type=PTR;
  retval->u.array=basetype;
  return retval;
}
  
typeList TypeList(type first, typeList rest, int offset) {
  typeList retval = (typeList) malloc(sizeof(struct typeList_));
  retval->offset = (int*) malloc(sizeof(int));
  retval->first = first;
  retval->rest = rest;
  *retval->offset = offset;
  return retval;
}
