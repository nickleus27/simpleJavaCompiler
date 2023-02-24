/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../errors/errors.h"
#include "../assembly/label.h"
#include "registerArm64.h"
#include "../parser/AST.h"
#include "../parser/ASTPrintTree.h"
#include "../assembly/AAT.h"
#include "../assembly/AATPrintTree.h"
#include "../semantic/semantic.h"
#include "codegenArm64.h"

extern int yyparse(void);
extern FILE *yyin;

ASTprogram parse(char *filename)
{
  yyin = fopen(filename, "r");
  if (yyin == NULL)
  {
    fprintf(stderr, "Cannot open file:%s\n", filename);
  }
  if (yyin != NULL && yyparse() == 0)
  { /* parsing worked */
    return ASTroot;
  }
  else
  {
    fprintf(stderr, "Parsing failed\n");
    return NULL;
  }
}

int main(int argc, char **argv)
{
  FILE *output;
  char *outfilename;
  int asCmdStrSize = 12;
  int ldCmdStrSize = 88;
  int nameSize = 0;
  const char *exe = ".out ";
  const char *as = "as -o ";
  const char *ld = "ld -o ";
  const char *ldPost = " -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64";

  ASTprogram program;
  AATstatement assem;
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }
  outfilename = (char *)malloc((strlen(argv[1]) + 3) * sizeof(char));
  strcpy(outfilename, argv[1]);
  strcat(outfilename, ".s");
  nameSize = strlen(argv[1]);

  program = parse(argv[1]);
  if (program != NULL)
  {
    printf("Abstract Syntax Tree\n");
    printf("--------------------\n");
    printAST(program);
    assem = analyzeProgram(program);
    if (!anyErrors())
    {
      printf("\nAbstract Assembly Tree\n");
      printf("----------------------\n");
      printAAT(assem);
      output = fopen(outfilename, "w");
      generateCode(assem, output);
      fflush(output);
      char asCmd[nameSize * 2 + asCmdStrSize + 1];
      strcpy(asCmd, as);
      strcat(asCmd, argv[1]);
      strcat(asCmd, ".o ");
      strcat(asCmd, outfilename);
      system(asCmd);
      char ldCmd[nameSize * 2 + ldCmdStrSize + 1];
      strcpy(ldCmd, ld);
      strcat(ldCmd, argv[1]);
      strcat(ldCmd, exe);
      strcat(ldCmd, argv[1]);
      strcat(ldCmd, ".o");
      strcat(ldCmd, ldPost);
      system(ldCmd);
    } else {
      printf("# of errors = %d", numErrors());
    }
  }
  free(outfilename);
  return 0;
}
