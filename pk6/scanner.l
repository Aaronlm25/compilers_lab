%option noyywrap yylineno nounput noinput

%{
// Solo los terminales de gram.txt. Se genera con flex y se compila como C++.
#include"lexer.hpp"

#define YY_DECL static int nextToken(void)

static bool unclosedComment=false;
static int commentLine=0;
%}

%x COMMENT

ID          [a-zA-Z_][a-zA-Z0-9_]*
WS          [ \t\r\n]+

%%

"/*"                    { commentLine=yylineno; BEGIN(COMMENT); }
<COMMENT>"*/"           { BEGIN(INITIAL); }
<COMMENT>\n|.           { }
<COMMENT><<EOF>>        { BEGIN(INITIAL); unclosedComment=true; return T_ERROR; }

"//"[^\n]*              { }
{WS}                    { }

"="                     { return T_ASSIGN; }

 /* solo el operador que corresponde a D es valido, el otro es error */
"*"|"&"                 { return yytext[0]==derefSymbol()?T_DEREF:T_ERROR; }

{ID}                    { return T_ID; }

<<EOF>>                 { return T_EOF; }

.                       { return T_ERROR; }

%%

vector<Token> tokenize(const string& source){
	vector<Token> tokens;
	YY_BUFFER_STATE buffer=yy_scan_string(source.c_str());
	yylineno=1;
	unclosedComment=false;
	BEGIN(INITIAL);

	for(;;){
		int line=yylineno;
		Terminal t=static_cast<Terminal>(nextToken());
		if(t==T_EOF){
			// el $ se reporta en la linea del ultimo token real
			tokens.push_back({T_EOF,"$",tokens.empty()?1:tokens.back().line});
			break;
		}
		if(t==T_ERROR&&unclosedComment){
			tokens.push_back({T_ERROR,"/*",commentLine});
		}else{
			tokens.push_back({t,yytext,line});
		}
	}

	yy_delete_buffer(buffer);
	return tokens;
}
