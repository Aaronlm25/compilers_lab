#include"parser.hpp"
#include"grammar.hpp"

// la pila lleva estados y simbolos intercalados: s0 X1 s1 X2 s2 ...
static string stackText(const vector<int>& states,const vector<unique_ptr<Node>>& symbols){
	ostringstream ss;
	ss<<states[0];
	for(size_t i=0;i<symbols.size();i++){
		ss<<" "<<symbols[i]->symbol<<" "<<states[i+1];
	}
	return ss.str();
}

static string inputText(const vector<Token>& tokens,size_t pos){
	ostringstream ss;
	for(size_t i=pos;i<tokens.size();i++){
		ss<<(i>pos?" ":"")<<terminalName(tokens[i].type);
	}
	return ss.str();
}

static string syntaxError(const Token& tok,int state){
	ostringstream ss;
	if(tok.type==T_ERROR){
		ss<<"Error lexico en linea "<<tok.line<<": ";
		if(tok.lexeme=="/*"){
			ss<<"comentario de bloque sin cerrar";
		}else{
			ss<<"'"<<tok.lexeme<<"' no es un token valido";
		}
		return ss.str();
	}
	ss<<"Error sintactico en linea "<<tok.line<<": token inesperado ";
	if(tok.type==T_EOF){
		ss<<"fin de entrada ($)";
	}else if(tok.lexeme!=terminalName(tok.type)){
		ss<<"'"<<tok.lexeme<<"' ("<<terminalName(tok.type)<<")";
	}else{
		ss<<"'"<<tok.lexeme<<"'";
	}
	ss<<" en el estado "<<state<<"; se esperaba:";
	for(Terminal t:expectedTerminals(state)){
		ss<<" "<<terminalName(t);
	}
	return ss.str();
}

// paso, pila, entrada y accion; las columnas se alinean hasta el final
using TraceRow=array<string,4>;

static void printTrace(ostream& out,const vector<TraceRow>& rows){
	array<size_t,4> width={0,0,0,0};
	for(const auto& row:rows){
		for(size_t c=0;c<3;c++){
			width[c]=max(width[c],row[c].size());
		}
	}
	for(const auto& row:rows){
		for(size_t c=0;c<3;c++){
			out<<left<<setw(width[c]+2)<<row[c];
		}
		out<<row[3]<<"\n";
	}
}

static ParseResult run(const vector<Token>& tokens,vector<TraceRow>* trace){
	ParseResult result;
	vector<int> states={0};
	vector<unique_ptr<Node>> symbols;
	size_t pos=0;
	int step=1;

	if(trace){
		trace->push_back({"Paso","Pila","Entrada","Accion"});
	}

	for(;;){
		int state=states.back();
		const Token& tok=tokens[pos];
		Action act=actionAt(state,tok.type);

		if(trace){
			trace->push_back({to_string(step++),stackText(states,symbols),inputText(tokens,pos),""});
		}

		if(act.type==ActionType::Shift){
			if(trace){
				trace->back()[3]="shift "+to_string(act.value);
			}
			auto leaf=make_unique<Node>();
			leaf->symbol=terminalName(tok.type);
			leaf->lexeme=tok.lexeme;
			symbols.push_back(move(leaf));
			states.push_back(act.value);
			pos++;
		}else if(act.type==ActionType::Reduce){
			const Production& p=PRODUCTIONS[act.value];
			size_t n=p.rhs.size();

			auto node=make_unique<Node>();
			node->symbol=nonterminalName(p.lhs);
			node->rule=act.value;
			// los ultimos n simbolos de la pila son los hijos, en orden
			for(size_t i=symbols.size()-n;i<symbols.size();i++){
				node->children.push_back(move(symbols[i]));
			}
			symbols.resize(symbols.size()-n);
			states.resize(states.size()-n);

			int next=gotoAt(states.back(),p.lhs);
			if(trace){
				trace->back()[3]="reduce "+to_string(act.value)+" ("+productionText(act.value)+"), goto "+to_string(next);
			}
			if(next<0){
				// no deberia pasar con una tabla LALR(1) correcta
				result.error="Error interno: GOTO["+to_string(states.back())+", "+nonterminalName(p.lhs)+"] vacio";
				return result;
			}
			symbols.push_back(move(node));
			states.push_back(next);
		}else if(act.type==ActionType::Accept){
			if(trace){
				trace->back()[3]="accept";
			}
			result.accepted=true;
			result.ast=move(symbols.back());
			return result;
		}else{
			if(trace){
				trace->back()[3]="error";
			}
			result.error=syntaxError(tok,state);
			return result;
		}
	}
}

ParseResult parse(const vector<Token>& tokens,ostream* trace){
	vector<TraceRow> rows;
	ParseResult result=run(tokens,trace?&rows:nullptr);
	if(trace){
		printTrace(*trace,rows);
	}
	return result;
}
