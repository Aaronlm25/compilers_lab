#ifndef PARSER_HPP
#define PARSER_HPP

#include<bits/stdc++.h>
#include"ast.hpp"
#include"lexer.hpp"
using namespace std;

// ast solo tiene valor si accepted, error solo si no
struct ParseResult{
	bool accepted=false;
	unique_ptr<Node> ast;
	string error;
};

// Motor LALR(1): interpreta las tablas de grammar.cpp con la pila clasica de
// estados y simbolos. Si trace!=nullptr escribe ahi cada paso.
ParseResult parse(const vector<Token>& tokens,ostream* trace=nullptr);

#endif
