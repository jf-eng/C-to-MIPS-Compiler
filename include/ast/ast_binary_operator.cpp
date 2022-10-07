#include "ast_binary_operator.hpp"
#include <string>

BinaryOperator::BinaryOperator(std::string opr) {
	classname = "Binary Operator: " + opr;
	op = opr;
}

std::string BinaryOperator::getOp(){
	return op;
}


void BinaryOperator::compileRec(reg_t destReg, int pointerArithmeticSize) {

}

void BinaryOperator::scanRec(Scope* parent){
	this->scope = parent;
	
}