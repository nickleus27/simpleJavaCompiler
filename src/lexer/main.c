/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

#include <stdio.h>
#include "tokens.h"
#include "errors.h"
#include "lex.h"

extern FILE *yyin;
YYSTYPE yylval;
int yylex(void);

char *tokenNames[] = { "IDENTIFIER","INTEGER_LITERAL","CLASS","DO","ELSE",
		       "TRUE","FALSE","FOR","IF","WHILE","PLUS","MINUS", 
		       "MULTIPLY", "DIVIDE", "LBRACK","RBRACK", "LBRACE",
		       "RBRACE","LPAREN","RPAREN","DOT","COMMA","SEMICOLON",
		       "EQ","NEQ","LT","GT","LEQ","GEQ","GETS","AND","OR",
		       "NOT","PLUSPLUS","MINUSMINUS","RETURN","NEW", "COLON", "DEF",
           "GLOBS", "FUNCTS", "LCOC", "DEFINITIONS" };

char *tokenName(int token) {
  if (token < 258 || token > 295)
    return "BAD TOKEN";
  else
    return tokenNames[token-258];
}

int main(int argc, char **argv) {
 char *filename; 
 int token;
 if (argc!=2) {
   fprintf(stderr,"usage: %s filename\n",argv[0]); 
   exit(1);
 }
 filename=argv[1];
 yyin = fopen(filename,"r");
 if (yyin == NULL) {
   fprintf(stderr,"Cannot open file:%s\n",filename);
 } else {
   for(;;) {
     token=yylex();
     if (token==0){
        printf("<EoF>\n");
        break;
     }
     switch(token) {
     case IDENTIFIER: 
       printf("line:%2d %15s  value = %s\n",
	      yylval.string_value.line_number, tokenName(token),
	      yylval.string_value.value);
       break;
     case INTEGER_LITERAL:
       printf("line:%2d %15s  value = %d\n", yylval.integer_value.line_number, tokenName(token), yylval.integer_value.value);
       break;
      case ERROR:
        printf("error line:%2d %15s\n", Current_Line,tokenName(token));
        break;
     default:
       printf("line:%2d %15s\n",yylval.line_number,tokenName(token));
     }
   }
 }
 return 0;
}








