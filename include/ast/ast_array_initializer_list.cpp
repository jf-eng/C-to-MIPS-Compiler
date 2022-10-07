#include "ast_array_initializer_list.hpp"
#include "ast_expression.hpp"

ArrayInitializerList::ArrayInitializerList(Node* expr){
    branches.push_back(expr);
    this->numOfElements = expr->branches.size();
}

void ArrayInitializerList::compileRec(reg_t destReg, int pointerArithmeticSize){

}

void ArrayInitializerList::scanRec(Scope* parent){
    this->scope = parent;
    branches[0]->scanRec(parent);
}

Node* ArrayInitializerList::getExpressionAtIndex(int i){
    return branches[0]->branches[i];
}
