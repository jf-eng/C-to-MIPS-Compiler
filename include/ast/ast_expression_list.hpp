#pragma once
#include "ast_node.hpp"
#include <string>

class ExpressionList : public Node {
public:
    ExpressionList(Node * expression);
    void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};