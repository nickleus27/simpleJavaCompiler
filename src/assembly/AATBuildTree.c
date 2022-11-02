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
#include <stdlib.h>


/*------------------> Expressions <------------------*/
AATexpression Allocate(AATexpression size) {
  /**
   * TODO: Need to get offset right in AATExpressionList call below
   * 
   */
  AATexpressionList actuals = AATExpressionList(size, NULL, REG64, 0);
  return AATFunctionCall("allocate",actuals, PTR);

}

AATexpression ClassVariable(AATexpression base, int offset, int size_type){
  /**
   * TODO: Need to come back and check that the size_types are correct for registers and memory
   * compatibility. Double check in analyzeVar in semantic.c too.
   */
  if ( offset ){
    return AATMemory( AATOperator(base, _AATConstant(offset, REG64), AAT_MINUS, REG64), size_type);
  }
  else
    return AATMemory( base, size_type ); /* if no offset do not need to subtract from memory */
}

AATexpression ArrayVariable(AATexpression base, AATexpression index, int elementSize, int size_type){
  /**
   * TODO: Need to come back and check that the size_types are correct for registers and memory
   * compatibility. Double check in analyzeVar in semantic.c too.
   */
  return 
    AATMemory( 
      AATOperator(base, 
        AATOperator(_AATConstant(elementSize, REG64),index, AAT_MULTIPLY, REG64), 
      AAT_PLUS, REG64), 
    size_type
  );
  /* if index == 0 than element size * 0 will == 0; */
}

AATexpression BaseVariable(int* offset, int size_type){
  return AATMemory( AATOperator(AATRegister( FP(), REG64),AATConstant( offset, REG64 ), AAT_PLUS, REG64), size_type);
}

AATexpression ConstantExpression(int value, int size_type){
 return _AATConstant(value, size_type);
}

AATexpression CallExpression(AATexpressionList actuals, Label name, int size_type){
  return AATFunctionCall( name, actuals, size_type);
}

AATstatement CallStatement(AATexpressionList actuals, Label name, int argMemSize){
  return AATProcedureCall( name, actuals, argMemSize);
}

AATexpression OperatorExpression(AATexpression left, AATexpression right, AAToperator operator, int size_type){
  return AATOperator(left, right, operator, size_type);
}

/*------------------> Statements <------------------*/
AATstatement functionDefinition(AATstatement body, int framesize, Label start, Label end){
  /* framesize * WORDSIZE + saved registers + (LR, FP, SP) */
  //int localVarSize = framesize - 4*8; /* saving 5 registers so 5-1=4, SP starts om 0 offset*/
 return AATFunctionDef(AATSequential(AATLabel(start), AATLabel(end)), body, framesize);
}

AATstatement ReturnStatement(AATexpression value, Label functionend, int size_type){
  /* need to check for size type to return?*/
  /* need to make if statement for 32 bit and 64 bit*/
  if(size_type==PTR){
    return AATSequential(AATMove(AATRegister(Result64(), PTR), value, PTR), AATJump(functionend));
  }else if(size_type==INT){
    return AATSequential(AATMove(AATRegister(Result32(), INT), value, INT), AATJump(functionend));
  }else{
    return AATSequential(AATMove(AATRegister(Result32(), BOOL), value, BOOL), AATJump(functionend));
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
