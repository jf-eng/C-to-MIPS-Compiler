#pragma once
#include "ast_node.hpp"
#include <string>

class ArrayInitializerList : public Node {

protected:
    int numOfElements;
public:

    // for assignment w/ declaration
    ArrayInitializerList(Node* dest);
    Node* getExpressionAtIndex(int i);

    void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};