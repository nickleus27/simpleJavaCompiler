/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef AAT_H
#define AAT_H

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
};

struct AATstatement_ {
  enum {AAT_MOVE, AAT_JUMP, AAT_CONDITIONALJUMP, AAT_PROCEDURECALL, AAT_SEQ, AAT_EMPTY, AAT_LABEL, AAT_RETURN, AAT_HALT} kind; 
  union {
    struct {
      AATexpression lhs;
      AATexpression rhs;
      int size;
    } move; /* TODO move needs an int size_type to check for proper move instruction */
    Label jump;
    struct {
      AATexpression test;
      Label jump;
    } conditionalJump;
    struct {
      Label jump;
      AATexpressionList actuals;
    } procedureCall;
    struct {
      AATstatement left;
      AATstatement right;
    } sequential;
    Label label;
  } u;
};

struct AATexpression_ {
  enum {AAT_MEMORY, AAT_OPERATOR, AAT_FUNCTIONCALL, AAT_CONSTANT, AAT_REGISTER} kind;
  union {
    AATexpression memory; /* todo {AATexpression mem, int size_type}memory */
    int* constant; /* todo struct { int const, int size_type}constant */
    struct {
      AATexpression left;
      AATexpression right;
      AAToperator op;
    } operator;
    struct {
      Label jump;
      AATexpressionList actuals;
    } functionCall;
    Register reg; /* todo struct {int reg, int size_type}register */
  } u;
};

AATexpressionList AATExpressionList(AATexpression first, AATexpressionList rest);
AATstatement AATMove(AATexpression lhs, AATexpression rhs, int size);
AATstatement AATLabel(Label);
AATstatement AATJump(Label jump);
AATstatement AATConditionalJump(AATexpression test,Label jump);
AATstatement AATProcedureCall(Label jump, AATexpressionList actuals);
AATstatement AATSequential(AATstatement left, AATstatement right);
AATstatement AATEmpty(void);
AATstatement AATReturn(void);
AATstatement AATHalt(void);
AATexpression AATMemory(AATexpression mem);
AATexpression AATOperator(AATexpression left, AATexpression right, AAToperator op);
AATexpression AATFunctionCall(Label jump, AATexpressionList actuals);
AATexpression AATConstant(int* constant);
AATexpression AATRegister(Register reg);

/* statement stack functions */
void AATpush(AATstatement stm);
AATstatement AATpop(void);
void AATseqStmCleanUp(AATstatement seqEnd);
AATexpression _AATConstant(int constant);

#endif