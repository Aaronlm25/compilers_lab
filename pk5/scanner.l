%option noyywrap yylineno nounput noinput

%{
/* Scanner de la practica 5: solo los tokens que usa gram.txt.
 * Se genera con flex y se compila como C++ (ver Makefile). */
#include "lexer.hpp"

#define YY_DECL static int next_token(void)

static bool unclosed_comment = false;
static int comment_line = 0;
%}

%x COMMENT

ID          [a-zA-Z_][a-zA-Z0-9_]*
INT         [0-9]+
FLOAT       {INT}\.[0-9]+
WS          [ \t\r\n]+

%%

"/*"                    { comment_line = yylineno; BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n|.           { /* descartar contenido de comentario */ }
<COMMENT><<EOF>>        { BEGIN(INITIAL); unclosed_comment = true; return T_ERROR; }

"//"[^\n]*              { /* comentario de linea */ }
{WS}                    { /* ignorar espacios */ }

"item"                  { return T_ITEM; }
"room"                  { return T_ROOM; }
"drop"                  { return T_DROP; }
"cry"                   { return T_CRY; }
"flee"                  { return T_FLEE; }
"heart"                 { return T_HEART; }
"tear"                  { return T_TEAR; }
"faith"                 { return T_FAITH; }
"rangeup"               { return T_RANGEUP; }
"speedup"               { return T_SPEEDUP; }
"angel"                 { return T_ANGEL; }
"devil"                 { return T_DEVIL; }
"roll"                  { return T_ROLL; }
"amnesia"               { return T_AMNESIA; }

"="                     { return T_ASSIGN; }
";"                     { return T_SEMI; }
"("                     { return T_LPAREN; }
")"                     { return T_RPAREN; }
"{"                     { return T_LBRACE; }
"}"                     { return T_RBRACE; }

{FLOAT}|{INT}           { return T_NUM; }
{ID}                    { return T_ID; }

<<EOF>>                 { return T_EOF; }

.                       { return T_ERROR; }

%%

std::vector<Token> tokenize(const std::string &source)
{
	std::vector<Token> tokens;
	YY_BUFFER_STATE buffer = yy_scan_string(source.c_str());
	yylineno = 1;
	unclosed_comment = false;
	BEGIN(INITIAL);

	for (;;) {
		int line = yylineno;
		Terminal t = static_cast<Terminal>(next_token());
		if (t == T_EOF) {
			/* el $ se reporta en la linea del ultimo token real */
			tokens.push_back({T_EOF, "$", tokens.empty() ? 1 : tokens.back().line});
			break;
		}
		if (t == T_ERROR && unclosed_comment)
			tokens.push_back({T_ERROR, "/*", comment_line});
		else
			tokens.push_back({t, yytext, line});
	}

	yy_delete_buffer(buffer);
	return tokens;
}
