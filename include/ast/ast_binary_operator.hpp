#pragma once
#include "ast_node.hpp"
#include <string>


class BinaryOperator : public Node {
protected:
	std::string op;
public:
	
    BinaryOperator(std::string opr);
	std::string getOp();
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};