/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef codeGenArm64_H
#define codeGenArm64_H
#include "../assembly/AAT.h"
#include <stdio.h>

void generateCode(AATstatement tree, FILE *output);

#endif