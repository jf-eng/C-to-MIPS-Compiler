#pragma once
#include "ast_node.hpp"

#include <string>


class JumpStatement : public Node {
protected:
	std::string type;
	
public:

	std::string jumpTarget;

    // Constructor
    JumpStatement(std::string type);

    std::string getType();

	void compileRec(reg_t destReg, int pointerArithmeticSize);
	void scanRec(Scope* parent);
};