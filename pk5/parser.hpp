#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"

struct ParseResult {
	bool accepted = false;
	std::unique_ptr<Node> ast; /* solo si accepted */
	std::string error;         /* solo si !accepted */
};

/*
 * Motor (driver) LR: interpreta las tablas ACTION/GOTO de grammar.cpp con
 * la pila clasica de estados y simbolos. Si trace != nullptr escribe ahi
 * cada paso (pila, entrada restante, accion).
 */
ParseResult parse(const std::vector<Token> &tokens, std::ostream *trace = nullptr);

#endif // PARSER_HPP
