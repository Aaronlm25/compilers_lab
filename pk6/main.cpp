#include<bits/stdc++.h>
#include"grammar.hpp"
#include"lexer.hpp"
#include"parser.hpp"
using namespace std;

static void usage(const char* prog){
	cerr<<"Uso: "<<prog<<" [-t] [-D digito | --op=OP] [archivo | -e \"cadena\"]\n"
	    <<"  -t           muestra la traza de la pila (shift/reduce)\n"
	    <<"  -D digito    penultimo digito del numero de cuenta: par usa '*', impar usa '&'\n"
	    <<"  --op=OP      fija el operador unario directamente ('*' o '&')\n"
	    <<"  -e cadena    analiza la cadena dada en lugar de un archivo\n"
	    <<"  sin archivo  lee la entrada estandar\n";
}

int main(int argc,char** argv){
	bool trace=false;
	string source;
	bool haveSource=false;

	for(int i=1;i<argc;i++){
		string arg=argv[i];
		if(arg=="-t"){
			trace=true;
		}else if(arg=="-D"&&i+1<argc){
			string d=argv[++i];
			if(d.size()!=1||!isdigit(static_cast<unsigned char>(d[0]))){
				cerr<<"-D espera un digito\n";
				return 2;
			}
			setDerefSymbol((d[0]-'0')%2==0?'*':'&');
		}else if(arg.rfind("--op=",0)==0){
			string op=arg.substr(5);
			if(op!="*"&&op!="&"){
				cerr<<"--op espera '*' o '&'\n";
				return 2;
			}
			setDerefSymbol(op[0]);
		}else if(arg=="-e"&&i+1<argc){
			source=argv[++i];
			haveSource=true;
		}else if(arg=="-h"||arg=="--help"){
			usage(argv[0]);
			return 0;
		}else if(!haveSource&&arg[0]!='-'){
			ifstream in(arg);
			if(!in){
				cerr<<"No se pudo abrir "<<arg<<"\n";
				return 2;
			}
			stringstream ss;
			ss<<in.rdbuf();
			source=ss.str();
			haveSource=true;
		}else{
			usage(argv[0]);
			return 2;
		}
	}

	if(!haveSource){
		stringstream ss;
		ss<<cin.rdbuf();
		source=ss.str();
	}

	vector<Token> tokens=tokenize(source);

	cout<<"Tokens:";
	for(const Token& t:tokens){
		cout<<" "<<terminalName(t.type);
	}
	cout<<"\n\n";

	if(trace){
		cout<<"Traza:\n";
	}
	ParseResult result=parse(tokens,trace?&cout:nullptr);
	if(trace){
		cout<<"\n";
	}

	if(!result.accepted){
		cout<<"Cadena RECHAZADA\n"<<result.error<<"\n";
		return 1;
	}

	cout<<"Cadena ACEPTADA\n\nAST:\n";
	printAst(cout,*result.ast);
	return 0;
}
