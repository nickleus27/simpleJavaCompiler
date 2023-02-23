/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "registerArm64.h"
#define EXP_REGISTERS 5


Register R_fp = NULL;
Register R_sp = NULL;
Register R_result32 = NULL;
Register R_result64 = NULL;
Register R_return = NULL;
Register R_acc32 = NULL;
Register R_acc64 = NULL;
Register R_zero = NULL;
Register R32_tmp0 = NULL;
Register R32_tmp1 = NULL;
Register R32_tmp2 = NULL;
Register R32_tmp3 = NULL;
Register R32_tmp4 = NULL;
Register R32_tmp5 = NULL;
Register R64_tmp0 = NULL;
Register R64_tmp1 = NULL;
Register R64_tmp2 = NULL;
Register R64_tmp3 = NULL;
Register R64_tmp4 = NULL;
Register R64_tmp5 = NULL;
ExpStack expStack = NULL;

struct ExpStack_{
  int index;
  Register expStack32[EXP_REGISTERS];
  Register expStack64[EXP_REGISTERS];
};

void freeRegisters() {
  if (R_fp) {
    free(R_fp);
  }
  if (R_sp) {
    free(R_sp);
  }
  if (R_result32) {
    free(R_result32);
  }
  if (R_result64) {
    free(R_result64);
  }
  if (R_return) {
    free(R_return);
  }
  if (R_acc32) {
    free(R_acc32);
  }
  if (R_acc64) {
    free(R_acc64);
  }
  if (R_zero) {
    free(R_zero);
  }
  if (R32_tmp0) {
    free(R32_tmp0);
  }
  if (R32_tmp1) {
    free(R32_tmp1);
  }
  if (R32_tmp2) {
    free(R32_tmp2);
  }
  if (R32_tmp3) {
    free(R32_tmp3);
  }
  if (R32_tmp4) {
    free(R32_tmp4);
  }
  if (R32_tmp5) {
    free(R32_tmp5);
  }
  if (R64_tmp0) {
    free(R64_tmp0);
  }
  if (R64_tmp1) {
    free(R64_tmp1);
  }
  if (R64_tmp2) {
    free(R64_tmp2);
  }
  if (R64_tmp3) {
    free(R64_tmp3);
  }
  if (R64_tmp4) {
    free(R64_tmp4);
  }
  if (R64_tmp5) {
    free(R64_tmp5);
  }
  if (expStack) {
    free(expStack);
  }
}

Register FP(void) {
  if (R_fp == NULL) {
    R_fp = (char *) malloc(sizeof(char)*3);
    strcpy(R_fp,"FP");
  } 
  return R_fp;
}


Register SP(void){
  if (R_sp == NULL) {
    R_sp = (char *) malloc(sizeof(char)*3);
    strcpy(R_sp,"SP");
  }
  return R_sp;
}


Register Zero(void) {
  if (R_zero == NULL) {
    R_zero = (char *) malloc(sizeof(char)*4);
    strcpy(R_zero,"xzr");
  } 
  return R_zero;
}


Register Result32(void) {
  if (R_result32 == NULL) {
    R_result32 = (char *) malloc(sizeof(char)*3);
    strcpy(R_result32,"w0");
  } 
  return R_result32;
}
Register Result64(void) {
  if (R_result64 == NULL) {
    R_result64 = (char *) malloc(sizeof(char)*3);
    strcpy(R_result64,"x0");
  } 
  return R_result64;
}

Register Acc32(void){
  if (R_acc32 == NULL) {
    R_acc32 = (char *) malloc(sizeof(char)*3);
    strcpy(R_acc32,"w9");
  } 
  return R_acc32;
}
Register Acc64(void){
  if (R_acc64 == NULL) {
    R_acc64 = (char *) malloc(sizeof(char)*3);
    strcpy(R_acc64,"x9");
  } 
  return R_acc64;
}
Register ReturnAddr(void){
  if (R_return == NULL) {
    R_return = (char *) malloc(sizeof(char)*3);
    strcpy(R_return,"LR");
  } 
  return R_return;
}

Register Tmp0_32(void){
  if (R32_tmp0 == NULL) {
    R32_tmp0 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp0,"w10");
  } 
  return R32_tmp0;
}
Register Tmp1_32(void) {
  if (R32_tmp1 == NULL) {
    R32_tmp1 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp1,"w11");
  }
  return R32_tmp1;
}
Register Tmp2_32(void) {
  if (R32_tmp2 == NULL) {
    R32_tmp2 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp2,"w12");
  }
  return R32_tmp2;
}
Register Tmp3_32(void){
  if (R32_tmp3 == NULL) {
    R32_tmp3 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp3,"w13");
  } 
  return R32_tmp3;
}
Register Tmp4_32(void) {
  if (R32_tmp4 == NULL) {
    R32_tmp4 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp4,"w14");
  }
  return R32_tmp4;
}
Register Tmp5_32(void) {
  if (R32_tmp5 == NULL) {
    R32_tmp5 = (char *) malloc(sizeof(char)*4);
    strcpy(R32_tmp5,"w15");
  }
  return R32_tmp5;
}
Register Tmp0_64(void) {
  if (R64_tmp0 == NULL) {
    R64_tmp0 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp0,"x10");
  }
  return R64_tmp0;
}
Register Tmp1_64(void) {
  if (R64_tmp1 == NULL) {
    R64_tmp1 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp1,"x11");
  }
  return R64_tmp1;
}
Register Tmp2_64(void) {
  if (R64_tmp2 == NULL) {
    R64_tmp2 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp2,"x12");
  }
  return R64_tmp2;
}
Register Tmp3_64(void) {
  if (R64_tmp3 == NULL) {
    R64_tmp3 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp3,"x13");
  }
  return R64_tmp3;
}
Register Tmp4_64(void) {
  if (R64_tmp4 == NULL) {
    R64_tmp4 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp4,"x14");
  }
  return R64_tmp4;
}
Register Tmp5_64(void) {
  if (R64_tmp5 == NULL) {
    R64_tmp5 = (char *) malloc(sizeof(char)*4);
    strcpy(R64_tmp5,"x15");
  }
  return R64_tmp5;
}

ExpStack getExpStack() {
  if (expStack == NULL) {
    expStack = (ExpStack)malloc(sizeof(struct ExpStack_));
    expStack->index = 0;
    expStack->expStack32[0] = Tmp1_32();
    expStack->expStack32[1] = Tmp2_32();
    expStack->expStack32[2] = Tmp3_32();
    expStack->expStack32[3] = Tmp4_32();
    expStack->expStack32[4] = Tmp5_32();
    expStack->expStack64[0] = Tmp1_64();
    expStack->expStack64[1] = Tmp2_64();
    expStack->expStack64[2] = Tmp3_64();
    expStack->expStack64[3] = Tmp4_64();
    expStack->expStack64[4] = Tmp5_64();
  }
  return expStack;
}

Register pushExpReg32() {
  ExpStack exp = getExpStack();
  if (exp->index<EXP_REGISTERS) {
    return exp->expStack32[exp->index++];
  }
  exp->index++;
  return NULL;
}

Register pushExpReg64() {
  ExpStack exp = getExpStack();
  if (exp->index<EXP_REGISTERS) {
    return exp->expStack64[exp->index++];
  }
  exp->index++;
  return NULL;

}

Register popExpReg32() {
  ExpStack exp = getExpStack();
  exp->index--;
  if (exp->index<EXP_REGISTERS) {
    return exp->expStack32[exp->index];
  }
  return NULL;
}

Register popExpReg64() {
  ExpStack exp = getExpStack();
  exp->index--;
  if (exp->index<EXP_REGISTERS) {
    return exp->expStack64[exp->index];
  }
  return NULL;
}

Register getTempReg(int index) {
  ExpStack exp = getExpStack();
  return exp->expStack64[index];
}

int getUsedRegs() {
  ExpStack exp = getExpStack();
  if (exp->index <= EXP_REGISTERS) {
    return exp->index;
  }
  return EXP_REGISTERS;
}