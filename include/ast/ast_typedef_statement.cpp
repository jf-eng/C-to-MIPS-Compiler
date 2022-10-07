#include "ast_typedef_statement.hpp"
#include "ast_expression.hpp"


TypedefStatement::TypedefStatement(Node* declSpec, std::string* ident, bool isPointer){
    classname = (isPointer) ? "Typedef (pointer)" : "Typedef (not pointer)";
    branches.push_back(declSpec);
    branches.push_back(new Expression(ident));
}

void TypedefStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
    // typedef doesn't compile into anything
    // it only modifies the typedefTable
}

void TypedefStatement::scanRec(Scope* parent){
    this->scope = parent;
    for(auto b : branches){
        b->scanRec(parent);
    }
}