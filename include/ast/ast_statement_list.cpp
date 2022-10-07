#include "ast_statement_list.hpp"
#include <vector>

StatementList::StatementList(Node* nodeptr){
	classname = "Statement List";
	branches.push_back(nodeptr);
}


void StatementList::compileRec(reg_t destReg, int pointerArithmeticSize){
	for (long unsigned int i = 0; i < branches.size(); i++){
		branches[i]->compileRec(destReg);
	}
}

void StatementList::scanRec(Scope* parent){
	this->scope = parent;

	for(auto b : branches){
		b->scanRec(parent);
	}
}

