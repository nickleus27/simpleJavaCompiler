/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef AAT_H
#define AAT_H
#include "../lib/offset_ref.h"

typedef struct AATexpression_ *AATexpression;
typedef struct AATstatement_ *AATstatement;
typedef struct AATexpressionList_ *AATexpressionList;
typedef struct AATstmStack_ *AATstmStack;

typedef enum {AAT_PLUS, AAT_MINUS, AAT_MULTIPLY, AAT_DIVIDE, AAT_LT,
	      AAT_GT, AAT_LEQ, AAT_GEQ, AAT_EQ, AAT_NEQ, AAT_AND, AAT_OR,
              AAT_NOT} AAToperator;


struct AATexpressionList_ {
  AATexpression first;
  AATexpressionList rest;
  int size_type;
  int offset;
};

struct AATstatement_ {
  enum {AAT_MOVE, AAT_JUMP, AAT_CONDITIONALJUMP, AAT_PROCEDURECALL, AAT_SEQ, AAT_EMPTY,
   AAT_LABEL, AAT_RETURN, AAT_HALT, AAT_FUNCDEF} kind;
   
  union {
    struct {
      AATexpression lhs;
      AATexpression rhs;
      int size;
    } move;
    label_ref jump;
    struct {
      AATexpression test;
      label_ref jump;
    } conditionalJump;
    struct {
      label_ref jump;
      AATexpressionList actuals;
      int argMemSize;
    } procedureCall;
    struct {
      AATstatement left;
      AATstatement right;
    } sequential;
    struct{
      AATstatement labels; //sequential containing labels. left is start, and right is end
      AATstatement body;
      int framesize;
    }functionDef;
    label_ref label;
  } u;
};

struct AATexpression_ {
  enum {AAT_MEMORY, AAT_OPERATOR, AAT_FUNCTIONCALL, AAT_CONSTANT, AAT_OFFSET, AAT_REGISTER} kind;
  int size_type;
  union {
    int constant;
    offset_ref offset;
    Register reg;
    AATexpression memory;
    struct {
      AATexpression left;
      AATexpression right;
      AAToperator op;
    } operator;
    struct {
      label_ref jump;
      AATexpressionList actuals;
      int argMemSize;
    } functionCall;
  } u;
};

/*statements*/
AATexpressionList AATExpressionList(AATexpression first, AATexpressionList rest, int size_type, int offset);
AATstatement AATMove(AATexpression lhs, AATexpression rhs, int size);
AATstatement AATLabel(label_ref);
AATstatement AATJump(label_ref jump);
AATstatement AATConditionalJump(AATexpression test, label_ref jump);
AATstatement AATProcedureCall(label_ref jump, AATexpressionList actuals, int argMemSize);
AATstatement AATSequential(AATstatement left, AATstatement right);
AATstatement AATEmpty(void);
AATstatement AATReturn(void);
AATstatement AATHalt(void);
AATstatement AATFunctionDef(AATstatement labels, AATstatement body, int framesize);

/*expressions*/
AATexpression AATConstant(int constant, int size_type);
AATexpression AATOffset(offset_ref offset, int size_type);
AATexpression AATRegister(Register reg, int size_type);
AATexpression AATMemory(AATexpression mem, int size_type);
AATexpression AATOperator(AATexpression left, AATexpression right, AAToperator op, int size_type);
AATexpression AATFunctionCall(label_ref jump, AATexpressionList actuals, int size_type, int argMemSize);

/* statement stack functions */
void AATpush(AATstatement stm);
AATstatement AATpop(void);
void AATseqStmCleanUp(AATstatement seqEnd);

#endif