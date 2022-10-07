#include "ast_init_declaration_list.hpp"
#include "../program/program.hpp"
#include "ast_declaration_specifier.hpp"
#include "ast_declaration.hpp"

InitDeclarationList::InitDeclarationList(Node* declaration){
	classname = "Init Declaration List";
	branches.push_back(declaration);
}

unsigned int InitDeclarationList::getArgSize() {
	unsigned int acc = 0;
	for(auto b : branches){
		acc += dynamic_cast<Declaration&>(*b).getDeclarationSize();
	}
	return acc;
}

unsigned int InitDeclarationList::getArgNum() {
	return branches.size();
}

void InitDeclarationList::scanRec(Scope* parent){
	this->scope = parent;

	for(long unsigned int i = 0; i < branches.size(); i++){
		dynamic_cast<Declaration&>(*branches[i]).insertFunctionVars(parent, functionName);
	}
}

void InitDeclarationList::compileRec(reg_t destReg, int pointerArithmeticSize){

}