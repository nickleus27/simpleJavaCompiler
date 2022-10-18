/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <string.h>
#include <stdlib.h>
#include "registerArm64.h"


Register R_fp = NULL;
Register R_sp = NULL;
Register R_result32 = NULL;
Register R_result64 = NULL;
Register R_return = NULL;
Register R_acc32 = NULL;
Register R_acc64 = NULL;
Register R_accSP32 = NULL;
Register R_accSP64 = NULL;
Register R_zero = NULL;
Register R32_tmp0 = NULL;
Register R32_tmp1 = NULL;
Register R32_tmp2 = NULL;
Register R64_tmp0 = NULL;
Register R64_tmp1 = NULL;




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
    strcpy(R_result32,"x0");
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
Register AccSP32(void) {
  if (R_accSP32 == NULL) {
    R_accSP32 = (Register) malloc(sizeof(*R_accSP32));
    R_accSP32 = (char *) malloc(sizeof(char)*4);
    strcpy(R_accSP32,"x13");
  }
  return R_accSP32;
}

Register AccSP64(void) {
  if (R_accSP64 == NULL) {
    R_accSP64 = (Register) malloc(sizeof(*R_accSP64));
    R_accSP64 = (char *) malloc(sizeof(char)*4);
    strcpy(R_accSP64,"x12");
  }
  return R_accSP64;
}

   
