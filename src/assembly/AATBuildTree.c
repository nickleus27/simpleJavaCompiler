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
  AATexpressionList actuals = AATExpressionList(size, NULL, 0, 0);
  return AATFunctionCall("allocate",actuals);

}

/* remember to add offset to envEntry every time a variable is entered into environment */
/* cmd+f : "VarEntry" and add variable offsets. */
/* ??? do formals have a positive offset ??? */
AATexpression ClassVariable(AATexpression base, int offset){
  if ( offset ){
    return AATMemory( AATOperator(base, _AATConstant(offset), AAT_MINUS) );
  }
  else
    return AATMemory( base ); /* if no offset do not need to subtract from memory */
}

/* remember to add offset to envEntry every time a variable is entered into environment */
/* cmd+f : "VarEntry" and add variable offsets. */
AATexpression ArrayVariable(AATexpression base, AATexpression index, int elementSize){
  return AATMemory( 
    AATOperator(base, 
      AATOperator(_AATConstant(elementSize),index, AAT_MULTIPLY), 
    AAT_MINUS)
  );
  /* if index == 0 than element size * 0 will == 0; */
}

/* remember to add offset to envEntry every time a variable is entered into environment */
/* cmd+f : "VarEntry" and add variable offsets. */
AATexpression BaseVariable(int* offset){
  return AATMemory( AATOperator(AATRegister( FP() ),AATConstant( offset ), AAT_PLUS ) );
}

AATexpression ConstantExpression(int value){
  /*
  int* val = (int*)malloc(sizeof(int));
  *val = value;
  return AATConstant(val);
  */
 return _AATConstant(value);
}

/*needs work. need to push args on stack */
AATexpression CallExpression(AATexpressionList actuals, Label name /*, int ArgSize*/){
  /* need to move SP() down first before function call*/
  /* AATSequential to move down SP(), and attach AATFunctionCall at the end*/
  return AATFunctionCall( name, actuals);
}

AATstatement CallStatement(AATexpressionList actuals, Label name, int argMemSize){
  /* need to move SP() down first before function call*/
  /* AATSequential to move down SP(), and attach AATProcedureCall at the end*/
  return AATProcedureCall( name, actuals, argMemSize);
}

AATexpression OperatorExpression(AATexpression left, AATexpression right, AAToperator operator){
  return AATOperator(left, right,  operator);
}

/*------------------> Statements <------------------*/
AATstatement functionDefinition(AATstatement body, int framesize, Label start, Label end){
  /* framesize * WORDSIZE + saved registers + (LR, FP, SP) */
  /**
   *  Need to make sure registers AccSP32() and AccSP64() are being save and restored properly
   */
  int localVarSize = framesize - 4*8; /* saving 5 registers so 5-1=4, SP starts om 0 offset*/
  return
  AATSequential(AATLabel(start),
    AATSequential(AATMove( AATRegister(SP()), AATOperator (AATRegister( SP() ) , _AATConstant( framesize ), AAT_MINUS ), REG),
      AATSequential(AATMove( AATMemory( AATRegister( SP() ) ), AATOperator (AATRegister( SP() ) , _AATConstant( framesize ), AAT_PLUS ), REG),
        AATSequential(AATMove( AATMemory( AATOperator(AATRegister( SP() ), _AATConstant( localVarSize ), AAT_PLUS ) ), AATRegister( FP() ), REG),
          AATSequential(AATMove( AATRegister(FP()), AATOperator(AATRegister( SP() ), _AATConstant( localVarSize ), AAT_PLUS ), REG),
            AATSequential(AATMove( AATMemory( AATOperator(AATRegister( FP() ), _AATConstant( REG ), AAT_PLUS ) ), AATRegister( ReturnAddr() ), REG),
              AATSequential(AATMove( AATMemory( AATOperator(AATRegister( FP() ), _AATConstant( 2* REG ), AAT_PLUS ) ), AATRegister( AccSP32() ), REG),
                AATSequential(AATMove( AATMemory( AATOperator(AATRegister( FP() ), _AATConstant( 3* REG ), AAT_PLUS ) ), AATRegister( AccSP64() ), REG),
                  AATSequential(body,
                    AATSequential(AATLabel(end),
                      AATSequential(AATMove(AATRegister(AccSP64()), AATMemory( AATOperator(AATRegister(FP()), _AATConstant( 3* REG ), AAT_PLUS)), REG),
                        AATSequential(AATMove(AATRegister(AccSP32()), AATMemory( AATOperator(AATRegister(FP()), _AATConstant( 2* REG ), AAT_PLUS)), REG),
                          AATSequential(AATMove(AATRegister(ReturnAddr()), AATMemory( AATOperator(AATRegister(FP()), _AATConstant( REG ), AAT_PLUS)), REG),
                            AATSequential(AATMove(AATRegister(FP()), AATMemory(AATRegister(FP())), REG),
                              AATSequential(AATMove(AATRegister(SP()), AATMemory( AATRegister(SP())), REG), AATReturn())
                            )
                          )
                        )    
                      )
                    )
                  )
                )
              )
            )
          )
        )
      )
    )
  );
}

AATstatement ReturnStatement(AATexpression value, Label functionend){
  /* need to check for size type to return?*/
  /* need to make if statement for 32 bit and 64 bit*/
  return AATSequential(AATMove(AATRegister(Result32()), value, INT), AATJump(functionend));
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
