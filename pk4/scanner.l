%option noyywrap yylineno

%{
#include <stdio.h>
#include "scanner.h"
%}

%x COMMENT

ID          [a-zA-Z_][a-zA-Z0-9_]*
INT         [0-9]+
FLOAT       ({INT}\.[0-9]*([eE][+-]?{INT})?|{INT}[eE][+-]?{INT})
WS          [ \t\r\n]+

%%

"/*"                    { BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n             { /* yylineno se actualiza automáticamente */ }
<COMMENT>.              { /* descartar contenido de comentario */ }
<COMMENT><<EOF>>        {
                            printf("[ERROR:comentario sin cerrar (EOF en linea %d)]\n", yylineno);
                            BEGIN(INITIAL);
                            return TOK_EOF;
                        }

"//"[^\n]*             { /* comentario de línea */ }

{WS}                    { /* ignorar espacios */ }

"heart"                 { return TOK_KW_INT; }
"tear"                  { return TOK_KW_FLOAT; }
"soulheart"             { return TOK_KW_DOUBLE; }
"rune"                  { return TOK_KW_CHAR; }
"faith"                 { return TOK_KW_BOOL; }
"void"                  { return TOK_KW_VOID; }
"if"                    { return TOK_KW_IF; }
"else"                  { return TOK_KW_ELSE; }
"while"                 { return TOK_KW_WHILE; }
"for"                   { return TOK_KW_FOR; }
"foreach"               { return TOK_KW_FOREACH; }
"item"                  { return TOK_KW_VAR; }
"eternal"               { return TOK_KW_CONST; }
"room"                  { return TOK_KW_FUNC; }
"drop"                  { return TOK_KW_RETURN; }
"flee"                  { return TOK_KW_BREAK; }
"dash"                  { return TOK_KW_CONTINUE; }
"cry"                   { return TOK_KW_PRINT; }
"pray"                  { return TOK_KW_INPUT; }
"die"                   { return TOK_KW_EXIT; }
"roll"                  { return TOK_KW_ROLL; }
"seed"                  { return TOK_KW_SEED; }
"angel"                 { return TOK_BOOL_LITERAL; }
"devil"                 { return TOK_BOOL_LITERAL; }

"speedup"               { return TOK_SHL; }
"speeddown"             { return TOK_SHR; }
"rangedown"             { return TOK_BIT_AND; }
"rangeup"               { return TOK_BIT_OR; }
"puberty"               { return TOK_BIT_XOR; }
"amnesia"               { return TOK_BIT_NOT; }

"statup"                { return TOK_INC; }
"statdown"              { return TOK_DEC; }

"+="                    { return TOK_PLUS_ASSIGN; }
"-="                    { return TOK_MINUS_ASSIGN; }
"*="                    { return TOK_MUL_ASSIGN; }
"/="                    { return TOK_DIV_ASSIGN; }
"%="                    { return TOK_MOD_ASSIGN; }
"=="                    { return TOK_EQ; }
"!="                    { return TOK_NEQ; }
"<="                    { return TOK_LE; }
">="                    { return TOK_GE; }
"&&"                    { return TOK_AND; }
"||"                    { return TOK_OR; }

"="                     { return TOK_ASSIGN; }
"<"                     { return TOK_LT; }
">"                     { return TOK_GT; }
"!"                     { return TOK_NOT; }
"+"                     { return TOK_PLUS; }
"-"                     { return TOK_MINUS; }
"*"                     { return TOK_MUL; }
"/"                     { return TOK_DIV; }
"%"                     { return TOK_MOD; }

"("                     { return TOK_LPAREN; }
")"                     { return TOK_RPAREN; }
"{"                     { return TOK_LBRACE; }
"}"                     { return TOK_RBRACE; }
"["                     { return TOK_LBRACKET; }
"]"                     { return TOK_RBRACKET; }
","                     { return TOK_COMMA; }
";"                     { return TOK_SEMICOLON; }

{FLOAT}                 { return TOK_FLOAT_LITERAL; }
{INT}                   { return TOK_INT_LITERAL; }
\'([^\\\n]|\\.)\'    { return TOK_CHAR_LITERAL; }
\"([^\\\n]|\\.)*\"  { return TOK_STRING_LITERAL; }
{ID}                    { return TOK_IDENTIFIER; }

<<EOF>>                 { return TOK_EOF; }

.                       { return TOK_ERROR; }

%%
