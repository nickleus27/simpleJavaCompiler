/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include "label.h"
#include "../codegen/registerArm64.h"
#include "AAT.h"
#include "../codegen/MachineDependent.h"
#include "AATBuildTree.h"
#include <stdlib.h>


/*------------------ Expressions ------------------*/
AATexpression Allocate(AATexpression size) {
  AATexpressionList actuals = AATExpressionList(size, NULL, REG32, 16);
  return AATFunctionCall("allocate",actuals, PTR, 32);
}

AATexpression ClassVariable(AATexpression base, int offset, int size_type){
  if ( offset ) {
    return AATMemory( AATOperator(base, AATConstant(offset, REG64), AAT_PLUS, REG64), size_type);
  }
  else
    return AATMemory( base, size_type ); /* if no offset do not need to subtract from memory */
}

AATexpression ArrayVariable(AATexpression base, AATexpression index, int elementSize, int size_type){
  return 
    AATMemory( 
      AATOperator(base, 
        AATOperator(AATConstant(elementSize, REG64), index, AAT_MULTIPLY, REG64),
      AAT_PLUS, REG64), 
    size_type
  );
  /* if index == 0 than element size * 0 will == 0; */
}

AATexpression BaseVariable(offset_ref offset, int size_type){
  return AATMemory( AATOperator(AATRegister( FP(), REG64),AATOffset( offset, REG64 ), AAT_PLUS, REG64), size_type);
}

AATexpression ConstantExpression(int value, int size_type){
 return AATConstant(value, size_type);
}

AATexpression CallExpression(AATexpressionList actuals, Label name, int size_type, int argMemSize){
  return AATFunctionCall( name, actuals, size_type, argMemSize);
}

AATstatement CallStatement(AATexpressionList actuals, Label name, int argMemSize){
  return AATProcedureCall( name, actuals, argMemSize);
}

AATexpression OperatorExpression(AATexpression left, AATexpression right, AAToperator operator, int size_type){
  return AATOperator(left, right, operator, size_type);
}

/*------------------ Statements ------------------*/
AATstatement functionDefinition(AATstatement body, int framesize, Label start, Label end){
 return AATFunctionDef(AATSequential(AATLabel(start), AATLabel(end)), body, framesize);
}

AATstatement ReturnStatement(AATexpression value, Label functionend, int size_type){
  if(size_type==PTR){
    return AATSequential(AATMove(AATRegister(Result64(), PTR), value, PTR), AATJump(functionend));
  }else if(size_type==INT){
    return AATSequential(AATMove(AATRegister(Result32(), INT), value, INT), AATJump(functionend));
  }else if(size_type==BYTE){
    return AATSequential(AATMove(AATRegister(Result32(), BOOL), value, BOOL), AATJump(functionend));
  }else{ // void return type
    return AATJump(functionend);
  }
}

AATstatement DoWhileStatement(AATexpression test, AATstatement dowhilebody){
  Label doWhile = NewNamedLabel("doWhile"), doEnd = NewNamedLabel("doEnd");
  return
  AATSequential(AATLabel(doWhile),
    AATSequential(dowhilebody,
      AATSequential(AATConditionalJump(test, doWhile), AATLabel(doEnd))
    )
  );
}

AATstatement ForStatement(AATstatement init, AATexpression test, AATstatement increment, AATstatement body){
  Label forTest = NewNamedLabel("forTest"), forStart = NewNamedLabel("forStart"), forEnd = NewNamedLabel("forEnd");
  return
  AATSequential(init,
    AATSequential(AATJump(forTest),
      AATSequential(AATLabel(forStart),
        AATSequential( body,
          AATSequential(increment,
            AATSequential(AATLabel(forTest),
              AATSequential(AATConditionalJump(test, forStart), AATLabel(forEnd))
            )
          )
        )
      )
    )
  );
}

AATstatement WhileStatement(AATexpression test, AATstatement whilebody){
  /* TODO: is whileEnd needed ??? it seems to not be used */
  Label whileTest = NewNamedLabel("whileTest"), whileStart = NewNamedLabel("whileStart"),
    whileEnd = NewNamedLabel("whileEnd");
  return
  AATSequential( AATJump(whileTest),
    AATSequential( AATLabel(whileStart),
      AATSequential( whilebody,
        AATSequential( AATLabel(whileTest),
          AATSequential( AATConditionalJump(test, whileStart), AATLabel(whileEnd) 
          )
        )
      )
    )
  );
}
AATstatement IfStatement(AATexpression test, AATstatement ifbody, AATstatement elsebody){
  Label iftrue = NewNamedLabel("IFTRUE"), ifend = NewNamedLabel("IFEND");
  if( elsebody )
    return
    AATSequential( AATConditionalJump( test, iftrue ),
      AATSequential( elsebody,
        AATSequential( AATJump( ifend ),
          AATSequential( AATLabel(iftrue),
            AATSequential( ifbody, AATLabel(ifend)
            )
          )
        )
      )
    );
  else
  /* Can the if statement made to be more efficient? */
    return
    AATSequential( AATConditionalJump( test, iftrue ),
      AATSequential( AATJump( ifend ),
        AATSequential( AATLabel(iftrue),
          AATSequential( ifbody, AATLabel(ifend)
          )
        )
      )
    );
}

AATstatement AssignmentStatement(AATexpression lhs, AATexpression rhs, int size){
  return AATMove(lhs, rhs, size);
}

AATstatement EmptyStatement() {
  return AATEmpty();
}

AATstatement SequentialStatement(AATstatement first, AATstatement second){
  return AATSequential(first, second);
}

AATexpressionList ActualList(AATexpression first, AATexpressionList rest, int size_type, int offset){
  return AATExpressionList(first, rest, size_type, offset);
}
