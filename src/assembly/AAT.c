/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdlib.h>
#include "../codegen/registerArm64.h"
#include "label.h"
#include "AAT.h"

/* type definitions */
typedef struct AATstmStack_ *AATstmStack;

struct AATstmStack_ {
  AATstatement stm;
  AATstmStack next;
};

/* global variables */
AATstmStack stack = NULL;

/* function definitions */
AATstmStack AATstmStackElem(AATstatement stm, AATstmStack head){
  AATstmStack retval = (AATstmStack) malloc(sizeof(struct AATstmStack_));
  retval->stm = stm;
  retval->next = head;
  return retval;
}

void AATpush(AATstatement stm){
  stack = AATstmStackElem(stm, stack);
}

AATstatement AATpop(){
  AATstmStack temp = stack;
  AATstatement retval = stack->stm;
  stack = stack->next;
  free(temp);
  return retval;
}

/* this function removes SequentialStatement what has 2 null ptrs *
** when visitStatementList returns to analyzeFunction             *
** the SequentialStatement is replaced with an empty statement    */
void AATseqStmCleanUp(AATstatement seqEnd){
  seqEnd->u.sequential.left = AATEmpty();
  seqEnd->u.sequential.right = AATEmpty();
  /*other option would be:

  free(seqEnd);

  and change AATprint and AATtraversals to check for NULL */
}

AATexpressionList AATExpressionList(AATexpression first, AATexpressionList rest, int size_type, int offset) {
  AATexpressionList retval = (AATexpressionList) malloc(sizeof(struct AATexpressionList_));
  retval->first = first;
  retval->rest = rest;
  retval->size_type = size_type;
  retval->offset = offset;
  return retval;
}

AATstatement AATMove(AATexpression lhs, AATexpression rhs, int size) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_MOVE;
  retval->u.move.lhs = lhs;
  retval->u.move.rhs = rhs;
  retval->u.move.size = size;
  return retval;
}

AATstatement AATLabel(Label label) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_LABEL;
  retval->u.label = label;
  return retval;
}
AATstatement AATJump(Label jump) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_JUMP;
  retval->u.jump = jump;
  return retval;
}

AATstatement AATConditionalJump(AATexpression test,Label jump) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_CONDITIONALJUMP;
  retval->u.conditionalJump.jump = jump;
  retval->u.conditionalJump.test = test;
  return retval;
}

AATstatement AATProcedureCall(Label jump, AATexpressionList actuals, int argMemSize) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_PROCEDURECALL;
  retval->u.procedureCall.jump = jump;
  retval->u.procedureCall.actuals = actuals;
  retval->u.procedureCall.argMemSize = argMemSize;
  return retval;
}
AATstatement AATSequential(AATstatement left, AATstatement right) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_SEQ;
  retval->u.sequential.left = left;
  retval->u.sequential.right = right;
  return retval;
}
AATstatement AATFunctionDef(AATstatement labels, AATstatement body, int framesize){
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_FUNCDEF;
  retval->u.functionDef.labels = labels;
  retval->u.functionDef.body = body;
  retval->u.functionDef.framesize = framesize;
  return retval;
}
AATstatement AATEmpty(void) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_EMPTY;
  return retval;
}
AATstatement AATReturn(void) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_RETURN;
  return retval;
}

AATstatement AATHalt(void) {
  AATstatement retval = (AATstatement) malloc(sizeof(struct AATstatement_));
  retval->kind = AAT_HALT;
  return retval;
}

AATexpression AATMemory(AATexpression memory, int size_type){
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_MEMORY;
  retval->size_type = size_type;
  retval->u.memory = memory;
  return retval;
}

AATexpression AATOperator(AATexpression left, AATexpression right, AAToperator op, int size_type) {
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_OPERATOR;
  retval->size_type = size_type;
  retval->u.operator.left = left;
  retval->u.operator.right = right;
  retval->u.operator.op = op;
  return retval;
}
AATexpression AATFunctionCall(Label jump, AATexpressionList actuals, int size_type, int argMemSize) {
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_FUNCTIONCALL;
  retval->size_type = size_type;
  retval->u.functionCall.jump = jump;
  retval->u.functionCall.actuals = actuals;
  retval->u.functionCall.argMemSize = argMemSize;
  return retval;
}
AATexpression AATConstant(int constant, int size_type){
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_CONSTANT;
  retval->size_type = size_type;
  retval->u.constant = constant;
  return retval;
}
AATexpression AATOffset(int* constant, int size_type) {
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_OFFSET;
  retval->size_type = size_type;
  retval->u.offset = constant;
  return retval;
}
AATexpression AATRegister(Register reg, int size_type){
  AATexpression retval = (AATexpression) malloc(sizeof(struct AATexpression_));
  retval->kind = AAT_REGISTER;
  retval->size_type = size_type;
  retval->u.reg = reg;
  return retval;
}
