#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include<bits/stdc++.h>
using namespace std;

// Gramatica de la practica 6 (ver gram.txt) y tablas LALR(1) hardcodeadas
// en grammar.cpp. El driver solo las consulta, aqui no se genera nada.

// T_DEREF es el operador unario, '*' o '&' segun D.
// T_ERROR es un caracter que no pertenece al lenguaje.
enum Terminal{
	T_ASSIGN,
	T_DEREF,
	T_ID,
	T_EOF,
	T_ERROR
};

enum NonTerminal{
	NT_S,
	NT_L,
	NT_R
};

enum class ActionType{Error,Shift,Reduce,Accept};

// value es el estado destino en un shift y el numero de regla en un reduce
struct Action{
	ActionType type=ActionType::Error;
	int value=0;
};

// rhs es solo para imprimir, el driver unicamente usa rhs.size()
struct Production{
	NonTerminal lhs;
	vector<string> rhs;
};

// indices 0..5 en el orden de gram.txt, el 0 es S' -> S
extern const vector<Production> PRODUCTIONS;

// Una fila por estado y dentro una celda por simbolo. Map anidado y no
// matriz densa porque la tabla es dispersa, asi la celda ausente ya es error.
extern const map<int,map<Terminal,Action>> ACTION;
extern const map<int,map<NonTerminal,int>> GOTO;
extern const int NUM_STATES;

Action actionAt(int state,Terminal t);

// -1 si no hay transicion
int gotoAt(int state,NonTerminal nt);

vector<Terminal> expectedTerminals(int state);

// D es el penultimo digito del numero de cuenta: par usa '*', impar usa '&'
void setDerefSymbol(char c);
char derefSymbol();

const char* terminalName(Terminal t);
const char* nonterminalName(NonTerminal nt);
string productionText(int rule);

#endif
