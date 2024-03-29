/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/
#include "../lib/offset_ref.h"

typedef struct AATexpression_ *AATexpression;
typedef struct AATstatement_ *AATstatement;
typedef struct AATexpressionList_ *AATexpressionList;
typedef char *Label;

/*
typedef enum {AAT_PLUS, AAT_MINUS, AAT_MULTIPLY, AAT_DIVIDE, AAT_LT,
	      AAT_GT, AAT_LEQ, AAT_GEQ, AAT_EQ, AAT_NEQ, AAT_AND, AAT_OR,
              AAT_NOT} AAToperator;
              */

/*statements*/
AATstatement functionDefinition(AATstatement body, int framesize, label_ref start, label_ref end);
AATstatement IfStatement(AATexpression test, AATstatement ifbody, AATstatement elsebody);
AATstatement WhileStatement(AATexpression test, AATstatement whilebody);
AATstatement DoWhileStatement(AATexpression test, AATstatement dowhilebody);
AATstatement ForStatement(AATstatement init, AATexpression test, AATstatement increment, AATstatement body);
AATstatement EmptyStatement(void);
AATstatement CallStatement(AATexpressionList actuals, label_ref name, int argMemSize);
AATstatement AssignmentStatement(AATexpression lhs, AATexpression rhs, int size);
AATstatement SequentialStatement(AATstatement first, AATstatement second);
AATstatement ReturnStatement(AATexpression value, label_ref functionend, int size_type);

/*expressions*/
AATexpression Allocate(AATexpression size);
AATexpression BaseVariable(offset_ref offset, int size_type);
AATexpression ArrayVariable(AATexpression base, AATexpression index, int elementSize, int size_type);
AATexpression ClassVariable(AATexpression base, int offset, int size_type);
AATexpression ConstantExpression(int value, int size_type);
AATexpression OperatorExpression(AATexpression left, AATexpression right, AAToperator operator, int size_type);
AATexpression CallExpression(AATexpressionList actuals, label_ref name, int size_type, int argMemSize);

AATexpressionList ActualList(AATexpression first, AATexpressionList rest, int size_type, int offset);