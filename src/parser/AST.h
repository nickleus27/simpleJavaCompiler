/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

typedef struct ASTprogram_ *ASTprogram;
typedef struct ASTclass_ *ASTclass;
typedef struct ASTclassList_ *ASTclassList;
typedef struct ASTinstanceVarDec_ *ASTinstanceVarDec;
typedef struct ASTinstanceVarDecList_ *ASTinstanceVarDecList;
typedef struct ASTfunctionDec_ *ASTfunctionDec;
typedef struct ASTfunctionDecList_ *ASTfunctionDecList;
typedef struct ASTformal_ *ASTformal;
typedef struct ASTformalList_ *ASTformalList;

typedef struct ASTstatement_ *ASTstatement;
typedef struct ASTstatementList_ *ASTstatementList;
typedef struct ASTexpression_ *ASTexpression;
typedef struct ASTvariable_ *ASTvariable;
typedef struct ASTexpressionList_ *ASTexpressionList;

typedef enum {AST_EQ, AST_NEQ, AST_LT, AST_GT, AST_LEQ, AST_GEQ, AST_AND, 
              AST_OR, AST_NOT, AST_PLUS, AST_MINUS, AST_MULTIPLY,
              AST_DIVIDE} ASToperator;

ASTprogram ASTProgram(int line, ASTclassList classes, ASTfunctionDecList functiondecs);
ASTclass ASTClass(int line, char *name, ASTinstanceVarDecList instancevars);
ASTclassList ASTClassList(int line, ASTclass first, ASTclassList rest);
ASTinstanceVarDec ASTInstanceVarDec(int line, char *type, char *name, int arraydimension);
ASTinstanceVarDecList ASTInstanceVarDecList(int line, ASTinstanceVarDec first,
					    ASTinstanceVarDecList rest);
ASTfunctionDec ASTFunctionDef(int line, char *returntype, char *name,
			      ASTformalList formals,
			      ASTstatementList body);
ASTfunctionDec ASTPrototype(int line, char *returntype, char *name,
			    ASTformalList formals);

ASTfunctionDecList ASTFunctionDecList(int line, ASTfunctionDec first,
				      ASTfunctionDecList rest);
ASTformal ASTFormal(int line, char *type, char *name, int arraydimension);
ASTformalList ASTFormalList(int line, ASTformal first, ASTformalList rest);

ASTstatement ASTAssignStm(int line, ASTvariable lhs, ASTexpression rhs);
ASTstatement ASTIfStm(int line, ASTexpression test, ASTstatement thenstm,
		      ASTstatement elsestm);
ASTstatement ASTForStm(int line, ASTstatement initialize, ASTexpression test,
		       ASTstatement increment, ASTstatement body);
ASTstatement ASTWhileStm(int line, ASTexpression test, ASTstatement body);
ASTstatement ASTDoWhileStm(int line, ASTexpression test, ASTstatement body);
ASTstatement ASTVarDecStm(int line, char *type, char *name, int arraydimension, ASTexpression init);
ASTstatement ASTCallStm(int line, char *name, ASTexpressionList actuals);
ASTstatement ASTBlockStm(int line, ASTstatementList statements);
ASTstatement ASTReturnStm(int line, ASTexpression returnval);
ASTstatement ASTEmptyStm(int line);
ASTstatementList ASTStatementList(int line, ASTstatement first, ASTstatementList rest);

ASTexpression ASTIntLiteralExp(int line, int value);
ASTexpression ASTBoolLiteralExp(int line, int value);
ASTexpression ASTOpExp(int line, ASToperator operator, ASTexpression left,
		       ASTexpression right);
ASTexpression ASTVarExp(int line, ASTvariable var);
ASTexpression ASTCallExp(int line, char *name, ASTexpressionList actuals);
ASTexpression ASTNewExp(int line, char *name);
ASTexpression ASTNewArrayExp(int line, char *name, ASTexpression size, int arraydimension);
ASTexpressionList ASTExpressionList(int line, ASTexpression first, ASTexpressionList rest);
ASTvariable ASTBaseVar(int line, char *name);
ASTvariable ASTClassVar(int line, ASTvariable base, char *instance);
ASTvariable ASTArrayVar(int line, ASTvariable base, ASTexpression index);


struct ASTprogram_ {
  int line;
  ASTclassList classes;
  ASTfunctionDecList functiondecs;
};

struct ASTclass_ {
  int line;
  char *name;
  ASTinstanceVarDecList instancevars;
};

struct ASTclassList_ {
  int line;
  ASTclass first;
  ASTclassList rest;
};

struct ASTinstanceVarDec_ {
  int line;
  char *type;
  char *name;
  int arraydimension;
};

struct ASTinstanceVarDecList_ {
  int line;
  ASTinstanceVarDec first;
  ASTinstanceVarDecList rest;
};

struct ASTfunctionDec_ {
  int line;
  enum {Prototype, FunctionDef} kind;
  union {
    struct {
      char *returntype;
      char *name;
      ASTformalList formals;
    } prototype;
    struct {
      char *returntype;
      char *name;
      ASTformalList formals;
      ASTstatementList body;
    } functionDef;
  } u;
};

struct ASTfunctionDecList_ {
  int line;
  ASTfunctionDec first;
  ASTfunctionDecList rest;
};

struct ASTformal_ {
  int line;
  char *type;
  char *name;
  int arraydimension;
};

struct ASTformalList_ {
  int line;
  ASTformal first;
  ASTformalList rest;
};

struct ASTstatement_ {
  int line;
  enum {AssignStm, IfStm, ForStm, WhileStm, DoWhileStm,
	VarDecStm, CallStm, BlockStm, ReturnStm, EmptyStm} kind;
  union {
    struct {
      ASTvariable lhs;
      ASTexpression rhs;
    } assignStm;
    struct {
      ASTexpression test;
      ASTstatement thenstm;
      ASTstatement elsestm;
    } ifStm;
    struct {
      ASTstatement initialize;
      ASTexpression test;
      ASTstatement increment;
      ASTstatement body;
    } forStm;
    struct {
      ASTexpression test;
      ASTstatement body;
    } whileStm;
    struct {
      ASTexpression test;
      ASTstatement body;
    } doWhileStm;
    struct {
      char *name;
      char *type;
      int arraydimension;
      ASTexpression init;
    } varDecStm;
    struct {
      char *name;
      ASTexpressionList actuals;
    } callStm;
    struct {
      ASTstatementList statements;
    } blockStm;
    struct {
      ASTexpression returnval;
    } returnStm;
  } u;
};

struct ASTstatementList_ {
  int line;
  ASTstatement first;
  ASTstatementList rest;
};

struct ASTexpression_ {
  int line;
  enum {IntLiteralExp, BoolLiteralExp, OpExp, VarExp, CallExp, NewExp, NewArrayExp} kind;
  union {
    struct {
      int value;
    } intLiteralExp;
    struct {
      int value;
    } boolLiteralExp;
    struct {
      ASToperator operator;
      ASTexpression left;
      ASTexpression right;
    } opExp;
    struct {
      ASTvariable var;
    } varExp;
    struct {
      char *name;
      ASTexpressionList actuals;
    } callExp;
    struct {
      char *name;
    } newExp;
    struct {
      char *name;
      int arraydimension;
      ASTexpression size;
    } newArrayExp;
  } u;
};

struct ASTexpressionList_ {
  int line;
  ASTexpression first;
  ASTexpressionList rest;
};

struct ASTvariable_ {
  int line;
  enum {BaseVar, ClassVar, ArrayVar} kind;
  union {
    struct {
      char *name;
    } baseVar;
    struct {
      ASTvariable base;
      char *instance;
    } classVar;
    struct {
      ASTvariable base;
      ASTexpression index;
    } arrayVar;
  } u;
};

extern ASTprogram ASTroot;
