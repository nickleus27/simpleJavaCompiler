/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "../assembly/label.h"
#include "registerArm64.h"
#include "MachineDependent.h"
#include "../assembly/AAT.h"

#define SWITCH_BYTE BYTE/4
#define SWITCH_REG32 REG32/4
#define SWITCH_REG64 REG64/4

FILE *outfile;
/* statement generation */
void generateStatement(AATstatement tree);
void generateMove(AATstatement tree);

/* expression generation */
void generateExpression(AATexpression tree);
void generateOpExp32(AATexpression tree);
void generateOpExp64(AATexpression tree);
void generateMemoryExpression(AATexpression tree);
void generateConstantExp(AATexpression tree);
void generateRegisterExp(AATexpression tree);
void generateOpExp(AATexpression tree);
void generateFunCall(AATexpression tree);

void emit(char *assem,...);
void emitLibrary(void);
void emitSetupCode(void);


void generateCode(AATstatement tree, FILE *output) {
  outfile = output; 
  emitSetupCode();
  generateStatement(tree);
  fprintf(output,"\n");
  //emitLibrary();
}

void emit(char *assem,...) {
  va_list ap;
  if (assem[strlen(assem)-1] != ':') 
    fprintf(outfile,"\t");
  va_start(ap,assem);
  vfprintf(outfile, assem, ap);
  va_end(ap);
  fprintf(outfile,"\n");
}

void generateExpression(AATexpression tree){
  switch (tree->kind) {
    case AAT_MEMORY:
      generateMemoryExpression(tree);
      break;
    case AAT_CONSTANT:
      generateConstantExp(tree);
      break;
    case AAT_REGISTER:
      generateRegisterExp(tree);
      break;
    case AAT_OPERATOR:
      generateExpression(tree->u.operator.left);
      generateExpression(tree->u.operator.right);
      generateOpExp(tree);
    break;
    case AAT_FUNCTIONCALL:
      generateFunCall(tree);
      break;
  }
}
void generateConstantExp(AATexpression tree){
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("mov %s, #%d", Acc32(), *tree->u.constant);
      emit("str %s, [%s]", Acc32(), AccSP32());//use str (not strb) because placing in 4b (1word) space on stack
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG32:
      emit("mov %s, #%d", Acc32(), *tree->u.constant);
      emit("str %s, [%s]", Acc32(), AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG64:
      emit("mov %s, #%d", Acc64(), *tree->u.constant);
      emit("str %s, [%s]", Acc64(), AccSP64());
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
    break;
    default:
    break;
  }
}
void generateRegisterExp(AATexpression tree){
    switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("strb %s, [%s]", tree->u.reg, AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG32:
      emit("str %s, [%s]", tree->u.reg, AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG64:
      /* need to check for SP reg... move first*/
      if( tree->u.reg != SP() ){
        emit("str %s, [%s]", tree->u.reg, AccSP64());
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
      }else{
        emit("mov %s, %s", Acc64(), SP());
        emit("str %s, [%s]", Acc64(), AccSP64());
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
      }
    break;
    default:
    break;
  }
}
void generateOpExp(AATexpression tree){
    switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      /*implement for char type*/
      /**
       * TOODO: Is it correct to always send bool and char to OpExp32?
       */
      generateOpExp32(tree);
    break;
    case SWITCH_REG32:
      generateOpExp32(tree);
    break;
    case SWITCH_REG64:
      generateOpExp64(tree);
    break;
    default:
      emit("/*BAD OPERATOR EXPRESSION*/\n");
    break;
  }
}
void generateFunCall(AATexpression tree){
    switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
    break;
    case SWITCH_REG32:
    break;
    case SWITCH_REG64:
    break;
    default:
    break;
  }
}

void generateOpExp64(AATexpression tree){
  switch (tree->u.operator.op){
    case AAT_PLUS:
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
      emit("add %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      break;
    case AAT_MINUS:
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
      emit("sub %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      break;
    case AAT_MULTIPLY:
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
      emit("mul %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      break;
    case AAT_DIVIDE:
    /**
     * TODO: Need to add code to throw error when dividing by ZERO
     */
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
      emit("sdiv %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      break;
    case AAT_LT:
      
      break;
    case AAT_GT:

      break;
    case AAT_LEQ:

      break;
    case AAT_GEQ:

      break;
    case AAT_EQ:

      break;
    case AAT_NEQ:

      break;
    case AAT_AND:

      break;
    case AAT_OR:

      break;
    case AAT_NOT:

      break;
    default: emit("Bad 64bit Operator Expression");
  }
}

void generateOpExp32(AATexpression tree){
  switch (tree->u.operator.op){
    case AAT_PLUS:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("add %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_MINUS:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("sub %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_MULTIPLY:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("mul %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_DIVIDE:
      /**
     * TODO: Need to add code to throw error when dividing by ZERO
     */
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("sdiv %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;

    /**
     * TODO: Do I need to change STRB TO STR in boolean expressions
     * When pushing value on 32bit (4byte/AccSP32) stack??
     * go through change and test!
     */
    case AAT_LT:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, lt", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_GT:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, gt", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_LEQ:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, le", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_GEQ:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, ge", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_EQ:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, eq", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_NEQ:
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("cmp %s, %s", Tmp0_32(), Tmp1_32());
      emit("cset %s, ne", Acc32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_AND:
      emit("ldrb %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldrb %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("and %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_OR:
      emit("ldrb %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldrb %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      emit("orr %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32());
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
      break;
    case AAT_NOT:
      emit("ldrb %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD); // Tmp0 == #2
      emit("ldrb %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD); // Tmp1 == boolean value
      emit("mvn %s, %s", Tmp1_32(), Tmp1_32()); // move NOT boolean value
      emit("add %s, %s, %s", Acc32(), Tmp0_32(), Tmp1_32()); // add #2 to inverted value to get 0 | 1
      emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
    break;
    default: emit("Bad 64bit Operator Expression");
  }
}
void addActualsToStack(AATexpressionList actual){
  if (!actual)
    return;
  addActualsToStack(actual->rest);
  switch(actual->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case(SWITCH_BYTE):
    {
      /**
       * TODO: ADD CODE
       */
    }break;
    case(SWITCH_REG32):
    {
      generateExpression(actual->first);
      emit("ldr %s, [%s, #%d]!", Acc32(), AccSP32(), HALFWORD);
      emit("str %s, [%s, #%d]", Acc32(), SP(), actual->offset);
    }break;
    case(SWITCH_REG64):
    {
      generateExpression(actual->first);
      emit("ldr %s, [%s, #%d]!", Acc64(), AccSP64(), WORD);
      emit("str %s, [%s, #%d]", Acc64(), SP(), actual->offset);
    }break;
    default: emit("Bad expression size type\n");
  }
}
void generateStatement(AATstatement tree) {
  switch (tree->kind) {
    case AAT_MOVE:
        generateMove(tree);
        break;
    case AAT_JUMP:
        emit("b %s", tree->u.jump);
        break;
    case AAT_CONDITIONALJUMP:
        generateExpression(tree->u.conditionalJump.test);
        emit("ldrb %s, [%s, #%d]!", Acc32(), AccSP32(), HALFWORD);
        emit("cmp %s, #1", Acc32());
        emit("b.eq %s", tree->u.conditionalJump.jump);
        break;
    case AAT_FUNCDEF:
    {
      int localVarSize = tree->u.functionDef.framesize - 4*8; /* saving 5 registers so 5-1=4, SP starts om 0 offset*/
      generateStatement(tree->u.functionDef.labels->u.sequential.left);
      /* assembly code fore saving registers and adjusting pointers*/
      emit("mov %s, %s", Acc64(), SP());
      emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.functionDef.framesize);
      emit("str %s, [%s, #%d]", FP(), SP(), localVarSize);
      emit("add %s, %s, #%d", FP(), SP(), localVarSize);
      emit("str %s, [%s, #%d]", ReturnAddr(), FP(), WORD);
      emit("str %s, [%s, #%d]", AccSP32(), FP(), WORD*2);
      emit("str %s, [%s, #%d]", AccSP64(), FP(), WORD*3);
      generateStatement(tree->u.functionDef.body);
      generateStatement(tree->u.functionDef.labels->u.sequential.right);
      emit("ldr %s, [%s, #%d]", AccSP64(), FP(), WORD*3);
      emit("ldr %s, [%s, #%d]", AccSP32(), FP(), WORD*2);
      emit("ldr %s, [%s, #%d]", ReturnAddr(), FP(), WORD);
      emit("ldr %s, [%s]", FP(), FP());
      emit("ldr %s, [%s]", Acc64(), SP());
      emit("mov %s, %s", SP(), Acc64());
      emit("ret");
    }
    break;
    case AAT_PROCEDURECALL:
      /**
       * adjust stack pointer for argMemSize
       * walk actual/formal list and add to stack according to offsets
       * bl startLabel
       * move sp back up the size of argMemSize
       */

      emit("mov %s, %s", Acc64(), SP());
      emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.procedureCall.argMemSize);
      addActualsToStack(tree->u.procedureCall.actuals);
      emit("bl %s", tree->u.procedureCall.jump);
      //emit("add %s, %s, #%d", SP(), SP(), tree->u.procedureCall.argMemSize);
      emit("str %s, [%s]", Acc64(), SP());
      emit("mov %s, %s", SP(), Acc64());
      break;
    case AAT_SEQ:
      generateStatement(tree->u.sequential.left);
      generateStatement(tree->u.sequential.right);
      break;
    case AAT_EMPTY:
        break;
    case AAT_LABEL:
      emit("%s:", tree->u.label);
        break;
    case AAT_RETURN:
      emit("ret");
        break;
    case AAT_HALT:

        break;
  }
}

void generateMove(AATstatement tree) {
  /* Generates inefficient assembly */
  if (tree->u.move.lhs->kind == AAT_REGISTER) {
    if(tree->u.move.size == REG32){
      generateExpression(tree->u.move.rhs);
      emit("ldr %s, [%s, #%d]!", tree->u.move.lhs->u.reg, AccSP32(), HALFWORD);
    }else if(tree->u.move.size == REG64 ){
      generateExpression(tree->u.move.rhs);
      /* need to check for SP reg... move first*/
      if( tree->u.move.lhs->u.reg != SP() ){
        emit("ldr %s, [%s, #%d]!", tree->u.move.lhs->u.reg, AccSP64(), WORD);
      }else{
        emit("ldr %s, [%s, #%d]!", Acc64(), AccSP64(), WORD);
        emit("mov %s, %s", SP(), Acc64());
      }
    }else if(tree->u.move.size == BYTE){
      generateExpression(tree->u.move.rhs);
      emit("ldrb %s, [%s, #%d]!", tree->u.move.lhs->u.reg, AccSP32(), HALFWORD);
    }
  } else if (tree->u.move.lhs->kind == AAT_MEMORY) {
    if(tree->u.move.size == REG32){
      generateExpression(tree->u.move.lhs->u.memory);//bypass dereference value @ mem addresss & just get address
      generateExpression(tree->u.move.rhs);
      emit("ldr %s, [%s, #%d]!", Acc64(), AccSP64(), WORD);//store address in a reg
      emit("ldr %s, [%s, #%d]!", Tmp0_32(), AccSP32(), HALFWORD);//store value in reg (32bit)
      emit("str %s, [%s]", Tmp0_32(), Acc64()); //implement move
    }else if (tree->u.move.size == REG64 ){
      generateExpression(tree->u.move.lhs->u.memory);
      generateExpression(tree->u.move.rhs);
      emit("//implement move");
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);//store address in a reg
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);//store value in reg (64bit)
      emit("str %s, [%s]", Tmp0_64(), Acc64()); //implement move
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2); //update exp stack pointer
    }else if (tree->u.move.size == BYTE ){
      generateExpression(tree->u.move.lhs->u.memory);//bypass dereference value @ mem addresss & just get address
      generateExpression(tree->u.move.rhs);
      emit("ldr %s, [%s, #%d]!", Acc64(), AccSP64(), WORD);//store address in a reg
      emit("ldrb %s, [%s, #%d]!", Tmp0_32(), AccSP32(), HALFWORD);//store value in reg (BYTE)
      emit("strb %s, [%s]", Tmp0_32(), Acc64()); //implement move
    }
  } else {
    fprintf(stderr,"Bad MOVE node -- LHS should be T_mem or T_register\n");
  }
}

void generateMemoryExpression(AATexpression tree) {
  /* generates inefficent code */
  generateExpression(tree->u.memory);
  emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD); //load address
  if(tree->size_type == REG32){
    emit("ldr %s, [%s]", Acc32(), Acc64()); //dereference address to get value
    emit("str %s, [%s], #%d", Acc32(), AccSP32(), 0-HALFWORD); //store value on reg32 Stack postfix adjust pointer
    emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD); //move up reg64 stack pointer
  }else if(tree->size_type == REG64){
    emit("ldr %s, [%s]", Acc64(), Acc64());
    emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD);
  }else if (tree->size_type == BYTE){
    emit("ldrb %s, [%s]", Acc32(), Acc64()); //dereference address to get value
    emit("str %s, [%s], #%d", Acc32(), AccSP32(), 0-HALFWORD); //use str because of 4byte space on stack
    emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD); //move up reg64 stack pointer
  }
}

void emitSetupCode(void) {
  emit(".globl _start");
  emit(".align 4");
  emit(".text");
  emit("_start:");
  emit("mov %s, #0", FP());
  emit("add %s, %s, #-%d", SP(), SP(), EXPR_STACKSIZE);
  emit("mov %s, %s", Tmp0_64(), SP());
  emit("str %s, [%s]", Tmp0_64(), SP());
  emit("add %s, %s, #%d", AccSP64(), Tmp0_64(), 4072);
  emit("str %s, [%s, #%d]", FP(), AccSP64(), WORD);
  emit("str %s, [%s, #%d]", ReturnAddr(), AccSP64(), 2*WORD);
  emit("add %s, %s, %#-d", AccSP32(), AccSP64(), 2036);
  emit("bl main1");
  /* how to push args on the stack for main? */
  emit("mov X0, #0");
  emit("mov X16, #1");
  emit("svc #0x80");

  /*print integer function*/
  emit("printInt:");
  emit("\tmov x9, sp\n"
    "\tstr x9, [sp, #-32]!//push down the stack\n"
    "\tstr fp, [sp, #16]  //store fp\n"
    "\tstr lr, [sp, #24]  //store lr above fp\n"
    "\tadd fp, sp, #16    //store set fp for this frame\n"
    "\tldr w9, [fp, #32]  //get integer from arg stack space\n"
    "\tstr w9, [fp, #-4]  //store integer in local var space\n"

    "\t/*check for negative value*/\n"
    "\tmov w11, #1\n"
    "\tlsl w11, w11, #31\n"
    "\tand w11, w11, w9\n"
  );

  emit("sign_test:");
  emit("\tcmp w11, #0\n"
    "\tb.eq sign_end\n"
    "\tmov w11, #45\n"
    "\tstrb w11, [fp, #-5]\n"
    "\tmov    x0, #1           // 1 = StdOut\n"
    "\tadd    x1, fp, #-5           // point to memory of byte to be written\n"
    "\tmov    x2, #1           // length of our string\n"
    "\tmov    X16, #4          // Unix write system call\n"
    "\tsvc    #0x80            // Call kernel to output the string\n"
    "\tmov w11, #-1\n"
    "\tmul w9, w9, w11\n"
  );
  emit("sign_end:");
  emit("\t/*set up registers for function */\n"
    "\tmov w10, #1 //placeholder value\n"
  );

  emit("placeholder_loop:");
  emit("\tmov w11, #10\n"
    "\tmul w10, w10, w11\n"
  );
  emit("placeholder_test:");
  emit("\tcmp w10, w9\n"
    "\tb.le placeholder_loop\n"
            
    "\tmov x11, #10\n"
    "\t/* need to add checking for division by 0 */\n"
    "\tsdiv w10, w10, w11\n"
  );

  emit("print_loop:");
  emit("\tsdiv w11, w9, w10\n"
    "\tadd w11, w11, #48\n"
    "\tstrb w11, [fp, #-5]\n"
    "\tmov    x0, #1           // 1 = StdOut\n"
    "\tadd    x1, fp, #-5           // point to memory of byte to be written\n"
    "\tmov    x2, #1           // length of our string\n"
    "\tmov    X16, #4          // Unix write system call\n"
    "\tsvc    #0x80            // Call kernel to output the string\n"
    "\tsdiv w11, w9, w10\n"
    "\tmul w11, w11, w10\n"
    "\tsub w9, w9, w11\n"
    "\tmov w11, #10\n"
    "\tsdiv w10, w10, w11\n"
  );

  emit("print_test:");
  emit("\tcmp w10, #0\n"
    "\tb.ne print_loop\n"

    "\t/* new line */\n"
    "\tmov w11, #10\n"
    "\tstrb w11, [fp, #-5]\n"
    "\tmov    x0, #1           // 1 = StdOut\n"
    "\tadd    x1, fp, #-5           // point to memory of byte to be written\n"
    "\tmov    x2, #1           // length of our string\n"
    "\tmov    X16, #4          // Unix write system call\n"
    "\tsvc    #0x80            // Call kernel to output the string\n"
  );

  emit("printIntEnd:");
  emit("\tldr lr, [fp, #8]  //restore registers\n"
    "\tldr fp, [fp]\n"
    "\tldr x9, [sp]\n"
    "\tmov sp, x9\n"
    "\tret\n"
  );

/**
 * TODO: In Allocate(AATexpression size) in AATBuildTree.c
 * need to enter correct argument offset for ExpressionList call
 * so that the size arg can be accessed correctly in allocation function below
 */

/*
  emit(".globl main");
  emit("main:");
  emit("addi %s,%s,0", AccSP(), SP());
  emit("addi %s, %s, %d", SP(), SP(), - WORDSIZE * STACKSIZE);
  emit("addi %s, %s, 0", Tmp1(), SP());
  emit("addi %s, %s, %d", Tmp1(), Tmp1(), - WORDSIZE * STACKSIZE);
  emit("la %s, HEAPPTR", Tmp2());
  emit("sw %s, 0(%s)", Tmp1(), Tmp2());
  emit("sw %s, %d(%s)", ReturnAddr() , WORDSIZE, SP());
  emit("jal main1");
  emit("lw %s,%d(%s)", ReturnAddr(), WORDSIZE, SP());
  emit("jr %s", ReturnAddr());
}

void emitLibrary() {
  emit("Print:");
  emit("lw $a0, 4(%s)",SP());
  emit("li $v0, 1");
  emit("syscall");
  emit("li $v0,4");
  emit("la $a0, sp");
  emit("syscall");
  emit("jr $ra");

  emit("Println:");
  emit("li $v0,4");
  emit("la $a0, cr");
  emit("syscall");
  emit("jr $ra");

  emit("Read:");
  emit("li $v0,5");
  emit("syscall");
  emit("jr $ra");

  emit("allocate:");
  emit("la %s, HEAPPTR", Tmp1()); 
  emit("lw %s, 0(%s)", Result(), Tmp1());
  emit("lw %s, %d(%s)", Tmp2(), WORDSIZE, SP());
  emit("sub %s, %s, %s", Tmp2(), Result(), Tmp2());
  emit("sw %s, 0(%s)", Tmp2(), Tmp1());
  emit("jr $ra");
  
  emit(".data");
  emit("cr:");
  emit(".asciiz \"\\n\"");
  emit("sp:");
  emit(".asciiz \" \"");
  emit("HEAPPTR:");
  emit(".word 0");
  */
}
