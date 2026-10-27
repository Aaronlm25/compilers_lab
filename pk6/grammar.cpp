#include"grammar.hpp"

// TABLA PROVISIONAL, la definitiva la entrega el integrante 2. El formato ya
// es el final, sustituirla es cambiar solo los inicializadores de abajo.
//
// El automata LR(1) canonico tiene 14 estados. Los kernels iguales se
// fusionan y quedan 10:
//     I4 con I11  -> 4    { L -> * . R }
//     I5 con I12  -> 5    { L -> id . }
//     I7 con I13  -> 7    { L -> * R . }
//     I8 con I10  -> 8    { R -> L . }
//
// El estado 2 es { [S -> L . = R, $], [R -> L ., $] }. En SLR se reduciria
// por R -> L con todo Follow(R)={=,$} y chocaria con el shift de '='. En
// LALR(1) el lookahead ahi es solo $, asi que ACTION[2,=] queda libre.

const vector<Production> PRODUCTIONS={
	{NT_S,{"S"}},
	{NT_S,{"L","=","R"}},
	{NT_S,{"R"}},
	{NT_L,{"*","R"}},
	{NT_L,{"id"}},
	{NT_R,{"L"}}
};

const int NUM_STATES=10;

static const Action s4={ActionType::Shift,4};
static const Action s5={ActionType::Shift,5};
static const Action s6={ActionType::Shift,6};
static const Action acc={ActionType::Accept,0};

static Action r(int rule){
	return {ActionType::Reduce,rule};
}

const map<int,map<Terminal,Action>> ACTION={
	{0,{{T_DEREF,s4},{T_ID,s5}}},
	{1,{{T_EOF,acc}}},
	{2,{{T_ASSIGN,s6},{T_EOF,r(5)}}},
	{3,{{T_EOF,r(2)}}},
	{4,{{T_DEREF,s4},{T_ID,s5}}},
	{5,{{T_ASSIGN,r(4)},{T_EOF,r(4)}}},
	{6,{{T_DEREF,s4},{T_ID,s5}}},
	{7,{{T_ASSIGN,r(3)},{T_EOF,r(3)}}},
	{8,{{T_ASSIGN,r(5)},{T_EOF,r(5)}}},
	{9,{{T_EOF,r(1)}}}
};

const map<int,map<NonTerminal,int>> GOTO={
	{0,{{NT_S,1},{NT_L,2},{NT_R,3}}},
	{4,{{NT_L,8},{NT_R,7}}},
	{6,{{NT_L,8},{NT_R,9}}}
};

Action actionAt(int state,Terminal t){
	auto row=ACTION.find(state);
	if(row==ACTION.end()){
		return Action();
	}
	auto cell=row->second.find(t);
	if(cell==row->second.end()){
		return Action();
	}
	return cell->second;
}

int gotoAt(int state,NonTerminal nt){
	auto row=GOTO.find(state);
	if(row==GOTO.end()){
		return -1;
	}
	auto cell=row->second.find(nt);
	if(cell==row->second.end()){
		return -1;
	}
	return cell->second;
}

vector<Terminal> expectedTerminals(int state){
	vector<Terminal> out;
	auto row=ACTION.find(state);
	if(row==ACTION.end()){
		return out;
	}
	for(const auto& cell:row->second){
		out.push_back(cell.first);
	}
	return out;
}

static char deref='*';

void setDerefSymbol(char c){
	deref=c;
}

char derefSymbol(){
	return deref;
}

const char* terminalName(Terminal t){
	static char derefText[2]={'*','\0'};
	switch(t){
		case T_ASSIGN:
			return "=";
		case T_DEREF:
			derefText[0]=deref;
			return derefText;
		case T_ID:
			return "id";
		case T_EOF:
			return "$";
		case T_ERROR:
			return "error";
	}
	return "?";
}

const char* nonterminalName(NonTerminal nt){
	switch(nt){
		case NT_S:
			return "S";
		case NT_L:
			return "L";
		case NT_R:
			return "R";
	}
	return "?";
}

string productionText(int rule){
	const Production& p=PRODUCTIONS[rule];
	string out=nonterminalName(p.lhs);
	out+=" ->";
	for(const string& sym:p.rhs){
		out+=" ";
		// la regla 3 se imprime con el operador activo
		out+=(sym=="*")?string(1,deref):sym;
	}
	return out;
}
