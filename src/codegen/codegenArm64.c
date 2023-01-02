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
void generateConstantExp_offset(AATexpression tree);
void generateRegisterExp(AATexpression tree);
void generateOpExp(AATexpression tree);
void generateFunCall(AATexpression tree);

void addActualsToStack(AATexpressionList actual);

void emit(char *assem,...);
void emitLibrary(void);
void emitPrintInt(void);
void emitAllocate(void);
void emitSetupCode(void);


void generateCode(AATstatement tree, FILE *output) {
  outfile = output; 
  emitSetupCode();
  generateStatement(tree);
  emitLibrary();
  fprintf(output,"\n");
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
    case AAT_OFFSET:
      generateConstantExp_offset(tree);
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
      emit("mov %s, #%d", Acc32(), tree->u.constant);
      emit("str %s, [%s]", Acc32(), AccSP32());//use str (not strb) because placing in 4b (1word) space on stack
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG32:
      emit("mov %s, #%d", Acc32(), tree->u.constant);
      emit("str %s, [%s]", Acc32(), AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG64:
      emit("mov %s, #%d", Acc64(), tree->u.constant);
      emit("str %s, [%s]", Acc64(), AccSP64());
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
    break;
    default:
    break;
  }
}
void generateConstantExp_offset(AATexpression tree){
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("mov %s, #%d", Acc32(), tree->u.offset->offset);
      emit("str %s, [%s]", Acc32(), AccSP32());//use str (not strb) because placing in 4b (1word) space on stack
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG32:
      emit("mov %s, #%d", Acc32(), tree->u.offset->offset);
      emit("str %s, [%s]", Acc32(), AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
    break;
    case SWITCH_REG64:
      emit("mov %s, #%d", Acc64(), tree->u.offset->offset);
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
          /**
       * TODO: check if argMemSize is 0 before adjusting stack size
       * 
       */
      if (tree->u.functionCall.argMemSize){
        emit("mov %s, %s", Acc64(), SP());
        emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result32(), AccSP32());
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
        emit("str %s, [%s]", Acc64(), SP());
        emit("mov %s, %s", SP(), Acc64());
      } else {
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result32(), AccSP32());
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
      }
    break;
    case SWITCH_REG32:
      if (tree->u.functionCall.argMemSize){
        emit("mov %s, %s", Acc64(), SP());
        emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result32(), AccSP32());
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
        emit("str %s, [%s]", Acc64(), SP());
        emit("mov %s, %s", SP(), Acc64());
      } else {
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result32(), AccSP32());
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-HALFWORD);
      }
    break;
    case SWITCH_REG64:
    if (tree->u.functionCall.argMemSize){
        emit("mov %s, %s", Acc64(), SP());
        emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result64(), AccSP64());
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
        emit("str %s, [%s]", Acc64(), SP());
        emit("mov %s, %s", SP(), Acc64());
    } else {
        emit("bl %s", tree->u.functionCall.jump);
        emit("str %s, [%s]", Result64(), AccSP64());
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), 0-WORD);
    }
    break;
    default:
    break;
  }
}

void generateOpExp64(AATexpression tree){
  if ( tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) { //if size types dont match
    emit("ldr %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD);
    emit("sxtw %s, %s", Acc64(), Acc32());
    emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
  } else {
    emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
    emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);
  }
  switch (tree->u.operator.op){
    case AAT_PLUS:
      emit("add %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      break;
    case AAT_MINUS:
      emit("sub %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      break;
    case AAT_MULTIPLY:
      emit("mul %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      break;
    case AAT_DIVIDE:
    /**
     * TODO: Need to add code to throw error when dividing by ZERO
     */
      emit("sdiv %s, %s, %s", Acc64(), Acc64(), Tmp0_64());
      break;
    case AAT_LT:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, lt", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;
    case AAT_GT:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, gt", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;
    case AAT_LEQ:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, le", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;;
    case AAT_GEQ:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, ge", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;
    case AAT_EQ:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, eq", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;
    case AAT_NEQ:
      emit("cmp %s, %s", Acc64(), Tmp0_64());
      emit("cset %s, ne", Acc64());
      if (tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
        emit("str %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
      } else {
        emit("str %s, [%s]", Acc32(), AccSP32()); //str lower 32bits of Acc64()
        emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0 - HALFWORD);
        emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2);
      }
      return;
    case AAT_AND:

      break;
    case AAT_OR:

      break;
    case AAT_NOT:

      break;
    default: emit("Bad 64bit Operator Expression");
  }
  if ( tree->u.operator.left->size_type == REG32 || tree->u.operator.right->size_type == REG32) {
    emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD);
    emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
  } else {
    emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);
    emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD);
  }
}

void generateOpExp32(AATexpression tree){
    /** REFACTOR THIS HERE AND RESTORE BELOW SWITCH AND REMOVE REDUNCDANT CODE
      emit("ldr %s, [%s, #%d]", Tmp0_32(), AccSP32(), WORD);
      emit("ldr %s, [%s, #%d]", Tmp1_32(), AccSP32(), HALFWORD);
      */
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
  /** ADD THIS CODE AFTER SWITCH AND REMOVE FROM EACH CASE...REDUNDANT
   *  emit("str %s, [%s, #%d]", Acc32(), AccSP32(), WORD);
   *  emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);
   * 
   */
}
void addActualsToStack(AATexpressionList actual){
  if (!actual)
    return;
  addActualsToStack(actual->rest);
  switch(actual->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case(SWITCH_BYTE):
    {
      generateExpression(actual->first);
      emit("ldrb %s, [%s, #%d]!", Acc32(), AccSP32(), HALFWORD);
      emit("str %s, [%s, #%d]", Acc32(), SP(), actual->offset);
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
      if (tree->u.procedureCall.argMemSize){ // check if need to add args to stack
        emit("mov %s, %s", Acc64(), SP());
        emit("str %s, [%s, #-%d]!", Acc64(), SP(), tree->u.procedureCall.argMemSize);
        addActualsToStack(tree->u.procedureCall.actuals);
        emit("bl %s", tree->u.procedureCall.jump);
        emit("str %s, [%s]", Acc64(), SP());
        emit("mov %s, %s", SP(), Acc64());
      } else {
        emit("bl %s", tree->u.procedureCall.jump);
      }
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
  emit(".bss");
  emit(".dword 0");
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
}

void emitLibrary(void) {
  emitPrintInt();
  emitAllocate();
  /*
  emit(".bss");
  emit(".align 4");
  emit("heap_ptr:	.dword	0");
  */
 emit("\t.globl\t_free_list                      ; @free_list\n");
emit(".zerofill __DATA,__common,_free_list,8,3\n");
}

void emitPrintInt(void) {
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
}

void emitAllocate(void) {
  /*
  emit("allocate:");
  emit("\t//function start\n"
    "\tmov x9, sp\n"
    "\tstr x9, [sp, #-32]!//push down the stack\n"
    "\tstr fp, [sp, #16]  //store fp\n"
    "\tstr lr, [sp, #24]  //store lr above fp\n"
    "\tadd fp, sp, #16    //store set fp for this frame\n"

    "\tldr w12, [FP, #32]  // get size arg from params 32 bit size\n"
    "\tsxtw x12, w12    //transfer 32bit to 64bit for memory access\n"
    "\tmov x14, #16 //used in heap_size_test loop\n"
    "\tb heap_size_test\n"
  );
  emit("heap_size_start:");
  emit("add x12, x12, #1");
  emit("heap_size_test:");
  emit("\tsdiv x13, x12, x14\n"
    "\tmul x13, x13, x14\n"
    "\tsub x13, x12, x13\n"
    "\tcmp x13, #0\n"
    "\tb.ne heap_size_start\n"

    "\tADRP x10, heap_ptr@PAGE   //get address of heap ptr\n"
    "\tADD	x10, x10, heap_ptr@PAGEOFF\n" 
    "\tldr x11, [x10]      // deref address\n"
    "\tcmp x11, #0         // check if heap ptr has been init (if heap ptr == 0)\n"
    "\tb.ne alloc_init_skip// skip initialize heap ptr\n"
    "\tadd x11, x10, #16   // offset address by 16 so heap_ptr is not overwritten\n"
  );
  emit("alloc_init_skip:");
  emit("\t// here is where allocate memory\n"
    "\tadd x11, x11, x12   // add size offset to heap_ptr\n"
    "\tstr x11, [x10]      // update heap_ptr\n"
    "\t//function end\n"
    "\tmov x0, x11         // return ptr to allocated mem\n"
    "\tldr lr, [fp, #8]    // restore registers\n"
    "\tldr fp, [fp]\n"
    "\tldr x9, [sp]\n"
    "\tmov sp, x9\n"
    "\tret\n"
  );
  */

emit("allocate:                              ; @allocate");
emit("; %%bb.0:");
emit("\tsub\tsp, sp, #96");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tadd\tx8, x8, _free_list@PAGEOFF");
emit("\tadd\tx8, x8, #8");
emit("\tstr\tx8, [sp, #16]                   ; 8-byte Folded Spill");
emit("\tldr\tw0, [sp, #112]");
emit("\tstr\tw0, [sp, #84]");
emit("\tldr\tw8, [sp, #84]");
emit("\tadd\tw8, w8, #8");
emit("\tstr\tw8, [sp, #84]");
emit("\tb\tLBB0_1");
emit("LBB0_1:                                 ; =>This Inner Loop Header: Depth=1");
emit("\tldr\tw8, [sp, #84]");
emit("\tmov\tw10, #8");
emit("\tsdiv\tw9, w8, w10");
emit("\tmul\tw9, w9, w10");
emit("\tsubs\tw8, w8, w9");
emit("\tcbz\tw8, LBB0_3");
emit("\tb\tLBB0_2");
emit("LBB0_2:                                 ;   in Loop: Header=BB0_1 Depth=1");
emit("\tldr\tw8, [sp, #84]");
emit("\tadd\tw8, w8, #1");
emit("\tstr\tw8, [sp, #84]");
emit("\tb\tLBB0_1");
emit("LBB0_3:");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tldr\tx8, [x8, _free_list@PAGEOFF]");
emit("\tcbnz\tx8, LBB0_5");
emit("\tb\tLBB0_4");
emit("LBB0_4:");
emit("\tldr\tx9, [sp, #16]                   ; 8-byte Folded Reload");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tstr\tx9, [x8, _free_list@PAGEOFF]");
emit("\tldr\tx9, [x8, _free_list@PAGEOFF]");
emit("\tmov\tw8, #49536");
emit("\tmovk\tw8, #124, lsl #16");
emit("\tstr\tw8, [x9]");
emit("\tb\tLBB0_5");
emit("LBB0_5:");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tldr\tx8, [x8, _free_list@PAGEOFF]");
emit("\tstr\tx8, [sp, #64]");
emit("\tldr\tx8, [sp, #64]");
emit("\tadd\tx8, x8, #8");
emit("\tstr\tx8, [sp, #56]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tx8, [x8]");
emit("\tcbnz\tx8, LBB0_9");
emit("\tb\tLBB0_6");
emit("LBB0_6:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tx9, [sp, #64]");
emit("\tldr\tw9, [x9]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.gt\tLBB0_8");
emit("\tb\tLBB0_7");
emit("LBB0_7:");
emit("\tldr\tx8, [sp, #56]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldrsw\tx9, [sp, #84]");
emit("\tadd\tx9, x8, x9");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tstr\tx9, [x8, _free_list@PAGEOFF]");
emit("\tldr\tx9, [sp, #64]");
emit("\tldr\tw9, [x9]");
emit("\tstr\tw9, [sp, #52]");
emit("\tldr\tw9, [sp, #84]");
emit("\tldr\tx10, [sp, #64]");
emit("\tstr\tw9, [x10]");
emit("\tldr\tx8, [x8, _free_list@PAGEOFF]");
emit("\tstr\tx8, [sp, #64]");
emit("\tldr\tw8, [sp, #52]");
emit("\tldr\tw9, [sp, #84]");
emit("\tsubs\tw8, w8, w9");
emit("\tldr\tx9, [sp, #64]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_8:");
emit("                                        ; kill: def $x8 killed $xzr");
emit("\tstr\txzr, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_9:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tx9, [sp, #64]");
emit("\tldr\tw9, [x9]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.ge\tLBB0_11");
emit("\tb\tLBB0_10");
emit("LBB0_10:");
emit("\tldr\tx8, [sp, #56]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldrsw\tx9, [sp, #84]");
emit("\tadd\tx9, x8, x9");
emit("\tadrp\tx8, _free_list@PAGE");
emit("\tstr\tx9, [x8, _free_list@PAGEOFF]");
emit("\tldr\tx9, [sp, #64]");
emit("\tldr\tw9, [x9]");
emit("\tstr\tw9, [sp, #48]");
emit("\tldr\tw9, [sp, #84]");
emit("\tldr\tx10, [sp, #64]");
emit("\tstr\tw9, [x10]");
emit("\tldr\tx8, [x8, _free_list@PAGEOFF]");
emit("\tstr\tx8, [sp, #64]");
emit("\tldr\tw8, [sp, #48]");
emit("\tldr\tw9, [sp, #84]");
emit("\tsubs\tw8, w8, w9");
emit("\tldr\tx9, [sp, #64]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_11:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tx9, [sp, #64]");
emit("\tldr\tw9, [x9]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.ne\tLBB0_13");
emit("\tb\tLBB0_12");
emit("LBB0_12:");
emit("\tldr\tx8, [sp, #56]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tx8, [x8]");
emit("\tadrp\tx9, _free_list@PAGE");
emit("\tstr\tx8, [x9, _free_list@PAGEOFF]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_13:");
emit("\tldr\tx8, [sp, #64]");
emit("\tadd\tx8, x8, #8");
emit("\tstr\tx8, [sp, #64]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tx8, [x8]");
emit("\tstr\tx8, [sp, #56]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tw8, [x8]");
emit("\tstr\tw8, [sp, #44]");
emit("\tb\tLBB0_14");
emit("LBB0_14:                                ; =>This Inner Loop Header: Depth=1");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tx8, [x8]");
emit("\tmov\tw9, #0");
emit("\tstr\tw9, [sp, #12]                   ; 4-byte Folded Spill");
emit("\tcbz\tx8, LBB0_16");
emit("\tb\tLBB0_15");
emit("LBB0_15:                                ;   in Loop: Header=BB0_14 Depth=1");
emit("\tldr\tw8, [sp, #44]");
emit("\tldr\tw9, [sp, #84]");
emit("\tsubs\tw8, w8, w9");
emit("\tcset\tw8, lt");
emit("\tstr\tw8, [sp, #12]                   ; 4-byte Folded Spill");
emit("\tb\tLBB0_16");
emit("LBB0_16:                                ;   in Loop: Header=BB0_14 Depth=1");
emit("\tldr\tw8, [sp, #12]                   ; 4-byte Folded Reload");
emit("\ttbz\tw8, #0, LBB0_18");
emit("\tb\tLBB0_17");
emit("LBB0_17:                                ;   in Loop: Header=BB0_14 Depth=1");
emit("\tldr\tx8, [sp, #56]");
emit("\tadd\tx8, x8, #8");
emit("\tstr\tx8, [sp, #56]");
emit("\tldr\tx8, [sp, #56]");
emit("\tstr\tx8, [sp, #64]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tx8, [x8]");
emit("\tstr\tx8, [sp, #56]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tw8, [x8]");
emit("\tstr\tw8, [sp, #44]");
emit("\tb\tLBB0_14");
emit("LBB0_18:");
emit("\tldr\tx8, [sp, #56]");
emit("\tadd\tx8, x8, #8");
emit("\tstr\tx8, [sp, #32]");
emit("\tldr\tx8, [sp, #32]");
emit("\tcbnz\tx8, LBB0_22");
emit("\tb\tLBB0_19");
emit("LBB0_19:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tw9, [sp, #44]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.gt\tLBB0_21");
emit("\tb\tLBB0_20");
emit("LBB0_20:");
emit("\tldr\tx8, [sp, #32]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tw8, [x8]");
emit("\tstr\tw8, [sp, #28]");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tx9, [sp, #56]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #32]");
emit("\tldrsw\tx9, [sp, #84]");
emit("\tadd\tx8, x8, x9");
emit("\tldr\tx9, [sp, #32]");
emit("\tstr\tx8, [x9]");
emit("\tldr\tx8, [sp, #32]");
emit("\tldr\tx8, [x8]");
emit("\tstr\tx8, [sp, #32]");
emit("\tldr\tw8, [sp, #28]");
emit("\tldr\tw9, [sp, #84]");
emit("\tsubs\tw8, w8, w9");
emit("\tldr\tx9, [sp, #32]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_21:");
emit("                                        ; kill: def $x8 killed $xzr");
emit("\tstr\txzr, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_22:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tw9, [sp, #44]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.ge\tLBB0_24");
emit("\tb\tLBB0_23");
emit("LBB0_23:");
emit("\tldr\tx8, [sp, #32]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #56]");
emit("\tldr\tw8, [x8]");
emit("\tstr\tw8, [sp, #24]");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tx9, [sp, #56]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #32]");
emit("\tldrsw\tx9, [sp, #84]");
emit("\tadd\tx8, x8, x9");
emit("\tldr\tx9, [sp, #64]");
emit("\tstr\tx8, [x9]");
emit("\tldr\tx8, [sp, #64]");
emit("\tldr\tx8, [x8]");
emit("\tstr\tx8, [sp, #32]");
emit("\tldr\tw8, [sp, #24]");
emit("\tldr\tw9, [sp, #84]");
emit("\tsubs\tw8, w8, w9");
emit("\tldr\tx9, [sp, #32]");
emit("\tstr\tw8, [x9]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_24:");
emit("\tldr\tw8, [sp, #84]");
emit("\tldr\tw9, [sp, #44]");
emit("\tsubs\tw8, w8, w9");
emit("\tb.ne\tLBB0_26");
emit("\tb\tLBB0_25");
emit("LBB0_25:");
emit("\tldr\tx8, [sp, #32]");
emit("\tstr\tx8, [sp, #72]");
emit("\tldr\tx8, [sp, #32]");
emit("\tldr\tx8, [x8]");
emit("\tldr\tx9, [sp, #64]");
emit("\tstr\tx8, [x9]");
emit("\tldr\tx8, [sp, #72]");
emit("\tstr\tx8, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_26:");
emit("                                        ; kill: def $x8 killed $xzr");
emit("\tstr\txzr, [sp, #88]");
emit("\tb\tLBB0_27");
emit("LBB0_27:");
emit("\tldr\tx0, [sp, #88]");
emit("\tadd\tsp, sp, #96");
/**
 * TODO: caller function expects sp to be in x9 returning from callee. should fix this in future
 */
emit("\tmov x9, sp");
emit("\tret");
emit("                                        ; -- End function");

}
