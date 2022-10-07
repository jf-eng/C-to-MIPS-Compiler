#pragma once
#include "ast_node.hpp"
#include <string>


class AssignmentOperator : public Node {

protected:
    std::string assignmentOp;

public:
    // Constructor
    AssignmentOperator(std::string op);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);
    
    std::string getAssignmentOperator();
};