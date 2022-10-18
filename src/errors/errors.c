/*
 * errors.c - functions used in all phases of the compiler to give
 *              error messages 
 */

/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errors.h"

int Number_Of_Errors = 0;

int Current_Line = 1;

void Error(int position, char *message,...)
{va_list ap;
 Number_Of_Errors++;
 fprintf(stderr,"Error in line %d:", position);
 va_start(ap,message);
 vfprintf(stderr, message, ap);
 va_end(ap);
 fprintf(stderr,"\n");
}

int numErrors() {
 return Number_Of_Errors;
}
int anyErrors() {
 return Number_Of_Errors;
}
