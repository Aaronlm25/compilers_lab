#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <map>
#include <string>
#include <vector>

/*
 * Gramatica de la practica 5 (ver gram.txt) y tablas SLR(1).
 * Las tablas ACTION y GOTO se obtuvieron a mano a partir del automata LR(0)
 * y estan hardcodeadas en grammar.cpp; aqui no se genera nada, el driver
 * (parser.cpp) solo las consulta.
 */

enum Terminal {
	T_ITEM,
	T_ID,
	T_ASSIGN,
	T_SEMI,
	T_ROOM,
	T_LPAREN,
	T_RPAREN,
	T_LBRACE,
	T_RBRACE,
	T_DROP,
	T_CRY,
	T_FLEE,
	T_HEART,
	T_TEAR,
	T_FAITH,
	T_RANGEUP,
	T_SPEEDUP,
	T_NUM,
	T_ANGEL,
	T_DEVIL,
	T_ROLL,
	T_AMNESIA,
	T_EOF,
	T_ERROR /* caracter no reconocido por el scanner, no es parte de la gramatica */
};

enum NonTerminal {
	NT_PROGRAM,
	NT_DECL,
	NT_TYPE,
	NT_EXPR,
	NT_SHIFT,
	NT_ATOM
};

enum class ActionType { Error, Shift, Reduce, Accept };

struct Action {
	ActionType type = ActionType::Error;
	int value = 0; /* estado destino (shift) o numero de regla (reduce) */
};

struct Production {
	NonTerminal lhs;
	std::vector<std::string> rhs; /* solo para imprimir; el driver usa rhs.size() */
};

/* 0 = S' -> Program (gramatica aumentada), 1..20 = reglas de gram.txt */
extern const std::vector<Production> PRODUCTIONS;

extern const std::map<int, std::map<Terminal, Action>> ACTION;
extern const std::map<int, std::map<NonTerminal, int>> GOTO;

/* consultas a las tablas; una celda ausente es un error */
Action action_at(int state, Terminal t);
int goto_at(int state, NonTerminal nt); /* -1 si no hay transicion */

/* terminales con accion definida en un estado (para los mensajes de error) */
std::vector<Terminal> expected_terminals(int state);

const char *terminal_name(Terminal t);
const char *nonterminal_name(NonTerminal nt);
std::string production_text(int rule);

#endif // GRAMMAR_HPP
