#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "grammar.hpp"
#include "lexer.hpp"
#include "parser.hpp"

static void usage(const char *prog)
{
	std::cerr << "Uso: " << prog << " [-t] [archivo | -e \"cadena\"]\n"
	          << "  -t           muestra la traza de la pila (shift/reduce)\n"
	          << "  -e cadena    analiza la cadena dada en lugar de un archivo\n"
	          << "  sin archivo  lee la entrada estandar\n";
}

int main(int argc, char **argv)
{
	bool trace = false;
	std::string source;
	bool have_source = false;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "-t") {
			trace = true;
		} else if (arg == "-e" && i + 1 < argc) {
			source = argv[++i];
			have_source = true;
		} else if (arg == "-h" || arg == "--help") {
			usage(argv[0]);
			return 0;
		} else if (!have_source && arg[0] != '-') {
			std::ifstream in(arg);
			if (!in) {
				std::cerr << "No se pudo abrir " << arg << "\n";
				return 2;
			}
			std::stringstream ss;
			ss << in.rdbuf();
			source = ss.str();
			have_source = true;
		} else {
			usage(argv[0]);
			return 2;
		}
	}

	if (!have_source) {
		std::stringstream ss;
		ss << std::cin.rdbuf();
		source = ss.str();
	}

	std::vector<Token> tokens = tokenize(source);

	std::cout << "Tokens:";
	for (const Token &t : tokens)
		std::cout << " " << terminal_name(t.type);
	std::cout << "\n\n";

	if (trace)
		std::cout << "Traza:\n";
	ParseResult result = parse(tokens, trace ? &std::cout : nullptr);
	if (trace)
		std::cout << "\n";

	if (!result.accepted) {
		std::cout << "Cadena RECHAZADA\n" << result.error << "\n";
		return 1;
	}

	std::cout << "Cadena ACEPTADA\n\nAST:\n";
	print_ast(std::cout, *result.ast);
	return 0;
}
