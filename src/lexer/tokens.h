/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#define IDENTIFIER 258
#define INTEGER_LITERAL 259
#define CLASS 260
#define DO 261
#define ELSE 262
#define TOKENTRUE 263
#define TOKENFALSE 264
#define FOR 265
#define IF 266
#define WHILE 267
#define PLUS 268
#define MINUS 269
#define MULTIPLY 270
#define DIVIDE 271
#define LBRACK 272
#define RBRACK 273
#define LBRACE 274
#define RBRACE 275
#define LPAREN 276
#define RPAREN 277
#define DOT 278
#define COMMA 279
#define SEMICOLON 280
#define EQ 281
#define NEQ 282
#define LT 283
#define GT 284
#define LEQ 285
#define GEQ 286
#define GETS 287
#define AND 288
#define OR 289
#define NOT 290
#define PLUSPLUS 291
#define MINUSMINUS 292
#define RETURN 293
#define NEW 294
#define GLOB 295
#define TOKENNULL 296
#define ERROR 297
/* todo(Char): add token for CHAR & STRING */

typedef union { 
     struct {
         int value;
         int line_number;
     } integer_value;
     struct {
         char *value;
         int line_number;
     } string_value;
     int line_number;
} YYSTYPE;
extern YYSTYPE yylval;
