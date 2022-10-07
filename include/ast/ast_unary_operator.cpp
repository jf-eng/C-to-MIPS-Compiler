#include "ast_unary_operator.hpp"

UnaryOperator::UnaryOperator(std::string op) {
	classname = "Unary Operator: " + op;
	this->op = op;
}

std::string UnaryOperator::getOp(){
	return this->op;
}

void UnaryOperator::compileRec(reg_t destReg, int pointerArithmeticSize) {

}

void UnaryOperator::scanRec(Scope* parent){
	this->scope = parent;

}