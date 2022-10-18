/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include "../assembly/label.h"
#include "../codegen/registerArm64.h"
#include "../assembly/AAT.h"
#include "../assembly/AATBuildTree.h"
#include "../parser/AST.h"
#include "environment1.h"
#include "type.h"
#include "environment2.h"
#include "semantic.h"
#include "../codegen/MachineDependent.h"
#include <stdio.h>
#include "../errors/errors.h"
#include "env_arm64.h"
#include <string.h>

typedef struct expressionRec_ expressionRec;

struct expressionRec_ {
   type typ;
   AATexpression tree;
};

#define NO_RETURN_TYPE 0
#define CORRECT_RETURN_TYPE 1
#define NON_VOID_TYPE 2
#define IN_BlOCK_SCOPE 1
#define END_BLOCK_SCOPE 0
#define ON 1
#define OFF 0

/* GLOBALS */
/* does expressionRec need to be global here ? */
int offset;  /* offset of last declared variable */
int RETURN_FLAG = NO_RETURN_TYPE;
int forStmFlag = END_BLOCK_SCOPE;
int blockScopeFlag = END_BLOCK_SCOPE;
envEntry GLOBfunctPtr = NULL;
stack_env functionStack = NULL;

/* does ExpressionRec need to return a pointer??? */
expressionRec ExpressionRec(type typ, AATexpression tree);

/* old function prototypes */
expressionRec analyzeOpExpression(environment typeEnv, environment functionEnv,environment varEnv, ASTexpression exp);
expressionRec analyzeExpression(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp);
expressionRec analyzeVar(environment typeEnv, environment functionEnv, environment varEnv, ASTvariable var);
expressionRec analyzeCallExp(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp);
AATstatement analyzeStatement(environment typeEnv, environment functionEnv, environment varEnv, ASTstatement statement);
AATstatement analyzeFunction(environment typeEnv, environment functionEnv, environment varEnv, ASTfunctionDec function);
void analyzeClass(environment typeEnv, environment functionEnv, environment varEnv, ASTclass class);
AATstatement visitStatementList(environment typeEnv, environment functionEnv, environment varEnv, ASTstatementList statement);
AATstatement visitFunctionList(environment typeEnv, environment functionEnv, environment varEnv, ASTfunctionDecList function);
void visitClassList(environment typeEnv, environment functionEnv, environment varEnv, ASTclassList class);
expressionRec analyzeArrayVar(environment typeEnv, environment functionEnv, environment varEnv, ASTvariable var);
AATexpression analyzeIndex(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression index);
expressionRec visitClassVar(environment typeEnv, environment functionEnv,environment varEnv, ASTvariable var);
envEntry analyzeFormal(environment typeEnv, environment functionEnv, environment varEnv, ASTformal formal);
void visitFormals(environment typeEnv, environment varEnv, typeList protoList, ASTfunctionDec function, ASTformalList formals);
typeList analyzeFormalList(environment typeEnv, environment functionEnv, environment varEnv, ASTformalList formals);
envEntry enterArrayTypesClass(environment typeEnv, envEntry varType, ASTinstanceVarDec node);
envEntry enterArrayTypesDectStm(environment typeEnv, envEntry varType, ASTstatement statement);
envEntry enterArrayTypesFormal(environment typeEnv, envEntry varType, ASTformal formal);

/* does this need to be malloced here and return a pointer ??? */
expressionRec ExpressionRec(type typ, AATexpression tree) {
  expressionRec retval;
  retval.typ = typ;
  retval.tree = tree;
  return retval;
}

/************************ OLD SEMANTIC.C ******************************************/


AATstatement analyzeProgram(ASTprogram program) {
  environment typeEnv;
  environment functionEnv;
  environment varEnv;

  typeEnv = Environment();
  functionEnv = Environment();
  varEnv = Environment();

  AddBuiltinTypes(typeEnv);
  AddBuiltinFunctions(functionEnv);
  initMemTrackers();
  functionStack = new_stack_env(8);

  /* analyze classes */
  visitClassList(typeEnv, functionEnv, varEnv, program->classes);
  /* analyze functions */
  AATstatement stm = visitFunctionList(typeEnv, functionEnv, varEnv, program->functiondecs);
  AATseqStmCleanUp(stm);
  return AATpop();
}
AATstatement visitFunctionList(environment typeEnv, environment functionEnv, environment varEnv, ASTfunctionDecList function){
  if(!function){
    AATstatement stm = SequentialStatement(NULL, NULL); 
    AATpush(stm); 
    return stm;
  }
  AATstatement stmLst = visitFunctionList(typeEnv, functionEnv, varEnv, function->rest);
  stmLst->u.sequential.left = analyzeFunction(typeEnv, functionEnv, varEnv, function->first);
  stmLst->u.sequential.right = SequentialStatement(NULL, NULL);
  return stmLst->u.sequential.right;
}
AATstatement visitStatementList(environment typeEnv, environment functionEnv, environment varEnv, ASTstatementList statement){
  if(!statement){
    AATstatement stm = SequentialStatement(NULL, NULL); 
    AATpush(stm); 
    return stm;
  }
  AATstatement stmLst = visitStatementList(typeEnv, functionEnv, varEnv, statement->rest);
  stmLst->u.sequential.left = analyzeStatement(typeEnv, functionEnv, varEnv, statement->first);
  stmLst->u.sequential.right = SequentialStatement(NULL, NULL);
  return stmLst->u.sequential.right;
}

void visitClassList(environment typeEnv, environment functionEnv, environment varEnv, ASTclassList class){
  if(!class) return;
  visitClassList(typeEnv, functionEnv, varEnv, class->rest);
  analyzeClass(typeEnv, functionEnv, varEnv, class->first);
}
/* adds nd arrays to type environment in analyze class function*/
envEntry enterArrayTypesClass(environment typeEnv, envEntry varType, ASTinstanceVarDec node){
  #define keyLen 101
  type baseType = varType->u.typeEntry.typ;
  char key[keyLen];
  /*check to make sure size is less than key */
  if( strlen(node->type)+1 >= keyLen)
    Error(node->line, " Array type %s must be less than 100 characters", node->type);
  else
    sprintf(key,"%s", node->type);
  for(int i = 0; i < node->arraydimension; i++){
    if( strlen(key)+1 + 2 >= keyLen)
      Error(node->line, " Array type %s must be less than 100 characters with included []'s", node->type);
    else
      sprintf(key, "%s%s", key, "[]");
    if( !find(typeEnv, key) ){
        enter( typeEnv, strdup(key), TypeEntry( ArrayType( baseType ) ) );
    }
    baseType = find(typeEnv, key)->u.typeEntry.typ;
  }
  return find(typeEnv, key);
}
/* adds nd arrays to type environment in analyze statement function*/
envEntry enterArrayTypesDectStm(environment typeEnv, envEntry varType, ASTstatement statement){
#define keyLen 101
  type baseType = varType->u.typeEntry.typ;
  char key[keyLen];
    /*check to make sure size is less than key */
  if( strlen(statement->u.varDecStm.type)+1 >= keyLen)
    Error(statement->line, " Array type %s must be less than 100 characters", statement->u.varDecStm.type);
  else
    sprintf(key, "%s", statement->u.varDecStm.type );
  for(int i = 0; i < statement->u.varDecStm.arraydimension; i++){
    if( strlen(key)+1 + 2 >= keyLen)
      Error(statement->line, " Array type %s must be less than 100 characters with included []'s", statement->u.varDecStm.type);
    else
      sprintf(key, "%s%s", key, "[]");
    if( !find(typeEnv, key) ){
      enter( typeEnv, strdup(key), TypeEntry( ArrayType( baseType ) ) );
    }
    baseType = find(typeEnv, key)->u.typeEntry.typ;
  }
  return find(typeEnv, key);
}

/* adds nd arrays to type environment in analyzeFormal function*/
envEntry enterArrayTypesFormal(environment typeEnv, envEntry varType, ASTformal formal){
#define keyLen 101
  type baseType = varType->u.typeEntry.typ;
  char key[keyLen];
  /*check to make sure size is less than key */
  if( strlen(formal->type)+1 >= keyLen)
    Error(formal->line, " Array type %s must be less than 100 characters", formal->type);
  else
    sprintf(key, "%s", formal->type );
  for(int i = 0; i < formal->arraydimension; i++){
    if( strlen(formal->type)+1 +2 >= keyLen)
      Error(formal->line, " Array type %s must be less than 100 characters with include []'s", formal->type);
    else
      sprintf(key, "%s%s", key, "[]");
    if( !find(typeEnv, key) ){
      enter( typeEnv, strdup(key), TypeEntry( ArrayType( baseType ) ) );
    }
    baseType = find(typeEnv, key)->u.typeEntry.typ;
  }
  return find(typeEnv, key);
}

void analyzeInstanceVarDecList(environment typeEnv, environment classVarEnv, environment varEnv, ASTinstanceVarDecList varList){
  if ( !varList ) return;
  analyzeInstanceVarDecList(typeEnv, classVarEnv, varEnv, varList->rest);
  envEntry varType = find(typeEnv, varList->first->type);
  if ( !varType ){
    Error(varList->line," %s type not defined", varList->first->type);
  }else{
    if( varList->first->arraydimension ){
      /* check for array type and enter into type env */
      varType = enterArrayTypesClass(typeEnv, varType, varList->first);
    } /* enter variable here */
      enter( classVarEnv, varList->first->name, VarEntry( varType->u.typeEntry.typ, varType->u.typeEntry.typ->size_type ) );
  }
}
/*Todo: make recursive function for instanceVarList */
void analyzeClass(environment typeEnv, environment functionEnv, environment varEnv, ASTclass class){
  environment classVarEnv = Environment();
  analyzeInstanceVarDecList(typeEnv, classVarEnv, varEnv, class->instancevars);
  enter( typeEnv, class->name, TypeEntry( ClassType( classVarEnv ) ) );
}

envEntry analyzeFormal(environment typeEnv, environment functionEnv, environment varEnv, ASTformal formal){
    envEntry varType = find(typeEnv, formal->type);
    if ( !varType ){
      Error(formal->line," %s type not defined", formal->type);
    }else{
      if( formal->arraydimension ){
          /* check for array type and enter into type env */
        varType = enterArrayTypesFormal(typeEnv, varType, formal);
      } /* enter variable here */
    }
    return varType;
}

/* compares function def to prototype formal typeList. */
/* Adds function def formals to varEnv */
void visitFormals(environment typeEnv, environment varEnv, typeList protoList, ASTfunctionDec function, ASTformalList formals){
  if (!formals && protoList || formals && !protoList){
    Error(function->line, " function formals differ from function prototype");
  }
  if ( !formals && !protoList ) return;
  if( protoList && formals )
    visitFormals(typeEnv, varEnv, protoList->rest, function, formals->rest);
  else if ( !protoList && formals)
    visitFormals(typeEnv, varEnv, NULL, function, formals->rest);
  else if ( protoList && !formals )
    visitFormals(typeEnv, varEnv, protoList->rest, function, NULL);
  else 
    visitFormals(typeEnv, varEnv, NULL, function, NULL);
  if (formals){
    envEntry formType = find(typeEnv, formals->first->type);
    if( !formType ){ 
      Error( formals->line, " %s type not defined", formals->first->type );
    }else{
      if( formals->first->arraydimension ){
        /* check for array type and enter into type env */
        formType = enterArrayTypesFormal(typeEnv, formType, formals->first);
      }/* enter variable here */
      enter( varEnv, formals->first->name, VarEntry( formType->u.typeEntry.typ, formType->u.typeEntry.typ->size_type ) );
    }
    if(formType && protoList){
      if ( formType->u.typeEntry.typ != protoList->first)
        Error( formals->line, " %s type does not match function prototype", formals->first->type );
    }else if ( !formType && protoList){
      Error( function->line, " formals do not match function prototype" );
    }else if ( formType && !protoList){
      Error( function->line, " formals do not match function prototype" );
    }
  }
}

/* adds functiondef/functionproto formals to typeList and returns list */
typeList analyzeFormalList(environment typeEnv, environment functionEnv, environment varEnv, ASTformalList formals){
  if(!formals) return NULL;
  typeList head = analyzeFormalList(typeEnv, functionEnv, varEnv, formals->rest);
  envEntry formType = analyzeFormal( typeEnv, functionEnv, varEnv, formals->first);
  return TypeList(formType->u.typeEntry.typ, head);
}

/* Need to check for return statement !!! */
AATstatement analyzeFunction(environment typeEnv, environment functionEnv, environment varEnv, ASTfunctionDec function){

  switch(function->kind){
    case Prototype:
    {
      envEntry retType = find(typeEnv, function->u.prototype.returntype);
      if( !retType ) Error(function->line, " %s is not a type", function->u.prototype.returntype);
      typeList formalList = analyzeFormalList( typeEnv, functionEnv ,  varEnv,  function->u.prototype.formals);
      if(retType)
        enter( functionEnv, function->u.prototype.name, FunctionEntry(retType->u.typeEntry.typ, formalList,
          NewNamedLabel(function->u.prototype.name), NewLabel()));
    }
    break;
    case FunctionDef:
    {
      /* analyze function signature (formals). if not found add to function environment*/
      envEntry funType = find(functionEnv, function->u.prototype.name );
      if ( !funType ){
        envEntry retType = find(typeEnv, function->u.prototype.returntype);
        if( !retType ) Error(function->line, " %s is not a type", function->u.prototype.returntype);
        typeList formalList = analyzeFormalList( typeEnv, functionEnv ,  varEnv,  function->u.prototype.formals);
        if(retType)
          enter( functionEnv, function->u.prototype.name, FunctionEntry(retType->u.typeEntry.typ, formalList,
            NewNamedLabel(function->u.prototype.name), NewLabel()));
        funType = find(functionEnv, function->u.prototype.name );
      }
      typeList formalList = funType->u.functionEntry.formals;

      beginScope(varEnv);
      visitFormals( typeEnv, varEnv, formalList, function, function->u.functionDef.formals );
      RETURN_FLAG = NO_RETURN_TYPE;
      GLOBfunctPtr = NULL;
      GLOBfunctPtr = find(functionEnv, function->u.functionDef.name);
      AATstatement stmPtr = visitStatementList(typeEnv, functionEnv, varEnv, function->u.functionDef.body);
      if( !RETURN_FLAG && GLOBfunctPtr->u.functionEntry.returntyp != VoidType() || RETURN_FLAG == NON_VOID_TYPE)
        Error(function->line, " Function does not return type %s", function->u.prototype.returntype);
      //GLOBfunctPtr = NULL;
      //RETURN_FLAG = NO_RETURN_TYPE;
      //offset = 0;
      arm64endScope( functionStack , endScope(varEnv));
      addMemSizes(functionStack, getMemTotals());
      endScope(varEnv);
      resetMemTotals();//need to call this at the end of analyzing stack memory
      AATseqStmCleanUp( stmPtr );
      return functionDefinition(AATpop(), generateStackMemory(functionStack), GLOBfunctPtr->u.functionEntry.startLabel, GLOBfunctPtr->u.functionEntry.endLabel);
    }
    break;
  }
}
/* how to reduce function arguments?!!!!!! <------------------------- */
/* todo: this function produces too many errors. How to isolate error output
* to only one error? <----------------------------------------------- */
AATexpressionList _analyzeCallExp(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp, ASTexpressionList expList, typeList formalList){
  AATexpressionList actuals;
  expressionRec expRec;
  if (!expList && formalList || expList && !formalList){
    Error(exp->line, " arguments differ from function definition");
  }
  if ( !expList && !formalList ) return NULL;
  if( expList && formalList )
    actuals = _analyzeCallExp(typeEnv, functionEnv, varEnv, exp, expList->rest, formalList->rest);
  else if ( !expList && formalList)
    actuals = _analyzeCallExp(typeEnv, functionEnv, varEnv, exp, NULL, formalList->rest);
  else if ( expList && !formalList )
    actuals = _analyzeCallExp(typeEnv, functionEnv, varEnv, exp, expList->rest, NULL);
  else 
    actuals = _analyzeCallExp(typeEnv, functionEnv, varEnv, exp, NULL, NULL);
  if(expList && formalList){
    expRec = analyzeExpression(typeEnv, functionEnv, varEnv, expList->first);
    if (  expRec.typ != formalList->first)
      Error( exp->line, " %s expression does not match function definition", exp->u.callExp.name);
      return ActualList(ConstantExpression(0), actuals);
  }else if ( !expList && formalList){
    Error( exp->line, " formals do not match function prototype" );
    return ActualList(ConstantExpression(0), actuals);
  }else if ( expList && !formalList){
    Error( exp->line, " formals do not match function prototype" );
    return ActualList(ConstantExpression(0), actuals);
  }
  return ActualList(expRec.tree, actuals);
}

/* todo: consider switching this function from recursive to iterative like analyzeCallStm */
expressionRec analyzeCallExp(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp){
  /*use post order recursive decent to test each ExpList exp to typeList type*/
  envEntry function = find( functionEnv, exp->u.callExp.name);
  if(!function){
    Error(exp->line," %s function not defined", exp->u.callExp.name);
    return ExpressionRec(NULL, ConstantExpression(0));
  }
  AATexpressionList actuals = _analyzeCallExp( typeEnv, functionEnv, varEnv, exp, exp->u.callExp.actuals,
    function->u.functionEntry.formals );
  if (! function->u.functionEntry.returntyp ){
    Error(exp->line," return type is not a defined type");
    return ExpressionRec(NULL, ConstantExpression(0));
  }
  if( function->u.functionEntry.returntyp == VoidType())
    Error(exp->line," %s void type can not be used as a expression", exp->u.callExp.name);
  return ExpressionRec( function->u.functionEntry.returntyp, CallExpression(actuals , function->u.functionEntry.startLabel));
}

void analyzeCallStm(environment typeEnv, environment functionEnv, environment varEnv, ASTstatement statement){
  typeList formalList;
  ASTexpressionList actualList;
  envEntry function = find( functionEnv, statement->u.callStm.name);
  if(!function){
    Error(statement->line," %s function not defined", statement->u.callStm.name);
    return;
  }
  formalList = function->u.functionEntry.formals;
  actualList = statement->u.callStm.actuals;
  while( formalList && actualList ){

    if (formalList->first != analyzeExpression(typeEnv, functionEnv, varEnv, actualList->first).typ){
      switch (formalList->first->kind){
        case integer_type:
          Error(actualList->line, " Acutal type does not match formal int type");
          break;
        case boolean_type: 
          Error(actualList->line, " Acutal type does not match formal boolean type");
          break;
        case void_type:
          Error(actualList->line, " Acutal type does not match formal void type");
	      case class_type:
          Error(actualList->line, " Acutal type does not match formal class type");
          break;
        case  array_type:
          Error(actualList->line, " Acutal type does not match formal array type");
          break;
        default:
          Error(actualList->line, " Bad statement");
      }
    }
    formalList = formalList->rest;
    actualList = actualList->rest;
  }
  if( formalList != NULL || actualList != NULL)
    Error(statement->line, " Number of actuals differs from formals");
}

AATstatement analyzeStatement(environment typeEnv, environment functionEnv, environment varEnv, ASTstatement statement) {
  switch(statement->kind) {
    case BlockStm:
    {
      int thisForStmFlag = forStmFlag;
      if( !thisForStmFlag )
        beginScope(varEnv);
      else
        forStmFlag = OFF;
      AATstatement stmPtr = visitStatementList(typeEnv, functionEnv, varEnv, statement->u.blockStm.statements);
      AATseqStmCleanUp( stmPtr );
      if( !thisForStmFlag )
        arm64endScope(functionStack, endScope(varEnv));
      return AATpop();
    }
    break;
    case VarDecStm:
    {

      /* needs some work !!! */
      envEntry varType = find(typeEnv, statement->u.varDecStm.type), varScope;
      if ( varType ){ 
        if( statement->u.varDecStm.arraydimension ){
          /* check for array type and enter into type env */
          varType = enterArrayTypesDectStm(typeEnv, varType, statement);
        }
        /* check if variable is already declared in this scope */
        varScope = find(varEnv, statement->u.varDecStm.name);
        if(varScope && varScope->u.varEntry.scope == getScope(varEnv))
          Error(statement->line, "redefinition of '%s'", statement->u.varDecStm.name);


        /* enter variable here */
        enter( varEnv, statement->u.varDecStm.name, VarEntry( varType->u.typeEntry.typ, varType->u.typeEntry.typ->size_type ) );
       // offset += varType->u.typeEntry.typ->size_type;
        envEntry LHS = find(varEnv, statement->u.varDecStm.name);
        enter_arm64(functionStack, LHS->u.varEntry.scope, LHS->u.varEntry.offset);

        if( statement->u.varDecStm.init ){/* check initialization types match */
          expressionRec RHS;
          RHS = analyzeExpression(typeEnv,functionEnv,varEnv, statement->u.varDecStm.init);
          if (LHS->u.varEntry.typ != RHS.typ) {
            Error(statement->line," Type mismatch on assignment");
          }
          return AssignmentStatement( BaseVariable(LHS->u.varEntry.offset), RHS.tree, LHS->u.varEntry.typ->size_type );
        }
        return AssignmentStatement(BaseVariable(LHS->u.varEntry.offset), EmptyStatement(), LHS->u.varEntry.typ->size_type );
      }else{
        Error(statement->line," %s type not defined", statement->u.varDecStm.type);
        return AssignmentStatement(EmptyStatement(), EmptyStatement(), 0);
      }
    }
    break;
  case AssignStm:
    { 
      expressionRec LHS, RHS;
      LHS = analyzeVar(typeEnv,functionEnv,varEnv,statement->u.assignStm.lhs);
      RHS = analyzeExpression(typeEnv,functionEnv,varEnv,statement->u.assignStm.rhs);
      if (LHS.typ != RHS.typ) {
	      Error(statement->line," Type mismatch on assignment");
      }
      return AssignmentStatement(LHS.tree, RHS.tree, LHS.typ->size_type);
    }
    break;
  case IfStm:
    {
      AATstatement thenTree, elseTree;
      expressionRec test = analyzeExpression(typeEnv, functionEnv, varEnv,
				    statement->u.ifStm.test);
      if (test.typ != BooleanType()) {
	      Error(statement->line," If test must be a boolean");
      }
      thenTree = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.ifStm.thenstm);
      if (statement->u.ifStm.elsestm != NULL)
	      elseTree = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.ifStm.elsestm);
      else
        elseTree = NULL;
      return IfStatement(test.tree, thenTree, elseTree);
    }
    break;
  case CallStm:
    {
     //return analyzeCallStm(typeEnv, functionEnv, varEnv, statement);
    }
    break;
  case ForStm:
    {
      forStmFlag = ON;
      beginScope(varEnv);
      AATstatement init = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.forStm.initialize);
      expressionRec test = analyzeExpression(typeEnv, functionEnv, varEnv, statement->u.forStm.test);
      if (test.typ != BooleanType())
        Error(statement->line, " For test must be a boolean");
      AATstatement increment = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.forStm.increment);
      AATstatement body = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.forStm.body);
      arm64endScope(functionStack, endScope(varEnv));
      forStmFlag = OFF;
      return ForStatement(init, test.tree, increment, body);
    }
    break;
  case WhileStm:
    {
      expressionRec test = analyzeExpression(typeEnv, functionEnv, varEnv,
				    statement->u.whileStm.test);
      if (test.typ != BooleanType()) {
	      Error(statement->line," While test must be a boolean");
      }
      AATstatement body = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.whileStm.body);
      return WhileStatement(test.tree, body);
    }
    break;
  case DoWhileStm:
    {
      AATstatement body = analyzeStatement(typeEnv, functionEnv, varEnv, statement->u.doWhileStm.body);
      expressionRec test = analyzeExpression(typeEnv, functionEnv, varEnv,
				    statement->u.doWhileStm.test);
      if ( test.typ != BooleanType() )
        Error(statement->line, " Do while test must be a boolean");
      return DoWhileStatement(test.tree, body);
    }
    break;
	case ReturnStm:
    {
      expressionRec expType = analyzeExpression(typeEnv, functionEnv, varEnv, statement->u.returnStm.returnval);
      if( GLOBfunctPtr->u.functionEntry.returntyp == expType.typ )
        RETURN_FLAG = CORRECT_RETURN_TYPE;
      if ( GLOBfunctPtr->u.functionEntry.returntyp == VoidType() ){
        if (statement->u.returnStm.returnval->kind == IntLiteralExp && 
            statement->u.returnStm.returnval->u.intLiteralExp.value != 0 
            || expType.typ != IntegerType() )
              RETURN_FLAG = NON_VOID_TYPE;
      }
      return ReturnStatement( expType.tree, GLOBfunctPtr->u.functionEntry.endLabel);
    }
    break; 
  case EmptyStm:
    {
      return EmptyStatement();
    }
    break;
  default:
    Error(statement->line," Bad Statement");
  }
}

expressionRec analyzeNewExp(environment typeEnv, ASTexpression exp){
    envEntry expType = find(typeEnv, exp->u.newExp.name);
    if( !expType ) return ExpressionRec( NULL, ConstantExpression(0));
    if( expType->u.typeEntry.typ->kind != class_type) return ExpressionRec( NULL, ConstantExpression(0));
    /*need to update envSize function to calculate correct memory size*/
    return ExpressionRec( expType->u.typeEntry.typ, Allocate( 
      ConstantExpression( envSize(expType->u.typeEntry.typ->u.class.instancevars) )));
}

expressionRec analyzeNewArray(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp){
#define keyLen 101
  char key[keyLen];
  envEntry expType;
  expressionRec expRec;
  /*check to make sure size is less than key */
  if( strlen(exp->u.newArrayExp.name)+1 >= keyLen)
    Error(exp->line, " Array type %s must be less than 100 characters", exp->u.newArrayExp.name);
  else
    sprintf(key,"%s", exp->u.newArrayExp.name);
  for(int i = 0; i < exp->u.newArrayExp.arraydimension; i++){
    if( strlen(key)+1 + 2 >= keyLen) // + 2 for the "[]" that are being added
      Error(exp->line, " Array type %s must be less than 100 characters with included []'s", exp->u.newArrayExp.name);
    else
      sprintf(key, "%s%s", key, "[]");
  }
  expRec = analyzeExpression(typeEnv, functionEnv, varEnv, exp->u.newArrayExp.size);
  if( expRec.typ != IntegerType())
    Error(exp->line, " Array size must be an integer");
  expType = find(typeEnv, key);
  if( !expType ) return ExpressionRec(NULL, ConstantExpression(0));
  return ExpressionRec(expType->u.typeEntry.typ, Allocate(OperatorExpression(
    exp->u.newArrayExp.size, ConstantExpression(expType->u.typeEntry.typ->u.array->size_type), AAT_MULTIPLY)));
}

expressionRec analyzeExpression(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp) {
  switch(exp->kind) {
  case IntLiteralExp:
    return ExpressionRec( IntegerType(), ConstantExpression(exp->u.intLiteralExp.value));
  case BoolLiteralExp:
    return ExpressionRec( BooleanType(), ConstantExpression(exp->u.boolLiteralExp.value));
  case OpExp:
    return analyzeOpExpression(typeEnv,functionEnv, varEnv, exp);
  case VarExp:
    return analyzeVar(typeEnv,functionEnv,varEnv,exp->u.varExp.var);
  case CallExp:
    return  analyzeCallExp(typeEnv, functionEnv, varEnv, exp);
  case NewExp:
    return analyzeNewExp(typeEnv, exp);
  case NewArrayExp:
    return analyzeNewArray(typeEnv, functionEnv, varEnv, exp);
  default:
    Error(exp->line," Bad Expression");
    return ExpressionRec( IntegerType(), ConstantExpression(0));
  }
}

AATexpression analyzeIndex(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression index){
  expressionRec indexRec = analyzeExpression(typeEnv, functionEnv, varEnv, index);
  if ( indexRec.typ  != IntegerType() )
    Error(index->line," Expression type is not integer type");
  return indexRec.tree;
}

/* should I make error types return ExpressionRec(IntegerType(),NULL);? */
expressionRec analyzeVar(environment typeEnv, environment functionEnv, environment varEnv, ASTvariable var) {
  envEntry baseEntry;
  expressionRec baseType;
  switch(var->kind){
    case BaseVar:
      baseEntry = find(varEnv, var->u.baseVar.name);
      if ( !baseEntry ){
        /* return ExpressionRec(IntegerType(),NULL); */
        Error(var->line," Variable %s not defined",var->u.baseVar.name);
        return ExpressionRec(NULL, ConstantExpression(0));
      }
      return ExpressionRec(baseEntry->u.varEntry.typ, BaseVariable(baseEntry->u.varEntry.offset));
      break;
    case ArrayVar:
      baseType = analyzeVar(typeEnv, functionEnv, varEnv, var->u.arrayVar.base);
      AATexpression indexExp = analyzeIndex(typeEnv, functionEnv, varEnv, var->u.arrayVar.index);
      /*ToDo: condense these if statements down to 1 for 1 error message */
      if(!baseType.typ){ 
        Error(var->line," Variable type is not array type");
        /* return ExpressionRec(IntegerType(),NULL); */
        return ExpressionRec(NULL, ConstantExpression(0));
      }
      if(baseType.typ->kind != array_type){
        Error(var->line," Variable type is not array type");
        /* return ExpressionRec(IntegerType(),NULL); */
        return ExpressionRec(NULL, ConstantExpression(0));
      }
      return ExpressionRec(baseType.typ->u.array, ArrayVariable(baseType.tree, indexExp, baseType.typ->size_type)); 
    break;
    case ClassVar:
      baseType = analyzeVar(typeEnv, functionEnv, varEnv, var->u.classVar.base);
      if (baseType.typ && baseType.typ->kind == class_type){
        baseEntry = find(baseType.typ->u.class.instancevars, var->u.classVar.instance);
        if ( !baseEntry ){
          Error(var->line," Variable %s not defined", var->u.classVar.instance);
          /* return ExpressionRec(IntegerType(),NULL); */
          return ExpressionRec(NULL, ConstantExpression(0));
        }
        return ExpressionRec(baseEntry->u.varEntry.typ, ClassVariable(baseType.tree, *baseEntry->u.varEntry.offset));
      }else{
        /* return ExpressionRec(IntegerType(),NULL); */
        return ExpressionRec(NULL, ConstantExpression(0));
      }
    break;
    default:
    Error(var->line," Malformed Variable");
  }
}

 /**********************************************************************************************/
/*###------>TODO-------->Need to add pointer (class and array) equality checks<--------------###*/
 /*     add checking for char types and pointer types (array types)                             */
 /**********************************************************************************************/
expressionRec analyzeOpExpression(environment typeEnv, environment functionEnv, environment varEnv, ASTexpression exp) {
  switch(exp->u.opExp.operator) {
  case AST_PLUS:
    {
      expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
      expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
      if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
	      Error(exp->line," Both arguments to + must be integers");
      return ExpressionRec(IntegerType(),OperatorExpression(LHS.tree,RHS.tree,AAT_PLUS));
    } 
    break;
  case AST_MINUS:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to - must be integers");
    return ExpressionRec(IntegerType(),OperatorExpression(LHS.tree,RHS.tree,AAT_MINUS));
    } 
    break;
  case AST_MULTIPLY:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to * must be integers");
    return ExpressionRec(IntegerType(),OperatorExpression(LHS.tree,RHS.tree,AAT_MULTIPLY));
    } 
    break;
  case AST_DIVIDE:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to / must be integers");
    return ExpressionRec(IntegerType(),OperatorExpression(LHS.tree,RHS.tree,AAT_DIVIDE));
    } 
    break;
  case AST_EQ: /* add pointer equality checking */
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if (LHS.typ == IntegerType() && RHS.typ == IntegerType()) 
      return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_EQ));
    if (LHS.typ == BooleanType() && RHS.typ == BooleanType()) 
      return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_EQ));
    Error(exp->line," Both arguments to == must be the same type");
      return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_EQ));
    } 
    break;
  case AST_NEQ: /* add checking for char type and pointer type */
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if (LHS.typ == IntegerType() && RHS.typ == IntegerType()) 
      return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_NEQ));
    if (LHS.typ == BooleanType() && RHS.typ == BooleanType()) 
      return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_NEQ));
    Error(exp->line," Both arguments to == must be the same type");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_NEQ));
    break;
    }
  case AST_LT:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to < must be integers");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_LT));
    } 
    break;
  case AST_LEQ:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to <= must be integers");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_LEQ));
    } 
    break;
  case AST_GT:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to > must be integers");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_GT));
    } 
    break;
  case AST_GEQ:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != IntegerType())) 
      Error(exp->line," Both arguments to >= must be integers");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_GEQ));
    } 
    break;
  case AST_AND:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != BooleanType()) || (RHS.typ != BooleanType()))
      Error(exp->line," Both arguments to || must be boolean");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_AND));
    }
    break;
  case AST_OR:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != BooleanType()) || (RHS.typ != BooleanType()))
      Error(exp->line," Both arguments to || must be boolean");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_OR));
    } 
    break;
  case AST_NOT:
    {
    expressionRec LHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.left);
    expressionRec RHS = analyzeExpression(typeEnv,functionEnv,varEnv,exp->u.opExp.right);
    if ((LHS.typ != IntegerType()) || (RHS.typ != BooleanType())) 
      Error(exp->line," Argument to ! must be boolean");
    return ExpressionRec(BooleanType(),OperatorExpression(LHS.tree,RHS.tree,AAT_NOT));
    } 
    break;
  default:
    Error(exp->line," Bad Operator expression");
  }
}