#pragma once
#include "ast_node.hpp"
#include <string>


class ReturnStatement : public Node {

public:
    // Constructor
    ReturnStatement(Node* nodeptr);
    Node* getReturnExpr();
    
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};