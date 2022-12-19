/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include "../codegen/registerArm64.h"
#include "label.h"
#include "AAT.h"
#include "AATPrintTree.h"
#include <stdio.h>

#define INITIAL_INDENT 3

char *OperatorNames[] = {"+","-","*","/","<",">","<=", ">=", "==", "!=", "&&", "||", "!"}; 

int AATindentstep = INITIAL_INDENT;
int AATindent = 0;

void printAATexp(AATexpression exp);
void printExpList(AATexpressionList expList);
void printAATOperator(int op);

void printAATindent(int AATindent) {
  int i;
  for (i=0; i<AATindent*AATindentstep; i++) {
    printf(" ");
  }
  fflush(stdout);
}

void printAAT(AATstatement body) {
  switch(body->kind) {
  case AAT_MOVE:
    printAATindent(AATindent);
    printf("MOVE (dest/source)\n");
    AATindent++;
    printAATexp(body->u.move.lhs);
    printAATexp(body->u.move.rhs);
    AATindent--;
    break;
  case AAT_JUMP:
    printAATindent(AATindent);
    printf("JUMP: %s\n",body->u.jump);
    break;
  case AAT_CONDITIONALJUMP:
    printAATindent(AATindent);
    printf("COND JUMP: %s\n",body->u.conditionalJump.jump);
    AATindent++;
    printAATexp(body->u.conditionalJump.test);
    AATindent--;
    break;
  case AAT_FUNCDEF:
    printf("Function Definition:\n");
    printAAT(body->u.functionDef.labels->u.sequential.left);
    AATindent++;
    printAATindent(AATindent);
    printf("Stack frame size: %d\n", body->u.functionDef.framesize);
    printAATindent(AATindent);
    printf("PUSH ON STACK: LR, FP, SP, and SAVED REGS \n");
    printAAT(body->u.functionDef.body);
    AATindent--;
    printAAT(body->u.functionDef.labels->u.sequential.right);
    AATindent++;
    printAATindent(AATindent);
    printf("POP STACK.\n");
    AATindent--;
    break;
  case AAT_PROCEDURECALL:
    printAATindent(AATindent);
    printf("PROCEDURE CALL: %s\n",body->u.procedureCall.jump);
    AATindent++;
    printExpList(body->u.procedureCall.actuals);
    AATindent--;
    break;
  case AAT_SEQ:
    printAAT(body->u.sequential.left);
    printAAT(body->u.sequential.right);
    break;
  case AAT_EMPTY:
    printAATindent(AATindent);
    printf("EMPTY \n");
    break;
  case AAT_LABEL:
    printAATindent(AATindent);
    printf("LABEL: %s\n",body->u.label);
    break;

  case AAT_RETURN:
    printAATindent(AATindent);
    printf("RETURN \n");
    break;
  case AAT_HALT:
    printAATindent(AATindent);
    printf("HALT \n");
    break;
  }
}

void printAATexp(AATexpression exp) {

  switch(exp->kind) {
  case AAT_MEMORY:
    printAATindent(AATindent);
    printf("MEMORY \n");
    AATindent++;
    printAATexp(exp->u.memory);
    AATindent--;
    break;
  case AAT_OPERATOR:
    if (exp->u.operator.op == AAT_NOT) {
      printAATindent(AATindent);
      printf("NOT");
      AATindent++;
      printAATexp(exp->u.operator.left);
      AATindent--;
    } else {
      printAATOperator(exp->u.operator.op);
      AATindent++;
      printAATexp(exp->u.operator.left);
      printAATexp(exp->u.operator.right);
      AATindent--;
    }
    break;
  case AAT_FUNCTIONCALL:
    printAATindent(AATindent);
    printf("CALL: %s\n", exp->u.functionCall.jump);
    AATindent++;
    printExpList(exp->u.functionCall.actuals);
    AATindent--;
    break;
  case AAT_CONSTANT:
    printAATindent(AATindent);
    printf("CONSTANT: %d \n", exp->u.constant);
    break;
  case AAT_OFFSET:
    printAATindent(AATindent);
    printf("OFFSET: %d \n", exp->u.offset->offset);
    break;
  case AAT_REGISTER:
    printAATindent(AATindent);
    printf("REGISTER: %s \n",exp->u.reg);
    break;
  }

}


void printExpList(AATexpressionList expList) {
  if (expList != NULL) {
    printAATexp(expList->first);
    printExpList(expList->rest);
  }

}



void setAATAATindent(int newAATindent) {
  AATindentstep = newAATindent;
}

void printAATOperator(int op) {
  printAATindent(AATindent);
  printf("%s \n", OperatorNames[op]);
}
