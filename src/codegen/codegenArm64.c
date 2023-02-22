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

/* global variables */
FILE *outfile;
int exp_stack_offset;

/* statement generation */
void generateStatement(AATstatement tree);
void generateMove(AATstatement tree);

/* expression generation */
void generateExpression(AATexpression tree);
void generateOpExp32(AATexpression tree);
void generateOpExp64(AATexpression tree);
void generateLeftExp(AATexpression tree);
void generateRightExp(AATexpression tree);
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
  exp_stack_offset = 0;
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

// spill expression onto stack for use later (when all temp registers are in use)
void pushExpStack() {
  if (exp_stack_offset == 0) {
    exp_stack_offset = 8;
    emit("sub %s, %s, #%d", SP(), SP(), QWORD);
  } else {
    exp_stack_offset -= 8;
  }
  //int offset = popExpStackOffset();
  if (exp_stack_offset) {
    emit("str %s, [%s, #%d]", Acc64(), SP(), exp_stack_offset);
  } else {
    emit("str %s, [%s]", Acc64(), SP());
  }
}

// move expression from stack for operation with accumulator
void popExpStack() {
  if (exp_stack_offset) {
    emit("ldr %s, [%s, #%d]", Tmp0_64(), SP(), exp_stack_offset);
  } else {
    emit("ldr %s, [%s]", Tmp0_64(), SP());
  }
  if (exp_stack_offset == 8) {
    emit("add %s, %s, #%d", SP(), SP(), QWORD);
    exp_stack_offset = 0;
  } else {
    exp_stack_offset += 8;
  }
}

// move expression to temp registers for use later
void pushExp(int size_type){
  Register tempReg;
  switch (size_type/4){
    case SWITCH_BYTE:
      tempReg = pushExpReg32();
      if (tempReg) {
        emit("mov %s, %s", tempReg, Acc32());
      } else {
        pushExpStack();
      }
      break;
    case SWITCH_REG32:
      tempReg = pushExpReg32();
      if (tempReg) {
        emit("mov %s, %s", tempReg, Acc32());
      } else {
        pushExpStack();
      }
      break;
    case SWITCH_REG64:
    tempReg = pushExpReg64();
      if (tempReg) {
        emit("mov %s, %s", tempReg, Acc64());
      } else {
        pushExpStack();
      }
      break;
    default:
      emit("/*BAD PUSHEXP EXPRESSION*/\n");
      break;
  }
}

// pop regsister from temp register to Tmp0 for operation with accumulator
void popExp(int size_type) {
  Register tempReg;
  switch (size_type/4){
    case SWITCH_BYTE:
      tempReg = popExpReg32();
      if (tempReg) {
        emit("mov %s, %s", Tmp0_32(), tempReg);
      } else {
        popExpStack();
      }
      break;
    case SWITCH_REG32:
      tempReg = popExpReg32();
      if (tempReg) {
        emit("mov %s, %s", Tmp0_32(), tempReg);
      } else {
        popExpStack();
      }
      break;
    case SWITCH_REG64:
      tempReg = popExpReg64();
      if (tempReg) {
        emit("mov %s, %s", Tmp0_64(), tempReg);
      } else {
        popExpStack();
      }
      break;
    default:
      emit("/*BAD POPEXP EXPRESSION*/\n");
      break;
  }
}

void generateExpression(AATexpression tree){
  switch (tree->kind) {
    case AAT_MEMORY:
    {
      generateMemoryExpression(tree);
      free(tree);
      break;
    }
    case AAT_CONSTANT:
    {
      generateConstantExp(tree);
      free(tree);
      break;
    }
    case AAT_OFFSET:
    {
      generateConstantExp_offset(tree);
      free(tree);
      break;
    }
    case AAT_REGISTER:
    {
      generateRegisterExp(tree);
      //free(tree);
      break;
    }
    case AAT_OPERATOR:
    {
      generateLeftExp(tree->u.operator.left);
      generateRightExp(tree->u.operator.right);
      generateOpExp(tree);
      free(tree);
    }
    break;
    case AAT_FUNCTIONCALL:
    {
      generateFunCall(tree);
      free(tree);
      break;
    }
  }
}
void generateLeftExp(AATexpression tree) {
  int size = tree->size_type;
  generateExpression(tree);
  pushExp(size);
}
void generateRightExp(AATexpression tree) {
    switch(tree->size_type/4) {
    case SWITCH_BYTE:
      generateExpression(tree);
      break;
    case SWITCH_REG32:
      generateExpression(tree);
      break;
    case SWITCH_REG64:
      generateExpression(tree);
      break;
    default:
      emit("/*BAD RIGHT HAND EXPRESSION*/\n");
      break;
  }
}
void generateConstantExp(AATexpression tree){
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("mov %s, #%d", Acc32(), tree->u.constant);
      break;
    case SWITCH_REG32:
      emit("mov %s, #%d", Acc32(), tree->u.constant);
      break;
    case SWITCH_REG64:
      emit("mov %s, #%d", Acc64(), tree->u.constant);
      break;
    default:
      emit("/*BAD CONSTANT EXPRESSION*/\n");
      break;
  }
}
void generateConstantExp_offset(AATexpression tree){
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("mov %s, #%d", Acc32(), tree->u.offset->offset);
      break;
    case SWITCH_REG32:
      emit("mov %s, #%d", Acc32(), tree->u.offset->offset);
      break;
    case SWITCH_REG64:
      emit("mov %s, #%d", Acc64(), tree->u.offset->offset);
      break;
    default:
      emit("/*BAD OFFSET EXPRESSION*/\n");
      break;
  }
  OFFSET_REF_DEC(tree->u.offset);
}

void generateRegisterExp(AATexpression tree){
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      emit("mov %s, %s", Acc32(), tree->u.reg);
      break;
    case SWITCH_REG32:
      emit("mov %s, %s", Acc32(), tree->u.reg);
      break;
    case SWITCH_REG64:
      emit("mov %s, %s", Acc64(), tree->u.reg);
      break;
    default:
      emit("/*BAD REGISTER EXPRESSION*/\n");
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

void saveTempReg() {
  int regs = getUsedRegs();
  if (regs == 0) {
    return;
  }
  int size = getUsedRegs() * 8;
  while (size % 16) {
    size++;
  }
  emit("sub %s, %s, #%d", SP(), SP(), size);
  int offset = REG64 * (regs-1);
  for (int i = 0; i < regs; i++) {
    if (offset) {
      emit("str %s, [%s, #%d]", getTempReg(i), SP(), offset);
    } else {
      emit("str %s, [%s]", getTempReg(i), SP());
    }
    offset-=8;
  }
}

void restoreTempReg() {
  int regs = getUsedRegs();
  if (regs == 0) {
    return;
  }
  int size = getUsedRegs() * 8;
  while (size % 16) {
    size++;
  }
  int offset = REG64 * (regs-1);
  for (int i = 0; i < regs; i++) {
    if (offset) {
      emit("ldr %s, [%s, #%d]", getTempReg(i), SP(), offset);
    } else {
      emit("ldr %s, [%s]", getTempReg(i), SP());
    }
    offset-=8;
  }
  emit("add %s, %s, #%d", SP(), SP(), size);
}

void generateFunCall(AATexpression tree){
  label_ref jump = tree->u.functionCall.jump;
  switch(tree->size_type/4){// devide by 4 to minimize space 1/4==0, 4/4==1, 8/4==2
    case SWITCH_BYTE:
      saveTempReg();
      if (tree->u.functionCall.argMemSize){
        emit("add %s, %s, #-%d", SP(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc32(), Result32());
        emit("add %s, %s, #%d", SP(), SP(), tree->u.functionCall.argMemSize);
      } else {
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc32(), Result32());
      }
      restoreTempReg();
      break;
    case SWITCH_REG32:
      saveTempReg();
      if (tree->u.functionCall.argMemSize){
        emit("add %s, %s, #-%d", SP(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc32(), Result32());
        emit("add %s, %s, #%d", SP(), SP(), tree->u.functionCall.argMemSize);
      } else {
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc32(), Result32());
      }
      restoreTempReg();
      break;
    case SWITCH_REG64:
      saveTempReg();
      if (tree->u.functionCall.argMemSize){
        emit("add %s, %s, #-%d", SP(), SP(), tree->u.functionCall.argMemSize);
        addActualsToStack(tree->u.functionCall.actuals);
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc64(), Result64());
        emit("add %s, %s, #%d", SP(), SP(), tree->u.functionCall.argMemSize);
      } else {
        emit("bl %s", jump->label);
        emit("mov %s, %s", Acc64(), Result64());
      }
      restoreTempReg();
      break;
    default:
      emit("/*BAD FUNCTION CALL EXPRESSION*/\n");
      break;
  }
  LABEL_REF_DEC(jump)
}

void generateOpExp64(AATexpression tree){
  popExp(tree->size_type);
  if (tree->u.operator.left->size_type == REG32) { //if size types dont match
    emit("sxtw %s, %s", Tmp0_64(), Tmp0_32());
  }
  if (tree->u.operator.right->size_type == REG32) {
    emit("sxtw %s, %s", Acc64(), Acc32());
  }
  switch (tree->u.operator.op){
    case AAT_PLUS:
      emit("add %s, %s, %s", Acc64(), Tmp0_64(), Acc64());
      break;
    case AAT_MINUS:
      emit("sub %s, %s, %s", Acc64(), Tmp0_64(), Acc64());
      break;
    case AAT_MULTIPLY:
      emit("mul %s, %s, %s", Acc64(), Tmp0_64(), Acc64());
      break;
    case AAT_DIVIDE:
    /**
     * TODO: Need to add code to throw error when dividing by ZERO
     */
      emit("sdiv %s, %s, %s", Acc64(), Tmp0_64(), Acc64());
      break;
    case AAT_LT:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, lt", Acc64());
      break;
    case AAT_GT:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, gt", Acc64());
      break;
    case AAT_LEQ:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, le", Acc64());
      return;;
    case AAT_GEQ:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, ge", Acc64());
      return;
    case AAT_EQ:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, eq", Acc64());
      return;
    case AAT_NEQ:
      emit("cmp %s, %s", Tmp0_64(), Acc64());
      emit("cset %s, ne", Acc64());
      return;
    case AAT_AND:
    /**
     * TODO:
     */
      break;
    case AAT_OR:
    /**
     * TODO:
     */
      break;
    case AAT_NOT:
    /**
     * TODO:
     */
      break;
    default: 
      emit("Bad 64bit Operator Expression");
      break;
  }
}

void generateOpExp32(AATexpression tree){
  popExp(tree->size_type);
  switch (tree->u.operator.op){
    case AAT_PLUS:
      emit("add %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_MINUS:
      emit("sub %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_MULTIPLY:
      emit("mul %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_DIVIDE:
      /**
     * TODO: Need to add code to throw error when dividing by ZERO
     */
      emit("sdiv %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_LT:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, lt", Acc32());
      break;
    case AAT_GT:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, gt", Acc32());
      break;
    case AAT_LEQ:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, le", Acc32());
      break;
    case AAT_GEQ:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, ge", Acc32());
      break;
    case AAT_EQ:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, eq", Acc32());
      break;
    case AAT_NEQ:
      emit("cmp %s, %s", Tmp0_32(), Acc32());
      emit("cset %s, ne", Acc32());
      break;
    case AAT_AND:
      emit("and %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_OR:
      emit("orr %s, %s, %s", Acc32(), Tmp0_32(), Acc32());
      break;
    case AAT_NOT:
      emit("mvn %s, %s", Acc32(), Acc32()); //Acc32 == boolean value, move NOT boolean value
      emit("add %s, %s, %s", Acc32(), Tmp0_32(), Acc32()); // add #2 to inverted value to get 0 | 1
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
      generateExpression(actual->first);
      emit("strb %s, [%s, #%d]", Acc32(), SP(), actual->offset);
    }break;
    case(SWITCH_REG32):
    {
      generateExpression(actual->first);
      emit("str %s, [%s, #%d]", Acc32(), SP(), actual->offset);
    }break;
    case(SWITCH_REG64):
    {
      generateExpression(actual->first);
      emit("str %s, [%s, #%d]", Acc64(), SP(), actual->offset);
    }break;
    default: emit("Bad expression size type\n");
  }
  free(actual);
}
void generateStatement(AATstatement tree) {
  switch (tree->kind) {
    case AAT_MOVE:
    {
      generateMove(tree);
      break;
    }
    case AAT_JUMP:
    {
      label_ref jump = tree->u.jump;
      emit("b %s", jump->label);
      LABEL_REF_DEC(jump)
      break;
    }
    case AAT_CONDITIONALJUMP:
    {
      label_ref jump = tree->u.conditionalJump.jump;
      generateExpression(tree->u.conditionalJump.test);
      emit("cmp %s, #1", Acc32());
      emit("b.eq %s", jump->label);
      LABEL_REF_DEC(jump)
      break;
    }
    case AAT_FUNCDEF:
    {
      int localVarSize = tree->u.functionDef.framesize - 2*8; /* saving 2 reegisters lr, fp */
      generateStatement(tree->u.functionDef.labels->u.sequential.left); // start label
      /* assembly code fore saving registers and adjusting pointers*/
      emit("add %s, %s, #-%d", SP(), SP(), tree->u.functionDef.framesize);
      emit("str %s, [%s, #%d]", FP(), SP(), localVarSize);
      emit("add %s, %s, #%d", FP(), SP(), localVarSize);
      emit("str %s, [%s, #%d]", ReturnAddr(), FP(), DWORD);
      generateStatement(tree->u.functionDef.body);
      generateStatement(tree->u.functionDef.labels->u.sequential.right); // end label
      emit("ldr %s, [%s, #%d]", ReturnAddr(), FP(), DWORD);
      emit("ldr %s, [%s]", FP(), FP());
      emit("add %s, %s, #%d", SP(), SP(), tree->u.functionDef.framesize);
      emit("ret");
    }
    break;
    case AAT_PROCEDURECALL:
    {
      /**
       * adjust stack pointer for argMemSize
       * walk actual/formal list and add to stack according to offsets
       * bl startLabel
       * move sp back up the size of argMemSize
       */
      saveTempReg();
      label_ref jump = tree->u.procedureCall.jump;
      if (tree->u.procedureCall.argMemSize){ // check if need to add args to stack
        emit("add %s, %s, #-%d", SP(), SP(), tree->u.procedureCall.argMemSize);
        addActualsToStack(tree->u.procedureCall.actuals);
        emit("bl %s", jump->label);
        emit("add %s, %s, #%d", SP(), SP(), tree->u.procedureCall.argMemSize);
      } else {
        emit("bl %s", jump->label);
      }
      LABEL_REF_DEC(jump)
      restoreTempReg();
      break;
    }
    case AAT_SEQ:
    {
      generateStatement(tree->u.sequential.left);
      generateStatement(tree->u.sequential.right);
      break;
    }
    case AAT_EMPTY:
    {
      break;
    }
    case AAT_LABEL:
    {
      label_ref label = tree->u.label;
      emit("%s:", label->label);
      LABEL_REF_DEC(label)
      break;
    }
    case AAT_RETURN:
    {
      emit("ret");
      break;
    }
    case AAT_HALT:
    {
      break;
    }
  }
  free(tree);
}

void generateMove(AATstatement tree) {
  /* Generates inefficient assembly */
  if (tree->u.move.lhs->kind == AAT_REGISTER) {
    generateExpression(tree->u.move.rhs);
    if(tree->u.move.size == REG32){
      emit("mov %s, %s", tree->u.move.lhs->u.reg, Acc32());
    }else if(tree->u.move.size == REG64 ){
      emit("mov %s, %s", tree->u.move.lhs->u.reg, Acc64());
    }else if(tree->u.move.size == BYTE){
      emit("mov %s, %s", tree->u.move.lhs->u.reg, Acc32());
    }
  } else if (tree->u.move.lhs->kind == AAT_MEMORY) {
    int size = tree->u.move.lhs->u.memory->size_type;
    generateExpression(tree->u.move.lhs->u.memory);//bypass dereference value @ mem addresss & just get address
    pushExp(size); //memory will have size type 8 bytes
    generateExpression(tree->u.move.rhs);
    popExp(size); //memory to pop address back into xn register
    if(tree->u.move.size == REG32){
      emit("str %s, [%s]", Acc32(), Tmp0_64()); //implement move
    }else if (tree->u.move.size == REG64 ){
      emit("str %s, [%s]", Acc64(), Tmp0_64()); //implement move
    }else if (tree->u.move.size == BYTE ){
      emit("strb %s, [%s]", Acc32(), Tmp0_64()); //implement move
    }
  } else {
    fprintf(stderr,"Bad MOVE node -- LHS should be T_mem or T_register\n");
  }
  //free(tree->u.move.lhs->u.memory);
}

void generateMemoryExpression(AATexpression tree) {
  /* generates inefficent code */
  generateExpression(tree->u.memory);
  if(tree->size_type == REG32){
    emit("ldr %s, [%s]", Acc32(), Acc64()); //dereference address to get value
  }else if(tree->size_type == REG64){
    emit("ldr %s, [%s]", Acc64(), Acc64());
  }else if (tree->size_type == BYTE){
    emit("ldrb %s, [%s]", Acc32(), Acc64()); //dereference address to get value
  }
}

void emitSetupCode(void) {
  emit(".globl _start");
  emit(".align 4");
  emit(".bss");
  emit(".dword 0");
  emit(".text");
  emit("_start:");
  emit("mov %s, #%d", FP(), 0);
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
  emit(
    "\tadd sp, sp, #-32   //push down the stack\n"
    "\tstr fp, [sp, #16]  //store fp\n"
    "\tstr lr, [sp, #24]  //store lr above fp\n"
    "\tadd fp, sp, #16    //store set fp for this frame\n"
    "\tldr w9, [fp, #24]  //get integer from arg stack space\n"
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
    "\tadd sp, sp, #32\n"
    "\tret\n"
  );
}

void emitAllocate(void) {
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
emit("\tret");
emit("                                        ; -- End function");

}
