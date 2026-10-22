#include "ast.hpp"

#include "grammar.hpp"

static void print_node(std::ostream &out, const Node &node, const std::string &prefix, bool last, bool root)
{
	out << prefix;
	if (!root)
		out << (last ? "`-- " : "|-- ");

	if (node.is_leaf()) {
		out << node.symbol;
		/* para id y num el lexema aporta informacion; en palabras clave es redundante */
		if (node.lexeme != node.symbol)
			out << " (" << node.lexeme << ")";
		out << "\n";
		return;
	}

	out << "[r" << node.rule << "] " << production_text(node.rule) << "\n";

	std::string child_prefix = prefix + (root ? "" : (last ? "    " : "|   "));
	for (size_t i = 0; i < node.children.size(); i++)
		print_node(out, *node.children[i], child_prefix, i + 1 == node.children.size(), false);
}

void print_ast(std::ostream &out, const Node &root)
{
	print_node(out, root, "", true, true);
}
