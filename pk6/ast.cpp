#include"ast.hpp"
#include"grammar.hpp"

static void printNode(ostream& out,const Node& node,const string& prefix,bool last,bool root){
	out<<prefix;
	if(!root){
		out<<(last?"`-- ":"|-- ");
	}
	if(node.isLeaf()){
		out<<node.symbol;
		// en id el lexema aporta informacion, en los simbolos es redundante
		if(node.lexeme!=node.symbol){
			out<<" ("<<node.lexeme<<")";
		}
		out<<"\n";
		return;
	}
	out<<"[r"<<node.rule<<"] "<<productionText(node.rule)<<"\n";
	string childPrefix=prefix+(root?"":(last?"    ":"|   "));
	for(size_t i=0;i<node.children.size();i++){
		printNode(out,*node.children[i],childPrefix,i+1==node.children.size(),false);
	}
}

void printAst(ostream& out,const Node& root){
	printNode(out,root,"",true,true);
}
