#pragma once
#include "ast_node.hpp"
#include <string>

class InitDeclarationList : public Node {

public:
    std::string functionName;

    // Constructor
    InitDeclarationList(Node* declaration);
	
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
	// called by function during a scan
	unsigned int getArgSize();
    unsigned int getArgNum();
};

