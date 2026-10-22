#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "grammar.hpp"

struct Token {
	Terminal type;
	std::string lexeme;
	int line;
};

/*
 * Convierte el texto fuente en la lista de tokens que recibe el parser.
 * Siempre termina con un token T_EOF ($). Si aparece un caracter que no
 * pertenece al lenguaje se incluye como T_ERROR y el parser lo rechaza.
 * Implementado con flex en scanner.l.
 */
std::vector<Token> tokenize(const std::string &source);

#endif // LEXER_HPP
