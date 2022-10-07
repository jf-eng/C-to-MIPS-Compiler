#pragma once
#include "ast_node.hpp"
#include <string>


class StatementList : public Node {

public:

    // Constructor
    StatementList(Node* nodeptr);

	Node* getDeclaration();

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};