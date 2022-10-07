#pragma once
#include "ast_node.hpp"
#include <string>


class ExpressionStatement : public Node {

public:

    // Constructor
    ExpressionStatement(Node* node);
    
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};