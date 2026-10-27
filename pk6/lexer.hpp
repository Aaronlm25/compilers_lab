#ifndef LEXER_HPP
#define LEXER_HPP

#include<bits/stdc++.h>
#include"grammar.hpp"
using namespace std;

struct Token{
	Terminal type;
	string lexeme;
	int line;
};

// Convierte el fuente en la lista de tokens que recibe el driver. Siempre
// termina con T_EOF. Un caracter ajeno al lenguaje entra como T_ERROR y el
// parser lo rechaza. Implementado con flex en scanner.l.
vector<Token> tokenize(const string& source);

#endif
