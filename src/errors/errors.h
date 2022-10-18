/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#ifndef errors_h
#define errors_h
extern int Current_Line;


void Error(int position, char * message,...);
int numErrors();
int anyErrors();
#endif
