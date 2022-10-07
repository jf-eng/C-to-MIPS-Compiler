#pragma once

#include "ast_node.hpp"
#include <string>

class CompoundStatement : public Node {

public:

    // Constructor
    CompoundStatement(Node* nodeptr);
    Node* getStatementList();
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);

    void scanRec(Scope* parent);
    void noNewScopescanRec(Scope* parent);

};