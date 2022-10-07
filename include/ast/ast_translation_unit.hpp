#pragma once

#include "ast_node.hpp"
#include "../scope/scope.hpp"


class TranslationUnit : public Node {

protected:
    Scope* level0;

public:

    // Constructor
    TranslationUnit(Node* nodeptr);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};
