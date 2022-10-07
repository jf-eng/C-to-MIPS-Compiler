#include "ast_enumerator.hpp"


Enumerator::Enumerator(std::string* ident){
    classname = "Enumerator (autoassigned)";
    type = "autoassign";
    identifier = *ident;
}

Enumerator::Enumerator(std::string* ident, Node* expr){
    classname = "Enumerator (constantexpr)";
    type = "expression";
    identifier = *ident;
    branches.push_back(expr);
}

std::string Enumerator::getIdentifier() {
    return identifier; 
}

std::string Enumerator::getType() {
    return type;
}

void Enumerator::compileRec(reg_t destReg, int pointerArithmeticSize){

}

void Enumerator::scanRec(Scope* parent){

}
