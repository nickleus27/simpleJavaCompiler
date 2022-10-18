/**
 * Starter code is by David Galles and can be found at https://www.cs.usfca.edu/~galles/compilerdesign/
 * File finished by Nick Anderson
 * nickleus27@gmail.com
 * 10/18/2022
 * 
*/

%{
#include <string.h>
#include "../errors/errors.h"
#include "../lexer/tokens.h"
  
int yywrap(void) {
  return 1;
}
YYSTYPE yylval;
#define NEWLINE yylineno++
#define ASSIGN_LINENO yylval.line_number = yylineno; Current_Line = yylineno
int nested_comment = 0;
%}

newline                   \n
whitespace                [ \t]
int_literal               [0-9]
letter                    [a-zA-Z]
underscore                "_"
  /* todo(Char): add functionality for chars and strings */
  /* char                   \'.\'          */
  /* str                    \".*\"          */
letter_or_underscore      ({letter}|{underscore})
letter_underscore_digit   ({letter}|{int_literal}|{underscore})
identifier                ({letter_or_underscore}{letter_underscore_digit}*)
line_comment              "//".*
begin_comment             "/*"
end_comment                "*/"
%x COMMENT

%%
{line_comment}    { }
{begin_comment}           { BEGIN(COMMENT); nested_comment++; }
<COMMENT>{begin_comment}  { nested_comment++; }
<COMMENT>{end_comment}    { nested_comment--; if(!nested_comment) BEGIN(INITIAL); }
<COMMENT>{newline}        { NEWLINE; }
<COMMENT>.                { }
{whitespace}      { }
"+"               { ASSIGN_LINENO; return PLUS; }
"-"               { ASSIGN_LINENO; return MINUS; }
"*"               { ASSIGN_LINENO; return MULTIPLY; }
"/"               { ASSIGN_LINENO; return DIVIDE; }
";"               { ASSIGN_LINENO; return SEMICOLON; }
"!"               { ASSIGN_LINENO; return NOT; }
"."               { ASSIGN_LINENO; return DOT; }
","               { ASSIGN_LINENO; return COMMA; }
"("               { ASSIGN_LINENO; return LPAREN; }
")"               { ASSIGN_LINENO; return RPAREN; }
"{"               { ASSIGN_LINENO; return LBRACE; }
"}"               { ASSIGN_LINENO; return RBRACE; }
"["               { ASSIGN_LINENO; return LBRACK; }
"]"               { ASSIGN_LINENO; return RBRACK; }
"="               { ASSIGN_LINENO; return GETS; }
"<"               { ASSIGN_LINENO; return LT; }
">"               { ASSIGN_LINENO; return GT; }
"++"              { ASSIGN_LINENO; return PLUSPLUS; }
"--"              { ASSIGN_LINENO; return MINUSMINUS; }
{newline}         { NEWLINE; }
!=                { ASSIGN_LINENO; return NEQ; }
==                { ASSIGN_LINENO; return EQ; }
"<="              { ASSIGN_LINENO; return LEQ; }
>=                { ASSIGN_LINENO; return GEQ; }
&&                { ASSIGN_LINENO; return AND; }
"||"              { ASSIGN_LINENO; return OR; }
if                { ASSIGN_LINENO; return IF; }
else              { ASSIGN_LINENO; return ELSE; }
do                { ASSIGN_LINENO; return DO; }
for               { ASSIGN_LINENO; return FOR; }
while             { ASSIGN_LINENO; return WHILE; }
true              { ASSIGN_LINENO; return TOKENTRUE; }
false             { ASSIGN_LINENO; return TOKENFALSE; }
class             { ASSIGN_LINENO; return CLASS; }
new               { ASSIGN_LINENO; return NEW; }
return            { ASSIGN_LINENO; return RETURN; }
glob              { ASSIGN_LINENO; return GLOB; }

  /* todo(Char): add functionality for chars and strings */

{int_literal}+    { yylval.integer_value.value = atoi(yytext);  yylval.integer_value.line_number = yylineno; return INTEGER_LITERAL; }
{identifier}      { //if( yylval.string_value.value ){ free( yylval.string_value.value ); }
                    yylval.string_value.value = (char *)malloc(sizeof(char) * strlen(yytext)+1);
                    strcpy(yylval.string_value.value, yytext);
                    yylval.string_value.line_number = yylineno; return IDENTIFIER; }
.                 { ASSIGN_LINENO; return ERROR; }
%%


     


