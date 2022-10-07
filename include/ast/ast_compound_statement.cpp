#include "ast_compound_statement.hpp"
#include <vector>
#include <iostream>


CompoundStatement::CompoundStatement(Node* nodeptr){
	classname = "Compound Statement";
	branches.push_back(nodeptr);
}

void CompoundStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
	std::cerr << "[DEBUG] Compiling " << classname << std::endl;
	// if there is nothing in compound statement
	if(branches[0] == NULL) return;

	branches[0]->compileRec(destReg);


	// kill all regs when exiting compound statements
	std::cerr << scope->classname << std::endl;

	for(auto &p : scope->symbolTable){
		
		if(p.second.reg != ""){
			Program::killRegister(p.second.reg, scope);
		}
	}
}

Node* CompoundStatement::getStatementList(){
	return branches[0];
}

void CompoundStatement::scanRec(Scope* parent){

	Scope* scp = new Scope(parent);
	this->scope = scp;
	
	if(getStatementList()){
		getStatementList()->scanRec(scp);
	}
}

void CompoundStatement::noNewScopescanRec(Scope* parent){

	this->scope = parent;
	
	if(getStatementList()){
		getStatementList()->scanRec(parent);
	}
}

