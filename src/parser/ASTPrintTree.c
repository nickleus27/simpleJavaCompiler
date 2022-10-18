/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include "AST.h"
#define INITIAL_INDENT_STEP 3

int indentstep = INITIAL_INDENT_STEP;

void printIndent(int indent, int line);
void printClassList(int indent, ASTclassList classes);
void printClass(int indent, ASTclass class);
void printInstanceVarDecList(int indent, ASTinstanceVarDecList instancevars);
void printInstanceVarDec(int indent, ASTinstanceVarDec instacevar);
void printFunctionDecList(int indent, ASTfunctionDecList functions);
void printFunctionDec(int indent, ASTfunctionDec function);
void printFormalList(int indent, ASTformalList formals);
void printFormal(int indent, ASTformal formal);
void printStatementList(int indent, ASTstatementList statements);
void printExpressionList(int indent, ASTexpressionList expressions);
void printStatement(int indent, ASTstatement statement);
void printExpression(int indent, ASTexpression exp);
void printOperator(ASToperator op);
void printVariable(int indent,ASTvariable var);

void printVariable(int indent,ASTvariable var) {
  switch(var->kind) {
  case BaseVar:
    printIndent(indent, var->line);
    printf("BASE VARIABLE: %s\n",var->u.baseVar.name);
    break;
  case ClassVar:
    printIndent(indent, var->line);
    printf("CLASS VARIABLE\n");
    printIndent(indent+1, var->line);
    printf("BASE:\n");
    printVariable(indent+2,var->u.classVar.base);
    printIndent(indent+1, var->line);
    printf("INSTANCE: %s\n",var->u.classVar.instance);
    break;
  case ArrayVar:
    printIndent(indent, var->line);
    printf("ARRAY VARIABLE\n");
    printIndent(indent+1, var->line);
    printf("BASE:\n");
    printVariable(indent+2,var->u.arrayVar.base);
    printIndent(indent+1, var->line);
    printf("INDEX:\n");
    printExpression(indent+2,var->u.arrayVar.index);
    break;

  }
}



void printOperator(ASToperator op) {
  switch (op) {
  case AST_EQ:
    printf("==");
    break;
  case AST_NEQ:
    printf("!=");
    break;
  case AST_LT:
    printf("<");
    break;
  case AST_GT:
    printf(">");
    break;
  case AST_LEQ:
    printf("<=");
    break;
  case AST_GEQ:
    printf(">=");
    break;
  case AST_AND:
    printf("&&");
    break;
  case AST_OR:
    printf("||");
    break;
  case AST_NOT:
    printf("!");
    break;
  case AST_PLUS:
    printf("+");
    break;
  case AST_MINUS:
    printf("-");
    break;
  case AST_MULTIPLY:
    printf("*");
    break;
  case AST_DIVIDE:
    printf("/");
    break;
  }

}

void printExpression(int indent, ASTexpression exp) {
  int i;
  switch(exp->kind) {
  case IntLiteralExp:
    printIndent(indent,exp->line);
    printf("INTEGER LITERAL: %d\n",exp->u.intLiteralExp.value);
    break;
  case BoolLiteralExp:
    printIndent(indent,exp->line);
    printf("BOOLEAN LITERAL: ");
    if (exp->u.boolLiteralExp.value) 
      printf("true \n");
    else
      printf("false \n");
    break;
  case OpExp:
    printIndent(indent, exp->line);
    printf("OPERATOR: ");
    printOperator(exp->u.opExp.operator);
    printf("\n");
    printExpression(indent+1, exp->u.opExp.left);
    printExpression(indent+1, exp->u.opExp.right);
    break;
  case VarExp:
    printVariable(indent,exp->u.varExp.var);
    break;
  case CallExp:
    printIndent(indent, exp->line);
    printf("CALL EXPRESSION: %s\n",exp->u.callExp.name);
    if (exp->u.callExp.actuals != NULL) {
      printIndent(indent+1, exp->line);
      printf("ACTUALS\n");
      printExpressionList(indent+2, exp->u.callExp.actuals);
    } else {
      printIndent(indent+1, exp->line);
      printf("NO ACTUALS\n");
      
    }
    break;
  case NewExp:
    printIndent(indent, exp->line);
    printf("NEW EXPRESSION: %s\n",exp->u.newExp.name);
    break;
  case NewArrayExp:
    printIndent(indent, exp->line);
    printf("NEW ARRAY EXPRESSION: %s",exp->u.newArrayExp.name);
    for (i=0; i<exp->u.newArrayExp.arraydimension; i++) 
      printf("[]");
    printf("\n");
    printIndent(indent+1, exp->line);
    printf("SIZE:\n");
    printExpression(indent+2,exp->u.newArrayExp.size);
    break;
  }
}




void printStatement(int indent, ASTstatement statement) {
  int i;
  switch(statement->kind) {
  case AssignStm:
    printIndent(indent, statement->line);
    printf("ASSIGN:\n");
    printIndent(indent+1,statement->u.assignStm.lhs->line);
    printf("LHS:\n");
    printVariable(indent+2, statement->u.assignStm.lhs);
    printIndent(indent+1,statement->u.assignStm.rhs->line);
    printf("RHS:\n");
    printExpression(indent+2, statement->u.assignStm.rhs);
    break;
  case IfStm:
    printIndent(indent, statement->line);
    printf("IF:\n");
    printIndent(indent+1,statement->u.ifStm.test->line);
    printf("TEST\n");
    printExpression(indent+2,statement->u.ifStm.test);
    printIndent(indent+1,statement->u.ifStm.thenstm->line);
    printf("THEN\n");
    printStatement(indent+2,statement->u.ifStm.thenstm);
    if (statement->u.ifStm.elsestm != NULL) {
      printIndent(indent+1,statement->u.ifStm.elsestm->line);
      printf("ELSE\n");
      printStatement(indent+2,statement->u.ifStm.elsestm);
    }
    break;
  case ForStm:
    printIndent(indent, statement->line);
    printf("FOR:\n");
    printIndent(indent+1,statement->u.forStm.initialize->line);
    printf("INITIALIZE\n");
    printStatement(indent+2,statement->u.forStm.initialize);
    printIndent(indent+1,statement->u.forStm.test->line);
    printf("TEST\n");
    printExpression(indent+2,statement->u.forStm.test);
    printIndent(indent+1,statement->u.forStm.increment->line);
    printf("INCREMENT\n");
    printStatement(indent+2,statement->u.forStm.increment);
    printIndent(indent+1,statement->u.forStm.increment->line);
    printf("BODY\n");
    printStatement(indent+2,statement->u.forStm.body);
    break;
  case WhileStm:
    printIndent(indent, statement->line);
    printf("WHILE:\n");
    printIndent(indent+1,statement->u.whileStm.test->line);
    printf("TEST:\n");
    printExpression(indent+2,statement->u.whileStm.test);
    printIndent(indent+1,statement->u.whileStm.body->line);
    printf("BODY:\n");
    printStatement(indent+2,statement->u.whileStm.body);
    break;
  case DoWhileStm:
    printIndent(indent, statement->line);
    printf("DO-WHILE:\n");
    printIndent(indent+1,statement->u.doWhileStm.test->line);
    printf("TEST:\n");
    printExpression(indent+2,statement->u.doWhileStm.test);
    printIndent(indent+1,statement->u.doWhileStm.body->line);
    printf("BODY:\n");
    printStatement(indent+2,statement->u.doWhileStm.body);
    break;
  case VarDecStm:
    printIndent(indent, statement->line);
    printf("%s %s", statement->u.varDecStm.type, statement->u.varDecStm.name);
    for (i=0; i<statement->u.varDecStm.arraydimension; i++) 
      printf("[]");
    printf("\n");
    if (statement->u.varDecStm.init == NULL) {
      printIndent(indent+1,statement->line);
      printf("NO INIT\n");
    } else {
      printIndent(indent+1,statement->line);
      printf("INIT\n");
      printExpression(indent+2,statement->u.varDecStm.init);
    }

    break;
  case CallStm:
    printIndent(indent, statement->line);
    printf("CALL STATEMENT: %s\n",statement->u.callStm.name);
    printIndent(indent+1, statement->line);
    printf("ACTUALS\n");
    fflush(stdout);
    printExpressionList(indent+2, statement->u.callStm.actuals);
    break;
  case BlockStm:
    printIndent(indent,statement->line);
    printf("{\n");
    printStatementList(indent+1, statement->u.blockStm.statements);
    printIndent(indent,statement->line);
    printf("}\n");
    break;
  case ReturnStm:
    printIndent(indent,statement->line);
    printf("RETURN\n");
    if (statement->u.returnStm.returnval != NULL) 
      printExpression(indent+1,statement->u.returnStm.returnval);
    else {
      printIndent(indent+1, statement->line);
      printf("<NONE>");
    }
      break;
  case EmptyStm:
    printIndent(indent,statement->line);
    printf("EMPTY\n");
    break;
  }
}


void printExpressionList(int indent, ASTexpressionList expressions) {
  if (expressions != NULL) {
    printExpression(indent, expressions->first);
    printExpressionList(indent, expressions->rest);
  }
}



void printStatementList(int indent, ASTstatementList statements) {
  if (statements != NULL) {
    printStatement(indent,statements->first);
    printStatementList(indent,statements->rest);
  }
}


void printFormal(int indent, ASTformal formal) {
  int i;
  printIndent(indent,formal->line);
  printf("%s %s", formal->type, formal->name);
  for (i=0; i<formal->arraydimension; i++) 
    printf("[]");
  printf("\n");
}

void printFormalList(int indent, ASTformalList formals) {
  if (formals != NULL) {
    printFormal(indent, formals->first);
    printFormalList(indent, formals->rest);
  }
}

void printFunctionDec(int indent, ASTfunctionDec function) {
  if (function->kind == Prototype) {
    printIndent(indent, function->line);
    printf("Prototype: %s Returns %s\n",function->u.prototype.name,function->u.prototype.returntype);
    printIndent(indent+1,function->line);
    printf("Formals\n");
    printFormalList(indent+2, function->u.prototype.formals);
  } else if (function->kind == FunctionDef) {
    printIndent(indent, function->line);
    printf("Function Definition: %s Returns %s\n",function->u.functionDef.name,function->u.functionDef.returntype);
    printIndent(indent+1,function->line);
    printf("Formals:\n");
    printFormalList(indent+2, function->u.functionDef.formals);
    if (function->u.functionDef.body != NULL) {
      printIndent(indent+1,function->u.functionDef.body->line);
      printf("Function Body:\n");
      printStatementList(indent+2, function->u.functionDef.body);
    } else {
      printIndent(indent+1,function->line);
      printf("NO BODY\n");
    }
  } else {
    printf("ERROR IN FUNCTION DEC AT LINE %d",function->line);
  }
}


void printFunctionDecList(int indent, ASTfunctionDecList functions) {
  if (functions != NULL) {
    printFunctionDec(indent, functions->first);
    printFunctionDecList(indent, functions->rest);
  }
}

void printIndent(int indent, int line) {
  int i;
  printf("%3d ",line);
  for (i=0; i<indent*indentstep; i++) {
    printf(" ");
  }
  fflush(stdout);
}


void printAST(ASTprogram program) {
  printClassList(0, program->classes);
  printFunctionDecList(0, program->functiondecs);
}

void printClassList(int indent, ASTclassList classes) {
  if (classes != NULL) {
    printClass(indent, classes->first);
    printClassList(indent, classes->rest);
  }
}
  
void printClass(int indent, ASTclass class) {
  printIndent(indent, class->line);
  printf("Class %s\n", class->name);
  printInstanceVarDecList(indent+1, class->instancevars);
}

void printInstanceVarDecList(int indent, ASTinstanceVarDecList instancevars) {
  if (instancevars != NULL) {
    printInstanceVarDec(indent, instancevars->first);
    printInstanceVarDecList(indent, instancevars->rest);
  }
}

void printInstanceVarDec(int indent, ASTinstanceVarDec instancevar) {
  int i;
  printIndent(indent,instancevar->line);
  printf("%s %s", instancevar->type, instancevar->name);
  for (i=0; i<instancevar->arraydimension; i++) {
    printf("[]");
  }
  printf("\n");
}


void setASTIndent(int newIndent) {
  indentstep = newIndent;
}
