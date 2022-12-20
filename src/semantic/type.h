/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/
#include "../lib/offset_ref.h"

typedef struct type_ *type;
typedef struct typeList_ *typeList;

struct type_ {
  enum {integer_type, boolean_type, void_type,
	class_type, array_type} kind;
  int size_type;
  union {
    type array;
    struct {
      environment instancevars;
    } class;
  } u;
};

struct typeList_ {
  offset_ref offset;
  type first;
  typeList rest;
};

type IntegerType();
type BooleanType();
type VoidType();
type ClassType(environment instancevars);
type ArrayType(type basetype);
typeList TypeList(type first, typeList rest, int offset);


