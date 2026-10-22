#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <vector>

/*
 * Nodo del arbol sintactico. Las hojas son terminales (guardan su lexema);
 * los nodos internos se crean en cada reduccion y recuerdan la regla usada.
 */
struct Node {
	std::string symbol;
	std::string lexeme; /* solo hojas */
	int rule = -1;      /* solo nodos internos */
	std::vector<std::unique_ptr<Node>> children;

	bool is_leaf() const { return rule < 0; }
};

/* imprime el arbol con indentacion jerarquica */
void print_ast(std::ostream &out, const Node &root);

#endif // AST_HPP
