#include "ast_assignment_operator.hpp"
#include <vector>

AssignmentOperator::AssignmentOperator(std::string op){
	classname = "Assignment Operator: " + op;
	assignmentOp = op;
}

std::string AssignmentOperator::getAssignmentOperator(){
	return assignmentOp;
}

void AssignmentOperator::compileRec(reg_t destReg, int pointerArithmeticSize){

}

void AssignmentOperator::scanRec(Scope* parent){

}
