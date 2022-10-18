/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include "../errors/errors.h"
#include "label.h"
#include "../codegen/registerArm64.h"
#include "AST.h"
#include "ASTPrintTree.h"
#include "AAT.h"
#include "AATPrintTree.h"
#include "../semantic/semantic.h"

extern int yyparse(void);
extern FILE *yyin;

ASTprogram parse(char *filename) {
  yyin = fopen(filename,"r");
  if (yyin == NULL) {
    fprintf(stderr,"Cannot open file:%s\n",filename);
  }
  if  (yyin != NULL && yyparse() == 0)  { /* parsing worked */
    return ASTroot;
  } else { 
    fprintf(stderr,"Parsing failed\n");
    return NULL;
  }
}


int main(int argc, char **argv) {
  ASTprogram program;
  AATstatement assem;
 if (argc!=2) {
   fprintf(stderr,"usage: %s filename\n",argv[0]); 
   exit(1);
 }
 program = parse(argv[1]);
 if (program != NULL) {
   printAST(program);
   assem = analyzeProgram(program);
   if (!anyErrors()) {
     printAAT(assem);
   }
 }
 
 return 0;
}

