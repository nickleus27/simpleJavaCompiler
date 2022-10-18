/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/
#define ARM64 1

#if defined (ARM64)
    #define CHAR 1
    #define BOOL 1
    #define INT 4
    #define PTR 8
    #define REG 8
    #define EXPR_STACKSIZE 2048
#endif

#if defined (MIPS)
    #define CHAR 1
    #define BOOL 4
    #define INT 4
    #define PTR 4
    #define REG 4
#endif