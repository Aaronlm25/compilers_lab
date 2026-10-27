#include "grammar.hpp"

static Action shift(int state) { return {ActionType::Shift, state}; }
static Action reduce(int rule) { return {ActionType::Reduce, rule}; }
static Action accept() { return {ActionType::Accept, 0}; }

const std::vector<Production> PRODUCTIONS = {
	/* 0 */  {NT_PROGRAM, {"Program"}}, /* S' -> Program */
	/* 1 */  {NT_PROGRAM, {"Program", "Decl"}},
	/* 2 */  {NT_PROGRAM, {"Decl"}},
	/* 3 */  {NT_DECL,    {"item", "Type", "id", "=", "Expr", ";"}},
	/* 4 */  {NT_DECL,    {"room", "id", "(", ")", "{", "Program", "}"}},
	/* 5 */  {NT_DECL,    {"drop", "Expr", ";"}},
	/* 6 */  {NT_DECL,    {"cry", "(", "Expr", ")", ";"}},
	/* 7 */  {NT_DECL,    {"flee", ";"}},
	/* 8 */  {NT_TYPE,    {"heart"}},
	/* 9 */  {NT_TYPE,    {"tear"}},
	/* 10 */ {NT_TYPE,    {"faith"}},
	/* 11 */ {NT_EXPR,    {"Expr", "rangeup", "Shift"}},
	/* 12 */ {NT_EXPR,    {"Shift"}},
	/* 13 */ {NT_SHIFT,   {"Shift", "speedup", "Atom"}},
	/* 14 */ {NT_SHIFT,   {"Atom"}},
	/* 15 */ {NT_ATOM,    {"id"}},
	/* 16 */ {NT_ATOM,    {"num"}},
	/* 17 */ {NT_ATOM,    {"angel"}},
	/* 18 */ {NT_ATOM,    {"devil"}},
	/* 19 */ {NT_ATOM,    {"roll", "(", "Expr", ")"}},
	/* 20 */ {NT_ATOM,    {"amnesia", "Atom"}},
};

/*
 * Tabla ACTION SLR(1).
 * Conflictos LR(0) resueltos con Follow:
 *   I15 = { Expr -> Shift . , Shift -> Shift . speedup Atom }
 *   I35 = { Expr -> Expr rangeup Shift . , Shift -> Shift . speedup Atom }
 * En LR(0) ambas celdas [15, speedup] y [35, speedup] tendrian shift 29 y
 * reduce (r12 / r11). Como Follow(Expr) = { ;, ), rangeup } no contiene
 * speedup, SLR(1) solo reduce en esos tres terminales y deja el shift.
 */
const std::map<int, std::map<Terminal, Action>> ACTION = {
	{0, {{T_ITEM, shift(3)}, {T_ROOM, shift(4)}, {T_DROP, shift(5)}, {T_CRY, shift(6)}, {T_FLEE, shift(7)}}},
	{1, {{T_ITEM, shift(3)}, {T_ROOM, shift(4)}, {T_DROP, shift(5)}, {T_CRY, shift(6)}, {T_FLEE, shift(7)}, {T_EOF, accept()}}},
	{2, {{T_ITEM, reduce(2)}, {T_ROOM, reduce(2)}, {T_RBRACE, reduce(2)}, {T_DROP, reduce(2)}, {T_CRY, reduce(2)}, {T_FLEE, reduce(2)}, {T_EOF, reduce(2)}}},
	{3, {{T_HEART, shift(10)}, {T_TEAR, shift(11)}, {T_FAITH, shift(12)}}},
	{4, {{T_ID, shift(13)}}},
	{5, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{6, {{T_LPAREN, shift(23)}}},
	{7, {{T_SEMI, shift(24)}}},
	{8, {{T_ITEM, reduce(1)}, {T_ROOM, reduce(1)}, {T_RBRACE, reduce(1)}, {T_DROP, reduce(1)}, {T_CRY, reduce(1)}, {T_FLEE, reduce(1)}, {T_EOF, reduce(1)}}},
	{9, {{T_ID, shift(25)}}},
	{10, {{T_ID, reduce(8)}}},
	{11, {{T_ID, reduce(9)}}},
	{12, {{T_ID, reduce(10)}}},
	{13, {{T_LPAREN, shift(26)}}},
	{14, {{T_SEMI, shift(27)}, {T_RANGEUP, shift(28)}}},
	{15, {{T_SEMI, reduce(12)}, {T_RPAREN, reduce(12)}, {T_RANGEUP, reduce(12)}, {T_SPEEDUP, shift(29)}}},
	{16, {{T_SEMI, reduce(14)}, {T_RPAREN, reduce(14)}, {T_RANGEUP, reduce(14)}, {T_SPEEDUP, reduce(14)}}},
	{17, {{T_SEMI, reduce(15)}, {T_RPAREN, reduce(15)}, {T_RANGEUP, reduce(15)}, {T_SPEEDUP, reduce(15)}}},
	{18, {{T_SEMI, reduce(16)}, {T_RPAREN, reduce(16)}, {T_RANGEUP, reduce(16)}, {T_SPEEDUP, reduce(16)}}},
	{19, {{T_SEMI, reduce(17)}, {T_RPAREN, reduce(17)}, {T_RANGEUP, reduce(17)}, {T_SPEEDUP, reduce(17)}}},
	{20, {{T_SEMI, reduce(18)}, {T_RPAREN, reduce(18)}, {T_RANGEUP, reduce(18)}, {T_SPEEDUP, reduce(18)}}},
	{21, {{T_LPAREN, shift(30)}}},
	{22, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{23, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{24, {{T_ITEM, reduce(7)}, {T_ROOM, reduce(7)}, {T_RBRACE, reduce(7)}, {T_DROP, reduce(7)}, {T_CRY, reduce(7)}, {T_FLEE, reduce(7)}, {T_EOF, reduce(7)}}},
	{25, {{T_ASSIGN, shift(33)}}},
	{26, {{T_RPAREN, shift(34)}}},
	{27, {{T_ITEM, reduce(5)}, {T_ROOM, reduce(5)}, {T_RBRACE, reduce(5)}, {T_DROP, reduce(5)}, {T_CRY, reduce(5)}, {T_FLEE, reduce(5)}, {T_EOF, reduce(5)}}},
	{28, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{29, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{30, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{31, {{T_SEMI, reduce(20)}, {T_RPAREN, reduce(20)}, {T_RANGEUP, reduce(20)}, {T_SPEEDUP, reduce(20)}}},
	{32, {{T_RPAREN, shift(38)}, {T_RANGEUP, shift(28)}}},
	{33, {{T_ID, shift(17)}, {T_NUM, shift(18)}, {T_ANGEL, shift(19)}, {T_DEVIL, shift(20)}, {T_ROLL, shift(21)}, {T_AMNESIA, shift(22)}}},
	{34, {{T_LBRACE, shift(40)}}},
	{35, {{T_SEMI, reduce(11)}, {T_RPAREN, reduce(11)}, {T_RANGEUP, reduce(11)}, {T_SPEEDUP, shift(29)}}},
	{36, {{T_SEMI, reduce(13)}, {T_RPAREN, reduce(13)}, {T_RANGEUP, reduce(13)}, {T_SPEEDUP, reduce(13)}}},
	{37, {{T_RPAREN, shift(41)}, {T_RANGEUP, shift(28)}}},
	{38, {{T_SEMI, shift(42)}}},
	{39, {{T_SEMI, shift(43)}, {T_RANGEUP, shift(28)}}},
	{40, {{T_ITEM, shift(3)}, {T_ROOM, shift(4)}, {T_DROP, shift(5)}, {T_CRY, shift(6)}, {T_FLEE, shift(7)}}},
	{41, {{T_SEMI, reduce(19)}, {T_RPAREN, reduce(19)}, {T_RANGEUP, reduce(19)}, {T_SPEEDUP, reduce(19)}}},
	{42, {{T_ITEM, reduce(6)}, {T_ROOM, reduce(6)}, {T_RBRACE, reduce(6)}, {T_DROP, reduce(6)}, {T_CRY, reduce(6)}, {T_FLEE, reduce(6)}, {T_EOF, reduce(6)}}},
	{43, {{T_ITEM, reduce(3)}, {T_ROOM, reduce(3)}, {T_RBRACE, reduce(3)}, {T_DROP, reduce(3)}, {T_CRY, reduce(3)}, {T_FLEE, reduce(3)}, {T_EOF, reduce(3)}}},
	{44, {{T_ITEM, shift(3)}, {T_ROOM, shift(4)}, {T_RBRACE, shift(45)}, {T_DROP, shift(5)}, {T_CRY, shift(6)}, {T_FLEE, shift(7)}}},
	{45, {{T_ITEM, reduce(4)}, {T_ROOM, reduce(4)}, {T_RBRACE, reduce(4)}, {T_DROP, reduce(4)}, {T_CRY, reduce(4)}, {T_FLEE, reduce(4)}, {T_EOF, reduce(4)}}},
};

/* Tabla GOTO (solo estados con alguna transicion por no terminal) */
const std::map<int, std::map<NonTerminal, int>> GOTO = {
	{0, {{NT_PROGRAM, 1}, {NT_DECL, 2}}},
	{1, {{NT_DECL, 8}}},
	{3, {{NT_TYPE, 9}}},
	{5, {{NT_EXPR, 14}, {NT_SHIFT, 15}, {NT_ATOM, 16}}},
	{22, {{NT_ATOM, 31}}},
	{23, {{NT_EXPR, 32}, {NT_SHIFT, 15}, {NT_ATOM, 16}}},
	{28, {{NT_SHIFT, 35}, {NT_ATOM, 16}}},
	{29, {{NT_ATOM, 36}}},
	{30, {{NT_EXPR, 37}, {NT_SHIFT, 15}, {NT_ATOM, 16}}},
	{33, {{NT_EXPR, 39}, {NT_SHIFT, 15}, {NT_ATOM, 16}}},
	{40, {{NT_PROGRAM, 44}, {NT_DECL, 2}}},
	{44, {{NT_DECL, 8}}},
};

Action action_at(int state, Terminal t)
{
	auto row = ACTION.find(state);
	if (row == ACTION.end())
		return {};
	auto cell = row->second.find(t);
	return cell == row->second.end() ? Action{} : cell->second;
}

int goto_at(int state, NonTerminal nt)
{
	auto row = GOTO.find(state);
	if (row == GOTO.end())
		return -1;
	auto cell = row->second.find(nt);
	return cell == row->second.end() ? -1 : cell->second;
}

std::vector<Terminal> expected_terminals(int state)
{
	std::vector<Terminal> out;
	auto row = ACTION.find(state);
	if (row != ACTION.end())
		for (const auto &[t, a] : row->second)
			out.push_back(t);
	return out;
}

const char *terminal_name(Terminal t)
{
	switch (t) {
		case T_ITEM: return "item";
		case T_ID: return "id";
		case T_ASSIGN: return "=";
		case T_SEMI: return ";";
		case T_ROOM: return "room";
		case T_LPAREN: return "(";
		case T_RPAREN: return ")";
		case T_LBRACE: return "{";
		case T_RBRACE: return "}";
		case T_DROP: return "drop";
		case T_CRY: return "cry";
		case T_FLEE: return "flee";
		case T_HEART: return "heart";
		case T_TEAR: return "tear";
		case T_FAITH: return "faith";
		case T_RANGEUP: return "rangeup";
		case T_SPEEDUP: return "speedup";
		case T_NUM: return "num";
		case T_ANGEL: return "angel";
		case T_DEVIL: return "devil";
		case T_ROLL: return "roll";
		case T_AMNESIA: return "amnesia";
		case T_EOF: return "$";
		case T_ERROR: return "ERROR";
	}
	return "?";
}

const char *nonterminal_name(NonTerminal nt)
{
	switch (nt) {
		case NT_PROGRAM: return "Program";
		case NT_DECL: return "Decl";
		case NT_TYPE: return "Type";
		case NT_EXPR: return "Expr";
		case NT_SHIFT: return "Shift";
		case NT_ATOM: return "Atom";
	}
	return "?";
}

std::string production_text(int rule)
{
	const Production &p = PRODUCTIONS[rule];
	std::string s = rule == 0 ? "S'" : nonterminal_name(p.lhs);
	s += " ->";
	for (const auto &sym : p.rhs)
		s += " " + sym;
	return s;
}
