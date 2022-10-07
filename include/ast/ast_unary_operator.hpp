#pragma once
#include "ast_node.hpp"
#include <string>

class UnaryOperator : public Node {
protected:
    std::string op;
public:
    // Constructor
    UnaryOperator(std::string op);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);
    std::string getOp();
};