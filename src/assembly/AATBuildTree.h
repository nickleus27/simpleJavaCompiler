/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

typedef struct AATexpression_ *AATexpression;
typedef struct AATstatement_ *AATstatement;
typedef struct AATexpressionList_ *AATexpressionList;
typedef char *Label;
/*
typedef enum {AAT_PLUS, AAT_MINUS, AAT_MULTIPLY, AAT_DIVIDE, AAT_LT,
	      AAT_GT, AAT_LEQ, AAT_GEQ, AAT_EQ, AAT_NEQ, AAT_AND, AAT_OR,
              AAT_NOT} AAToperator;
              */

AATstatement functionDefinition(AATstatement body, int framesize, Label start, Label end);
AATstatement IfStatement(AATexpression test, AATstatement ifbody, AATstatement elsebody);
AATstatement WhileStatement(AATexpression test, AATstatement whilebody);
AATstatement DoWhileStatement(AATexpression test, AATstatement dowhilebody);
AATstatement ForStatement(AATstatement init, AATexpression test, AATstatement increment, AATstatement body);
AATstatement EmptyStatement(void);
AATstatement CallStatement(AATexpressionList actuals, Label name);
AATstatement AssignmentStatement(AATexpression lhs, AATexpression rhs, int size);
AATstatement SequentialStatement(AATstatement first, AATstatement second);
AATstatement ReturnStatement(AATexpression value, Label functionend);

AATexpression Allocate(AATexpression size);
AATexpression BaseVariable(int* offset);
AATexpression ArrayVariable(AATexpression base, AATexpression index, int elementSize);
AATexpression ClassVariable(AATexpression base, int offset);
AATexpression ConstantExpression(int value);
AATexpression OperatorExpression(AATexpression left, AATexpression right, AAToperator operator);
AATexpression CallExpression(AATexpressionList actuals, Label name);

AATexpressionList ActualList(AATexpression first, AATexpressionList rest);