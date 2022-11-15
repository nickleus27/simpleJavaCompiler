/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdlib.h>
#include "AST.h"

ASTprogram ASTProgram(int line, ASTclassList classes, ASTfunctionDecList functiondecs) {
  ASTprogram retval = (ASTprogram) malloc(sizeof(struct ASTprogram_));
  retval->line = line;
  retval->classes = classes;
  retval->functiondecs = functiondecs;
  return retval;
}

ASTclass ASTClass(int line, char *name, ASTinstanceVarDecList instancevars) {
  ASTclass retval = (ASTclass) malloc(sizeof(struct ASTclass_));
  retval->line = line;
  retval->instancevars = instancevars;
  retval->name = name;
  return retval;
}

ASTclassList ASTClassList(int line, ASTclass first, ASTclassList rest) {
  ASTclassList retval = (ASTclassList) malloc(sizeof(struct ASTclassList_));
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}
ASTinstanceVarDec ASTInstanceVarDec(int line, char *type, char *name, int arraydimension) {
  ASTinstanceVarDec retval = (ASTinstanceVarDec) malloc(sizeof(struct ASTinstanceVarDec_));
  retval->line = line;
  retval->type = type;
  retval->name = name;
  retval->arraydimension = arraydimension;
  return retval;
}
  
ASTinstanceVarDecList ASTInstanceVarDecList(int line, ASTinstanceVarDec first,
					    ASTinstanceVarDecList rest) {
  ASTinstanceVarDecList retval = (ASTinstanceVarDecList)
                                  malloc(sizeof(struct ASTinstanceVarDecList_)); 
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}

/**
 * TODO: Need to add functionality for array return types
 * The same way the this is acheived with variable types with an
 * initial array dimension integer
 */
ASTfunctionDec ASTFunctionDef(int line, char *returntype, char *name,
			      ASTformalList formals,
			      ASTstatementList body) {
  ASTfunctionDec retval = (ASTfunctionDec) malloc(sizeof(struct ASTfunctionDec_));
  retval->line = line;
  retval->kind = FunctionDef;
  retval->u.functionDef.returntype = returntype;
  retval->u.functionDef.name = name;
  retval->u.functionDef.formals = formals;
  retval->u.functionDef.body = body;
  return retval;
}

ASTfunctionDec ASTPrototype(int line, char *returntype, char *name,
			    ASTformalList formals) {
  ASTfunctionDec retval = (ASTfunctionDec) malloc(sizeof(struct ASTfunctionDec_));
  retval->line = line;
  retval->kind = Prototype;
  retval->u.prototype.returntype = returntype;
  retval->u.prototype.name = name;
  retval->u.prototype.formals = formals;
  return retval;
}

ASTfunctionDecList ASTFunctionDecList(int line, ASTfunctionDec first,
				      ASTfunctionDecList rest) {
  ASTfunctionDecList retval = (ASTfunctionDecList) malloc(sizeof(struct ASTfunctionDecList_));
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}
ASTformal ASTFormal(int line, char *type, char *name, int arraydimension) {
  ASTformal retval = (ASTformal) malloc(sizeof(struct ASTformal_));
  retval->line = line;
  retval->type = type;
  retval->name = name;
  retval->arraydimension = arraydimension;
  return retval;
}
ASTformalList ASTFormalList(int line, ASTformal first, ASTformalList rest) {
  ASTformalList retval = (ASTformalList) malloc(sizeof(struct ASTformalList_));
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}

ASTstatement ASTAssignStm(int line, ASTvariable lhs, ASTexpression rhs) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = AssignStm;
  retval->u.assignStm.lhs = lhs;
  retval->u.assignStm.rhs = rhs;
  return retval;

}
ASTstatement ASTEmptyStm(int line) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->kind = EmptyStm;
  retval->line = line;
  return retval;
}

ASTstatement ASTIfStm(int line, ASTexpression test, ASTstatement thenstm,
		      ASTstatement elsestm) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind =IfStm;
  retval->u.ifStm.test = test;
  retval->u.ifStm.thenstm = thenstm;
  retval->u.ifStm.elsestm = elsestm;
  return retval;
}
ASTstatement ASTForStm(int line, ASTstatement initialize, ASTexpression test,
		       ASTstatement increment, ASTstatement body) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = ForStm;
  retval->u.forStm.initialize = initialize;
  retval->u.forStm.test = test;
  retval->u.forStm.increment = increment;
  retval->u.forStm.body = body;
  return retval;
}
ASTstatement ASTWhileStm(int line, ASTexpression test, ASTstatement body) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = WhileStm;
  retval->u.whileStm.test = test;
  retval->u.whileStm.body = body;
  return retval;
}

    

ASTstatement ASTDoWhileStm(int line, ASTexpression test, ASTstatement body) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = DoWhileStm;
  retval->u.doWhileStm.test = test;
  retval->u.doWhileStm.body = body;
  return retval;
}

ASTstatement ASTVarDecStm(int line, char *type, char *name, int arraydimension, ASTexpression init) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = VarDecStm;
  retval->u.varDecStm.type = type;
  retval->u.varDecStm.name = name;
  retval->u.varDecStm.arraydimension = arraydimension;
  retval->u.varDecStm.init = init;
  return retval;
}

ASTstatement ASTCallStm(int line, char *name, ASTexpressionList actuals) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = CallStm;
  retval->u.callStm.name = name;
  retval->u.callStm.actuals = actuals;
  return retval;
}
ASTstatement ASTBlockStm(int line, ASTstatementList statements) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = BlockStm;
  retval->u.blockStm.statements = statements;
  return retval;
}

ASTstatement ASTReturnStm(int line, ASTexpression returnval) {
  ASTstatement retval = (ASTstatement) malloc(sizeof(struct ASTstatement_));
  retval->line = line;
  retval->kind = ReturnStm;
  retval->u.returnStm.returnval = returnval;
  return retval;
}

ASTstatementList ASTStatementList(int line, ASTstatement first, ASTstatementList rest) {
  ASTstatementList retval = (ASTstatementList) malloc(sizeof(struct ASTstatementList_));
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}
  
ASTexpression ASTIntLiteralExp(int line, int value) {
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = IntLiteralExp;
  retval->u.intLiteralExp.value = value;
  return retval;
}

ASTexpression ASTBoolLiteralExp(int line, int value) {
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = BoolLiteralExp;
  retval->u.boolLiteralExp.value = value;
  return retval;
}

ASTexpression ASTOpExp(int line, ASToperator operator, ASTexpression left,
			ASTexpression right) {
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = OpExp;
  retval->u.opExp.operator = operator;
  retval->u.opExp.left = left;
  retval->u.opExp.right = right;
  return retval;
}

ASTexpression ASTVarExp(int line, ASTvariable var) {
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = VarExp;
  retval->u.varExp.var = var;
  return retval;
}

ASTexpression ASTNewExp(int line, char *name) {
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = NewExp;
  retval->u.newExp.name = name;
  return retval;
}

ASTexpression ASTNewArrayExp(int line, char *name, ASTexpression size, int arraydimension){
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = NewArrayExp;
  retval->u.newArrayExp.name = name;
  retval->u.newArrayExp.arraydimension = arraydimension;
  retval->u.newArrayExp.size = size;
  return retval;
}

ASTexpression ASTCallExp(int line, char *name, ASTexpressionList actuals){
  ASTexpression retval = (ASTexpression) malloc(sizeof(struct ASTexpression_));
  retval->line = line;
  retval->kind = CallExp;
  retval->u.callExp.name = name;
  retval->u.callExp.actuals = actuals;
  return retval;
}

ASTexpressionList ASTExpressionList(int line, ASTexpression first, ASTexpressionList rest) {
  ASTexpressionList retval = (ASTexpressionList) malloc(sizeof(struct ASTexpressionList_));
  retval->line = line;
  retval->first = first;
  retval->rest = rest;
  return retval;
}

ASTvariable ASTBaseVar(int line, char *name) {
  ASTvariable retval = (ASTvariable) malloc(sizeof(struct ASTvariable_));
  retval->line = line;
  retval->kind =BaseVar;
  retval->u.baseVar.name = name;
  return retval;
}
ASTvariable ASTClassVar(int line, ASTvariable base, char *instance) {
  ASTvariable retval = (ASTvariable) malloc(sizeof(struct ASTvariable_));
  retval->line = line;
  retval->kind = ClassVar;
  retval->u.classVar.base = base;
  retval->u.classVar.instance = instance;
  return retval;
}

ASTvariable ASTArrayVar(int line, ASTvariable base, ASTexpression index) {
  ASTvariable retval = (ASTvariable) malloc(sizeof(struct ASTvariable_));
  retval->line = line;
  retval->kind =ArrayVar;
  retval->u.arrayVar.base = base;
  retval->u.arrayVar.index = index;
  return retval;
}


ASTprogram ASTroot;
