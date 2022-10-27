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


#define WORD 8
#define HALFWORD 4

FILE *outfile;

void generateStatement(AATstatement tree);
void generateExpression32(AATexpression tree);
void generateExpression64(AATexpression tree);
void generateMove(AATstatement tree);
void generateMemoryExpression(AATexpression tree);
void generateOpExp32(AATexpression tree);
void generateOpExp64(AATexpression tree);

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

void generateExpression64(AATexpression tree){
  switch (tree->kind) {
    case AAT_MEMORY:
      generateMemoryExpression(tree);
      break;
    case AAT_CONSTANT:
      emit("mov %s, #%d",Acc64(), *tree->u.constant);
      emit("str %s, [%s]", Acc64(), AccSP64());
      emit("add %s, %s, #-%d", AccSP64(), AccSP64(), WORD);
      break;
    case AAT_REGISTER:
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
    case AAT_OPERATOR:
      /* check if op->LHS == reg or mem... send it to generateExpression64 */
      if(tree->u.operator.left->kind == AAT_REGISTER 
        || tree->u.operator.left->kind == AAT_MEMORY){
        generateExpression64(tree->u.operator.left);
        generateExpression64(tree->u.operator.right);
        generateOpExp64(tree);
      }else{
        generateExpression32(tree->u.operator.left);
        generateExpression32(tree->u.operator.right);
        generateOpExp32(tree);
      }
    break;
    case AAT_FUNCTIONCALL:
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
  }
}


/* check if op->LHS == reg or mem... send it to generateExpression64 */
void generateExpression32(AATexpression tree) {
  switch (tree->kind) {
    case AAT_MEMORY:
      generateMemoryExpression(tree);
      break;
    case AAT_CONSTANT:
      emit("mov %s, #%d",Acc32(), *tree->u.constant);//changed
      emit("str %s, [%s]", Acc32(), AccSP32());//push constand on Expression Stack
      emit("add %s, %s, #-%d", AccSP32(), AccSP32(), INT);//subtract int size off expression stack
      break;
    case AAT_REGISTER:
      emit("str %s, [%s]", tree->u.reg, AccSP32());
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), 0-INT);
      break;
    case AAT_OPERATOR:
      if(tree->u.operator.left->kind == AAT_REGISTER 
        || tree->u.operator.left->kind == AAT_MEMORY){
        generateExpression64(tree->u.operator.left);
        generateExpression64(tree->u.operator.right);
        generateOpExp64(tree);
      }else{
        generateExpression32(tree->u.operator.left);
        generateExpression32(tree->u.operator.right);
        generateOpExp32(tree);
      }
    break;
    case AAT_FUNCTIONCALL:

      break;
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
  }
}
void addActualsToStack(AATexpressionList actual){
  if (!actual)
    return;
  addActualsToStack(actual->rest);
  switch(actual->size_type){
    case(WORD):
    {
      generateExpression64(actual->first);
      emit("ldr %s, [%s, #8]!", Acc32(), AccSP32());
      emit("str %s, [%s, #%d]", Acc32(), SP(), actual->offset);
    }break;
    case(HALFWORD):
    {
      generateExpression64(actual->first);
      emit("ldr %s, [%s, #8]!", Acc64(), AccSP64());
      emit("str %s, [%s, #%d]", Acc64(), SP(), actual->offset);
    }break;
    default: printf("Bad expression size type");
  }
}
void generateStatement(AATstatement tree) {
  switch (tree->kind) {
    case AAT_MOVE:
        generateMove(tree);
        break;
    case AAT_JUMP:

        break;
    case AAT_CONDITIONALJUMP:

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
       * @brief TODO: adjust stack pointer for argMemSize
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
/* 
 * 
 *  Need to add AccSP32() and AccSP64() registers to saved register group x19-x29?
 *  Need to adjust calculations for stack size in EnvArmy64.c for the saved registers
 *  Need to add move and restores AAT to AATBuildTree.c
 * 
 * 
*/

void generateMove(AATstatement tree) {
  /* Generates inefficient assembly */
  if (tree->u.move.lhs->kind == AAT_REGISTER) {
    if(tree->u.move.size == 4){
      generateExpression32(tree->u.move.rhs);
      emit("ldr %s, [%s, #%d]", tree->u.move.lhs->u.reg, AccSP32(), HALFWORD);
      emit("add %s,%s,#%d", AccSP32(), AccSP32(), HALFWORD);
    }else if(tree->u.move.size ==8 ){
      generateExpression64(tree->u.move.rhs);
      /* need to check for SP reg... move first*/
      if( tree->u.move.lhs->u.reg != SP() ){
        emit("ldr %s, [%s, #%d]", tree->u.move.lhs->u.reg, AccSP64(), WORD);
        emit("add %s,%s,#%d", AccSP64(), AccSP64(), WORD);
      }else{
        emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD);
        emit("mov %s, %s", SP(), Acc64());
        emit("add %s,%s,#%d", AccSP64(), AccSP64(), WORD);
      }
    }
  } else if (tree->u.move.lhs->kind == AAT_MEMORY) {
    if(tree->u.move.size == 4){

/**
 * TODO: Problem here!!!
 * 
 */

      generateExpression64(tree->u.move.lhs->u.memory);
      generateExpression32(tree->u.move.rhs);
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);//store address in a reg
      emit("ldr %s, [%s, #%d]", Acc32(), AccSP32(), HALFWORD);//store value in reg (32bit)
      emit("str %s, [%s]", Acc32(), Tmp0_64()); //implement move
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD); //update exp stack pointer
      emit("add %s, %s, #%d", AccSP32(), AccSP32(), HALFWORD);//update exp stack pointer
    }else if (tree->u.move.size ==8 ){
      generateExpression64(tree->u.move.lhs->u.memory);
      generateExpression64(tree->u.move.rhs);
      emit("//implement move");
      emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD * 2);//store address in a reg
      emit("ldr %s, [%s, #%d]", Tmp0_64(), AccSP64(), WORD);//store value in reg (64bit)
      emit("str %s, [%s]", Tmp0_64(), Acc64()); //implement move
      emit("add %s, %s, #%d", AccSP64(), AccSP64(), WORD * 2); //update exp stack pointer
    }
  } else {
    fprintf(stderr,"Bad MOVE node -- LHS should be T_mem or T_register\n");
  }
}

void generateMemoryExpression(AATexpression tree) {
  
  /* generates inefficent code */
  generateExpression64(tree->u.memory);
  emit("ldr %s, [%s, #%d]", Acc64(), AccSP64(), WORD); //changed
  emit("ldr %s, [%s]", Acc64(), Acc64());
  emit("str %s, [%s, #%d]", Acc64(), AccSP64(), WORD);
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
    emit(
        "printInt:\n"
        "mov x9, sp\n"
        "str x9, [sp, #-32]!//push down the stack\n"
        "str fp, [sp, #16]  //store fp\n"
        "str lr, [sp, #24]  //store lr above fp\n"
        "add fp, sp, #16    //store set fp for this frame\n"
        "ldr w9, [fp, #32]  //get integer from arg stack space\n"
        "str w9, [fp, #-4]  //store integer in local var space\n"

        "/*check for negative value*/\n"
        "mov w11, #1\n"
        "lsl w11, w11, #31\n"
        "and w11, w11, w9\n"
"sign_test:\n"
        "cmp w11, #0\n"
        "b.eq sign_end\n"
        "mov w11, #45\n"
        "strb w11, [fp, #-5]\n"
        "mov    x0, #1           // 1 = StdOut\n"
        "add    x1, fp, #-5           // point to memory of byte to be written\n"
        "mov    x2, #1           // length of our string\n"
        "mov    X16, #4          // Unix write system call\n"
        "svc    #0x80            // Call kernel to output the string\n"
        "mov w11, #-1\n"
        "mul w9, w9, w11\n"
"sign_end:\n"
 "       /*set up registers for function */\n"
 "       mov w10, #1 //placeholder value\n"

"placeholder_loop:\n"
"        mov w11, #10\n"
"        mul w10, w10, w11\n"
"placeholder_test:\n"
"        cmp w10, w9\n"
"        b.le placeholder_loop\n"
        
"        mov x11, #10\n"
"        /* need to add checking for division by 0 */\n"
"        sdiv w10, w10, w11\n"

"print_loop:\n"
"        sdiv w11, w9, w10\n"
"        add w11, w11, #48\n"
"        strb w11, [fp, #-5]\n"
"        mov    x0, #1           // 1 = StdOut\n"
"        add    x1, fp, #-5           // point to memory of byte to be written\n"
"        mov    x2, #1           // length of our string\n"
"        mov    X16, #4          // Unix write system call\n"
"        svc    #0x80            // Call kernel to output the string\n"
"        sdiv w11, w9, w10\n"
"        mul w11, w11, w10\n"
"        sub w9, w9, w11\n"
"        mov w11, #10\n"
"        sdiv w10, w10, w11\n"

"print_test:\n"
"        cmp w10, #0\n"
"        b.ne print_loop\n"


"        /* new line */\n"
"        mov w11, #10\n"
"        strb w11, [fp, #-5]\n"
"        mov    x0, #1           // 1 = StdOut\n"
"        add    x1, fp, #-5           // point to memory of byte to be written\n"
"        mov    x2, #1           // length of our string\n"
"        mov    X16, #4          // Unix write system call\n"
"        svc    #0x80            // Call kernel to output the string\n"

"printIntEnd:\n"
"        ldr lr, [fp, #8]  //restore registers\n"
"        ldr fp, [fp]\n"
"        ldr x9, [sp]\n"
"        mov sp, x9\n"
"        ret\n"
        );
    /**
     * @brief TODO: make print(d) function on startup
     * 
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
