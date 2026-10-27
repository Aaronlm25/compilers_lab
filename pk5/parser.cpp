#include "parser.hpp"

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

#include "grammar.hpp"

/* pila con estados y simbolos intercalados: s0 X1 s1 X2 s2 ... */
static std::string stack_text(const std::vector<int> &states, const std::vector<std::unique_ptr<Node>> &symbols)
{
	std::ostringstream ss;
	ss << states[0];
	for (size_t i = 0; i < symbols.size(); i++)
		ss << " " << symbols[i]->symbol << " " << states[i + 1];
	return ss.str();
}

static std::string input_text(const std::vector<Token> &tokens, size_t pos)
{
	std::ostringstream ss;
	for (size_t i = pos; i < tokens.size(); i++)
		ss << (i > pos ? " " : "") << terminal_name(tokens[i].type);
	return ss.str();
}

static std::string syntax_error(const Token &tok, int state)
{
	std::ostringstream ss;
	if (tok.type == T_ERROR) {
		ss << "Error lexico en linea " << tok.line << ": ";
		if (tok.lexeme == "/*")
			ss << "comentario de bloque sin cerrar";
		else
			ss << "'" << tok.lexeme << "' no es un token valido";
		return ss.str();
	}

	ss << "Error sintactico en linea " << tok.line << ": token inesperado ";
	if (tok.type == T_EOF)
		ss << "fin de entrada ($)";
	else if (tok.lexeme != terminal_name(tok.type))
		ss << "'" << tok.lexeme << "' (" << terminal_name(tok.type) << ")";
	else
		ss << "'" << tok.lexeme << "'";

	ss << " en el estado " << state << "; se esperaba:";
	for (Terminal t : expected_terminals(state))
		ss << " " << terminal_name(t);
	return ss.str();
}

/* filas de la traza: paso, pila, entrada, accion; se alinean al final */
using TraceRow = std::array<std::string, 4>;

static void print_trace(std::ostream &out, const std::vector<TraceRow> &rows)
{
	std::array<size_t, 4> width = {0, 0, 0, 0};
	for (const auto &row : rows)
		for (size_t c = 0; c < 3; c++)
			width[c] = std::max(width[c], row[c].size());

	for (const auto &row : rows) {
		for (size_t c = 0; c < 3; c++)
			out << std::left << std::setw(width[c] + 2) << row[c];
		out << row[3] << "\n";
	}
}

static ParseResult run(const std::vector<Token> &tokens, std::vector<TraceRow> *trace)
{
	ParseResult result;
	std::vector<int> states = {0};
	std::vector<std::unique_ptr<Node>> symbols;
	size_t pos = 0;
	int step = 1;

	if (trace)
		trace->push_back({"Paso", "Pila", "Entrada", "Accion"});

	for (;;) {
		int state = states.back();
		const Token &tok = tokens[pos];
		Action act = action_at(state, tok.type);

		if (trace)
			trace->push_back({std::to_string(step++), stack_text(states, symbols), input_text(tokens, pos), ""});

		switch (act.type) {
		case ActionType::Shift: {
			if (trace)
				trace->back()[3] = "shift " + std::to_string(act.value);
			auto leaf = std::make_unique<Node>();
			leaf->symbol = terminal_name(tok.type);
			leaf->lexeme = tok.lexeme;
			symbols.push_back(std::move(leaf));
			states.push_back(act.value);
			pos++;
			break;
		}

		case ActionType::Reduce: {
			const Production &p = PRODUCTIONS[act.value];
			size_t n = p.rhs.size();

			auto node = std::make_unique<Node>();
			node->symbol = nonterminal_name(p.lhs);
			node->rule = act.value;
			/* los ultimos n simbolos de la pila son los hijos, en orden */
			for (size_t i = symbols.size() - n; i < symbols.size(); i++)
				node->children.push_back(std::move(symbols[i]));
			symbols.resize(symbols.size() - n);
			states.resize(states.size() - n);

			int next = goto_at(states.back(), p.lhs);
			if (trace)
				trace->back()[3] = "reduce " + std::to_string(act.value) + " (" + production_text(act.value) +
				                   "), goto " + std::to_string(next);
			if (next < 0) {
				/* no deberia pasar con una tabla SLR correcta */
				result.error = "Error interno: GOTO[" + std::to_string(states.back()) + ", " +
				               nonterminal_name(p.lhs) + "] vacio";
				return result;
			}
			symbols.push_back(std::move(node));
			states.push_back(next);
			break;
		}

		case ActionType::Accept:
			if (trace)
				trace->back()[3] = "accept";
			result.accepted = true;
			result.ast = std::move(symbols.back());
			return result;

		case ActionType::Error:
			if (trace)
				trace->back()[3] = "error";
			result.error = syntax_error(tok, state);
			return result;
		}
	}
}

ParseResult parse(const std::vector<Token> &tokens, std::ostream *trace)
{
	std::vector<TraceRow> rows;
	ParseResult result = run(tokens, trace ? &rows : nullptr);
	if (trace)
		print_trace(*trace, rows);
	return result;
}
