#ifndef AST_HPP
#define AST_HPP

#include<bits/stdc++.h>
using namespace std;

// Nodo del arbol sintactico. Las hojas son terminales y guardan su lexema,
// los nodos internos se crean en cada reduccion y guardan la regla usada.
struct Node{
	string symbol;
	string lexeme;
	int rule=-1;
	vector<unique_ptr<Node>> children;

	bool isLeaf() const{
		return rule<0;
	}
};

void printAst(ostream& out,const Node& root);

#endif
