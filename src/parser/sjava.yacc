/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

%{
#include <stdio.h>
#include <stdlib.h>
#include "../errors/errors.h"  
#include "../parser/AST.h"
#include "lex.yy.h"

void yyerror(char *s) {
    Error(Current_Line,"%s\n",s);  
}

typedef enum {AST_INC, AST_DEC}AST_INC_DEC;

ASTvariable ASTclassArray(ASTvariable first, ASTvariable third){
    ASTvariable current = third->u.arrayVar.base, prev = third; 
    while(current->kind != BaseVar){
        prev = current;
        current = current->u.arrayVar.base;
    } 
    prev->u.arrayVar.base = ASTClassVar(first->line, first, current->u.baseVar.name); 
    free(current); 
    return third;
}

//helper function for var++ or var--
//this clones the variable so that memory can be deleted in tree traversal in semantic.c
ASTstatement AST_inc_dec_asgn (ASTvariable var, AST_INC_DEC inc_dec) {
    ASTvariable dupVar;
    switch(var->kind){
        case BaseVar:
        {
            printf("BASE VAR DUP FOR ++ OR --\n");
            dupVar = ASTBaseVar(var->line, var->u.baseVar.name);
            break;
        }
        case ClassVar:
        {
            printf("CLASS VAR DUP FOR ++ OR --\n");
            break;
        }
        case ArrayVar:
        {
            printf("ARRAY VAR DUP FOR ++ OR --\n");
            break;
        }
        default: printf("BAD VARIABLE TYPE FOR A ++ OR -- STATEMENT\n");
    }
    if(inc_dec == AST_INC) {
        return ASTAssignStm(var->line, var, ASTOpExp( var->line, AST_PLUS, ASTVarExp( var->line, dupVar), ASTIntLiteralExp( var->line, 1)));
    }
    return ASTAssignStm(var->line, var, ASTOpExp( var->line, AST_MINUS, ASTVarExp( var->line, dupVar), ASTIntLiteralExp( var->line, 1)));
}
%}


%union { 
     struct {
         int value;
         int line_number;
     } integer_value;
     struct {
         char *value;
         int line_number;
     } string_value;
     int line_number;

    ASTprogram Program;
    ASTclassList ClassList;
    ASTfunctionDecList FunctionDecList;
    ASTclass Class;
    ASTfunctionDec FunctionDec;
    ASTinstanceVarDecList InstanceVarDecList;
    ASTinstanceVarDec InstanceVarDec;
    ASTformalList FormalList;
    ASTformal Formal;
    ASTstatementList StatementList;
    ASTstatement Statement;
    ASToperator Operator;
    ASTexpressionList ExpressionList;
    ASTexpression Expression;
    ASTvariable Variable;
  
}

%expect 1 /* Dangling else ambiguity. We attach the else statement to inner-most if statement. */

%type <ClassList> class_lst
%type <FunctionDecList> funct_lst
%type <Class> a_class
%type <FunctionDec> prototype funct_def
%type <InstanceVarDecList>  instnc_var_list
%type <InstanceVarDec> instnc_var
%type <FormalList> formal_lst
%type <Formal>  formal
%type <StatementList> stmnt_lst
%type <Statement> asn_stmnt call_stmt wil_stmnt blck_stmt varDecStmt for_stmnt for_init empty_stm
%type <Statement> for_inc_dec plus_plus min_min a_stmnt if_e_stmt rtrn_stmt lhs_seq do_while arrDec
%type <ExpressionList> exp_lst
%type <Expression> exp new_arr_exp new_exp
%type <Variable> var class_var arr_var cls_arr nd_arr var_types
%type <string_value> prim_typ arr_typ nd_ar_tp ret_typ

%token <string_value> IDENTIFIER
/* todo(Char): add token <string_value> for CHAR & STRING */
%token <integer_value> INTEGER_LITERAL
%token <line_number> CLASS DO ELSE TOKENTRUE TOKENFALSE FOR IF  WHILE PLUS 
       MINUS MULTIPLY DIVIDE LBRACK RBRACK LBRACE
       RBRACE LPAREN RPAREN DOT COMMA SEMICOLON
       EQ NEQ LT GT LEQ GEQ GETS AND OR NOT PLUSPLUS
       MINUSMINUS RETURN NEW GLOB TOKENNULL
       UMINUS UNOT

%left OR
%left AND
%nonassoc EQ NEQ LT GT LEQ GEQ 
%left PLUS MINUS
%left MULTIPLY DIVIDE
%left UMINUS UNOT

%start program

%%
/*  TODO:   Following are steps for addition of global vars, chars, and strings.    */
/*  Need to create a tree for global variables.                                                                             *
**  Program will branch: left = dataSeg; right = textSeg;                                                                   *
**  AST nodes will need to be created for ASTData and ASTText and ASTProgram will be changed appropriately.                 *
**  AST nodes for ASTdataList and ASTdataVar will need to be created.                                                       *
**  A global variable that points to top of ASTdataList stack will be kept in this yacc files global C code section or      *
**  As an external global variable in AST.c.                                                                                *
**  AST.h needs CharLiteralExp and StcStrExp kinds added to  ASTexpression_ and appropriate structs.                        *
**  New regex need to be added to lexer file and token macros to token.h make sure macro matches enum order in this file.   *
**  static strings (stc_str) can use the string_value token type ane char can use integer_literal type.                     *
**  stc_str types will use char[] type in environments in semantic.c.                                                       */

program: class_lst funct_lst YYEOF { ASTroot = ASTProgram(1, $1, $2); }

/*function rules */
funct_lst:                                              { $$ = NULL; }
|               funct_lst   prototype                   { $$ = ASTFunctionDecList($2->line, $2, $1); }
|               funct_lst   funct_def                   { $$ = ASTFunctionDecList($2->line, $2, $1); }

/* function return types */
/*
** TODO: need to add functionality for return array types
** Look to line 211 for arr delcaration example after
** adding int arrayDimension field to ASTFunctionDef and ASTPrototype
** add array checking in analyzeFunction in semantic.c
 **/
prim_typ:       IDENTIFIER                              { $$ = $1; }
arr_typ:        prim_typ LBRACK RBRACK                  { $$ = $1; }
nd_ar_tp:       arr_typ LBRACK RBRACK                   { $$ = $1; }
|               nd_ar_tp LBRACK RBRACK                  { $$ = $1; }
ret_typ:        prim_typ                                { $$ = $1; }
|               arr_typ                                 { $$ = $1; }
|               nd_ar_tp                                { $$ = $1; }

prototype:  ret_typ IDENTIFIER LPAREN formal_lst RPAREN SEMICOLON                { $$ = ASTPrototype($1.line_number, $1.value, $2.value, $4); }
funct_def:  ret_typ IDENTIFIER LPAREN formal_lst RPAREN LBRACE stmnt_lst RBRACE  { $$ =  ASTFunctionDef($1.line_number, $1.value, $2.value, $4, $7); }
formal_lst:                                             { $$ = NULL;}
|               { $<FormalList>$ = NULL; } formal       { $$ = ASTFormalList($2->line, $2, $<FormalList>1);}
|               formal_lst  COMMA formal                { $$ = ASTFormalList($3->line, $3, $1); }
formal:         IDENTIFIER IDENTIFIER                   { $$ = ASTFormal($1.line_number, $1.value, $2.value, 0); }
|               formal LBRACK RBRACK                    { $$ = $1; $1->arraydimension++;}
/* need to add nd array for formals */ 

/* class rules */
class_lst:  class_lst    a_class                        { $$ = ASTClassList($2->line, $2, $1); }
|                                                       { $$ = NULL; }
a_class:   CLASS IDENTIFIER instnc_var_list RBRACE      { $$ = ASTClass($1, $2.value, $3); }
 
/* variable rules */
/* class instance variables */
instnc_var_list: LBRACE                                 { $$ = NULL; }
|               instnc_var_list instnc_var SEMICOLON    { $$ = ASTInstanceVarDecList($2->line, $2, $1); }
instnc_var:     IDENTIFIER IDENTIFIER                   { $$ = ASTInstanceVarDec($1.line_number, $1.value, $2.value, 0); }
|               instnc_var LBRACK RBRACK                { $$ = $1; $1->arraydimension++;}
/* variable trees */
class_var:      var DOT var                             { $$ = ASTClassVar($1->line, $1, $3->u.baseVar.name); free($3); }
|               class_var DOT var                       { $$ = ASTClassVar($1->line, $1, $3->u.baseVar.name); free($3); }
|               cls_arr DOT var                         { $$ = ASTClassVar($1->line, $1, $3->u.baseVar.name); free($3); }          
arr_var:        var LBRACK exp RBRACK                   { $$ = ASTArrayVar($1->line, $1, $3); }
cls_arr:        var DOT arr_var                         { $$ = ASTclassArray($1, $3); }
|               class_var DOT arr_var                   { $$ = ASTclassArray($1, $3); }
|               arr_var DOT arr_var                     { $$ = ASTclassArray($1, $3); }
|               cls_arr DOT arr_var                     { $$ = ASTclassArray($1, $3); }
|               arr_var DOT var                         { $$ = ASTClassVar($1->line, $1, $3->u.baseVar.name); free($3); }
|               nd_arr DOT var                          { $$ = ASTClassVar($1->line, $1, $3->u.baseVar.name); free($3); }
|               nd_arr DOT nd_arr                       { $$ = ASTclassArray($1, $3); }
|               var DOT nd_arr                          { $$ = ASTclassArray($1, $3); }
|               class_var DOT nd_arr                    { $$ = ASTclassArray($1, $3); }
|               cls_arr DOT nd_arr                      { $$ = ASTclassArray($1, $3); }
|               arr_var DOT nd_arr                      { $$ = ASTclassArray($1, $3); }
|               nd_arr DOT arr_var                      { $$ = ASTclassArray($1, $3); }
nd_arr:         arr_var LBRACK exp RBRACK               { $$ = ASTArrayVar($1->line, $1, $3); }
|               nd_arr  LBRACK exp RBRACK               { $$ = ASTArrayVar($1->line, $1, $3); }
var:            IDENTIFIER                              { $$ = ASTBaseVar( $1.line_number, $1.value ); }
var_types:  var
|           cls_arr
|           arr_var
|           class_var
|           nd_arr

/* statement rules */
stmnt_lst:                                              { $$ = NULL; }
|           stmnt_lst a_stmnt                           { $$ = ASTStatementList( $2->line, $2, $1); }
a_stmnt:    asn_stmnt
|           call_stmt
|           if_e_stmt
|           wil_stmnt
|           varDecStmt
|           for_stmnt
|           blck_stmt
|           plus_plus
|           min_min
|           rtrn_stmt
|           do_while
|           empty_stm
call_stmt:  IDENTIFIER LPAREN exp_lst RPAREN SEMICOLON  { $$ = ASTCallStm($1.line_number, $1.value, $3); }
asn_stmnt:  var_types GETS exp SEMICOLON                { $$ = ASTAssignStm ( $1->line, $1, $3 ); }
if_e_stmt:   IF LPAREN exp RPAREN a_stmnt               { $$ = ASTIfStm($1, $3, $5, NULL); } 
|            IF LPAREN exp RPAREN a_stmnt ELSE a_stmnt  { $$ = ASTIfStm($1, $3, $5, $7); }
wil_stmnt:  WHILE LPAREN exp RPAREN a_stmnt             { $$ = ASTWhileStm($1, $3, $5); }
for_stmnt:  FOR LPAREN for_init exp SEMICOLON for_inc_dec RPAREN a_stmnt {$$=ASTForStm($1, $3, $4, $6, $8);}
for_init:   varDecStmt
|           asn_stmnt
for_inc_dec:    asn_stmnt
|           var_types PLUSPLUS    { $$ = AST_inc_dec_asgn ($1, AST_INC); }
|           var_types MINUSMINUS  { $$ = AST_inc_dec_asgn ($1, AST_DEC); }
plus_plus:  var_types PLUSPLUS SEMICOLON    {$$ = AST_inc_dec_asgn ($1, AST_INC); }
min_min:    var_types MINUSMINUS SEMICOLON  { $$ = AST_inc_dec_asgn ($1, AST_DEC); }
lhs_seq:    IDENTIFIER IDENTIFIER                       { $$ = ASTVarDecStm($1.line_number, $1.value, $2.value, 0, NULL); }
arrDec:     lhs_seq LBRACK RBRACK                       { $$ = $1; $1->u.varDecStm.arraydimension++; }
|           arrDec LBRACK RBRACK                        { $$ = $1; $1->u.varDecStm.arraydimension++; }
varDecStmt: lhs_seq SEMICOLON                           { $$ = $1; }
|           arrDec SEMICOLON                            { $$ = $1; }
|           lhs_seq GETS exp SEMICOLON                  { $$ = $1; $1->u.varDecStm.init = $3; }
|           arrDec GETS exp SEMICOLON                   { $$ = $1; $1->u.varDecStm.init = $3; }
do_while:   DO a_stmnt WHILE LPAREN exp RPAREN SEMICOLON { $$ = ASTDoWhileStm($1, $5, $2); }
empty_stm:  SEMICOLON                                   { $$ = ASTEmptyStm($1); }
blck_stmt:  LBRACE stmnt_lst RBRACE                     { $$ = ASTBlockStm($1, $2); } 
rtrn_stmt:  RETURN exp SEMICOLON                        { $$ = ASTReturnStm($1, $2); }
|           RETURN SEMICOLON                            { $$ = ASTReturnStm($1, ASTIntLiteralExp( $1, 0)); }

/* expression rules */
/* AST_EQ, AST_NEQ, AST_LT, AST_GT, AST_LEQ, AST_GEQ, AST_AND,
	      AST_OR, AST_NOT, AST_PLUS, AST_MINUS, AST_MULTIPLY,
              AST_DIVIDE */
new_arr_exp: NEW IDENTIFIER LBRACK exp RBRACK   { $$ = ASTNewArrayExp($1, $2.value, $4, 1); }
|        new_arr_exp LBRACK RBRACK              { $$ = $1; $1->u.newArrayExp.arraydimension++; }
new_exp: NEW IDENTIFIER LPAREN RPAREN           { $$ = ASTNewExp($1, $2.value); }
exp_lst:                                    { $$ = NULL; }
|       { $<ExpressionList>$ = NULL; } exp  { $$ = ASTExpressionList($2->line, $2, $<ExpressionList>1); }
|       exp_lst COMMA exp                   { $$ = ASTExpressionList($3->line, $3, $1); }
exp:    exp EQ exp                          { $$ = ASTOpExp( $1->line, AST_EQ, $1, $3 ); }
|       exp NEQ exp                         { $$ = ASTOpExp( $1->line, AST_NEQ, $1, $3 ); }
|       exp LT exp                          { $$ = ASTOpExp( $1->line, AST_LT, $1, $3 ); }
|       exp GT exp                          { $$ = ASTOpExp( $1->line, AST_GT, $1, $3 ); }
|       exp LEQ exp                         { $$ = ASTOpExp( $1->line, AST_LEQ, $1, $3 ); }
|       exp GEQ exp                         { $$ = ASTOpExp( $1->line, AST_GEQ, $1, $3 ); }
|       exp AND exp                         { $$ = ASTOpExp( $1->line, AST_AND, $1, $3 ); }
|       exp OR exp                          { $$ = ASTOpExp( $1->line, AST_OR, $1, $3 ); }
|       exp MINUS exp                       { $$ = ASTOpExp( $1->line, AST_MINUS, $1, $3 ); }
|       exp PLUS exp                        { $$ = ASTOpExp( $1->line, AST_PLUS, $1, $3 ); }
|       exp MULTIPLY exp                    { $$ = ASTOpExp( $1->line, AST_MULTIPLY, $1, $3 ); }
|       exp DIVIDE exp                      { $$ = ASTOpExp( $1->line, AST_DIVIDE, $1, $3 ); }
|       NOT exp %prec UNOT                  { $$ = ASTOpExp( $1, AST_NOT, ASTIntLiteralExp( $2->line, 2), $2 ); }
|       MINUS exp %prec UMINUS              { $$ = ASTOpExp( $1, AST_MINUS, ASTIntLiteralExp( $2->line, 0), $2 ); }
|       var_types                           { $$ = ASTVarExp( $1->line, $1 ); }
|       IDENTIFIER LPAREN exp_lst RPAREN    { $$ = ASTCallExp($1.line_number, $1.value, $3); } //call expression
|       INTEGER_LITERAL                     { $$ = ASTIntLiteralExp( $1.line_number, $1.value ); }
|       TOKENTRUE                           { $$ = ASTBoolLiteralExp($1, 1); }
|       TOKENFALSE                          { $$ = ASTBoolLiteralExp($1, 0); }
|       TOKENNULL                           { $$ = ASTNullExp($1); } 
|       new_arr_exp                         { $$ = $1; }
|       new_exp                             { $$ = $1; }
|       LPAREN exp RPAREN                   { $$ = $2; }
/* todo(char): add charliteral */


