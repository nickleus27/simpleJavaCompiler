/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include "../errors/errors.h"
#include "label.h"
#include "../codegen/registerArm64.h"
#include "AST.h"
#include "ASTPrintTree.h"
#include "AAT.h"
#include "AATPrintTree.h"
#include "semantic.h"
#include "AATBuildTree.h"
#include "MachineDependent.h"

//int baseOffset;
//int actualOffset;
int offset;

int main() {
  offset = 0;
  AATstatement s1;

  //s1 = AssignmentStatement(BaseVariable(WORDSIZE),ConstantExpression(13));
  /*
  AATexpression x = BaseVariable(offset++);
  AATexpression y = BaseVariable(offset++);
  AATexpressionList e1 = AATExpressionList(y, NULL);

  e1 = AATExpressionList(ConstantExpression(3), e1);

  e1 = AATExpressionList(AATOperator(x, ConstantExpression(1), AAT_PLUS), e1);
  
  s1 = AssignmentStatement(x, CallExpression(e1, "foo"));
  */
 /*
 AATexpression x = BaseVariable(offset++);
 AATexpression base = BaseVariable(offset++);
 AATexpression index = ConstantExpression(3);
 s1 = AssignmentStatement(ArrayVariable(base, index, WORDSIZE), x);
 */
/*
AATexpression s = BaseVariable(offset++);
AATexpression x = ConstantExpression(1);
s1 = AssignmentStatement(ClassVariable(s, 1), x);
*/
//AATexpression test = ConstantExpression(1);
AATexpression a = BaseVariable(0);
AATexpression b = ConstantExpression(5);
s1 = AssignmentStatement(a, b);
/*
AATexpression test = OperatorExpression( a, b, AAT_LT);
s1 = AssignmentStatement(a, b);
AATexpression a1 = BaseVariable(offset++);
AATexpression b1 = ConstantExpression(11);
AATstatement s2 = AssignmentStatement(a1, b1);
//s1 = IfStatement(test, s1, NULL);
//s1 = WhileStatement(test, s1);
s1 = ForStatement(s1, test, s2, s1);
*/
AATstatement s2 = functionDefinition(s1, INT, "foo", NewNamedLabel("funcEnd"));
  printAAT(s2);

  /* Add more tests, to test the rest of your AATBuildTree interface */

}
