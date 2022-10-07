#include "ast_expression_statement.hpp"

// Constructor
ExpressionStatement::ExpressionStatement(Node* node){
    this->classname = "Expression Statement";
    branches.push_back(node);
}

void ExpressionStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
    if(branches[0] != NULL){
        branches[0]->compileRec(destReg);
    }
}


void ExpressionStatement::scanRec(Scope* parent){
    this->scope = parent;
    if(branches[0] != NULL){
        branches[0]->scanRec(parent);
    }
}