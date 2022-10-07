#include "ast_expression_list.hpp"
#include <vector>

ExpressionList::ExpressionList(Node * expression){
    classname = "Expression List";
    branches.push_back(expression);
}

void ExpressionList::compileRec(reg_t destReg, int pointerArithmeticSize){
    for(auto b : branches){
        b->compileRec(destReg);
    }
}

void ExpressionList::scanRec(Scope* parent){
    this->scope = parent;
    for(auto b : branches){
        b->scanRec(parent);
    }
}