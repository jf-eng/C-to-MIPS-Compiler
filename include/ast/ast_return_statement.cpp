#include "ast_return_statement.hpp"
#include <vector>

Node* ReturnStatement::getReturnExpr() { return branches[0]; }

ReturnStatement::ReturnStatement(Node* nodeptr){
	classname = "Return Statement";
	branches.push_back(nodeptr);
}


void ReturnStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
	// return value to $f0 if function returns double or float
	if(Program::funcTable[this->scope->funcName].returnType.find("float") != std::string::npos){
		getReturnExpr()->compileRec("$f0");
		std::cout<<"cvt.s.d $f0,$f0"<<std::endl;
	} 
	else if(Program::funcTable[this->scope->funcName].returnType.find("double") != std::string::npos){
		getReturnExpr()->compileRec("$f0");
	}
	else{
		getReturnExpr()->compileRec("$v0");
	}

	// terminate function upon executing return
	std::cerr << "*************** FuncName" << this->scope->funcName << std::endl;
	std::cout << "j " << this->scope->funcName <<"_endlabel"<< std::endl;
	std::cout << "nop" << std::endl;
	
}

void ReturnStatement::scanRec(Scope* parent){
	this->scope = parent;
	getReturnExpr()->scanRec(parent);
}
