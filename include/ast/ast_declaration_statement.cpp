#include "ast_declaration_statement.hpp"
#include "ast_declaration_specifier.hpp"
#include "ast_declaration.hpp"

DeclarationStatement::DeclarationStatement(Node* nodeptr) {
	classname = "Declaration Statement";
	branches.push_back(nodeptr);
	if(nodeptr){
		hasDeclaration = true;
	}
	else{
		hasDeclaration = false;
	}
}


void DeclarationStatement::compileRec(reg_t destReg, int pointerArithmeticSize) {
	if(hasDeclaration){
		auto IdentifierName =dynamic_cast<Declaration&>(*getDeclaration()).getIdentifierName();
		if(scope->getParent()){
			// if not in global scope, push to stack
			// int a; (do need to push anything to stack)
		}
		else{

		}
	}
}

Node* DeclarationStatement::getDeclaration() {
	return branches[0];
}

void DeclarationStatement::scanRec(Scope* parent){
	this->scope = parent;
	if(hasDeclaration){
		// variables
		try {
			auto decl = dynamic_cast<Declaration&>(*getDeclaration());
			decl.insertVar(parent);
		} catch (const std::exception& e){std::cerr << e.what() << std::endl;}

		getDeclaration()->scanRec(parent);
	}
}



