#include "ast_labelled_statement.hpp"

LabelledStatement::LabelledStatement(Node* expr, Node* statement){
    classname = "Labelled Statement";
    if(expr == NULL){
        type = "default";
    } else {
        type = "case";
    }
    branches.push_back(expr);
    branches.push_back(statement);
}

Node* LabelledStatement::getExpression(){
    return branches[0];
}

Node* LabelledStatement::getStatement(){
    return branches[1];
}

std::string LabelledStatement::getType(){
    return type;
}

void LabelledStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
    std::cout << label << ":" << std::endl;
    branches[1]->compileRec(destReg);
}

void LabelledStatement::scanRec(Scope* parent){
    this->scope = parent;
    for(auto &b : branches){
        if(b != NULL) b->scanRec(parent);
    }
}