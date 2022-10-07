#include "ast_node.hpp"
#include <iostream>
#include <vector>
#include "ast_expression.hpp"

std::vector<Node*> Node::getBranches(){
    return branches;
}

void Node::listAppend(Node* nodeptr) {
    branches.push_back(nodeptr);
}

void Node::listAppend(Node* nodeptr, std::string type){

}

void Node::listAppend(std::string* ident, std::string type){

}


void Node::PrettyPrint(std::ostream &dst, std::string indent) {
    dst << indent << classname << " [Branches: " << branches.size() << "]";

    if(branches.size()>0){
        dst << " [" <<std::endl;
        for(auto b : branches){
            if(b == NULL) continue;
            b->PrettyPrint(dst, indent+"    ");
        }
        dst << indent << "]";
    }
    dst << std::endl;
}

void Node::ScopePrettyPrint(std::ostream &dst, std::string indent) {
    std::string scopename = (scope == NULL) ? "NULL" : scope->classname;
    dst << indent << classname << " [Scope: " << scopename << "]";

    if(branches.size()>0){
        dst << " [" <<std::endl;
        for(auto b : branches){
            if(b == NULL) continue;
            b->ScopePrettyPrint(dst, indent+"    ");
        }
        dst << indent << "]";
    }
    dst << std::endl;
}

// returns addr of x.a, x[1], x
int Node::getStackOffSet(){
	auto exprPtr = dynamic_cast<Expression*>(this);

    if(exprPtr->getValue()!=""){
        return this->scope->searchIdent(exprPtr->getValue()).stackOffset;
    }
    else{
        if(exprPtr->getOpr()=="[]"){
            int offset = dynamic_cast<Expression*>(this->branches[1])->getConstantValue().INT;
            int elementSize = this->scope->searchIdent(dynamic_cast<Expression*>(this->branches[0])->getValue()).pointedSize;
            offset *= elementSize;
            return offset + this->branches[0]->getStackOffSet() + elementSize;
        }
        else if(exprPtr->getOpr()=="."){
            std::string base = dynamic_cast<Expression*>(exprPtr->branches[0])->getValue();
            std::string member = dynamic_cast<Expression*>(exprPtr->branches[1])->getValue();
            
            int baseoffset = exprPtr->branches[0]->getStackOffSet();
            std::string structType = this->scope->searchIdent(base).type.substr(7, std::string::npos);
            int memberoffset = Program::structTable[structType].attributes[member].second;
            return baseoffset + memberoffset;
        }
    }
}

std::string Node::getIdent(){
    for (auto &b : branches){
        std::string id = b->getIdent();
        if(id != "") return id;
    }
    

    // base case
    if(dynamic_cast<Expression*>(this)){
        if(dynamic_cast<Expression*>(this)->getType() == "identifier") {
            return dynamic_cast<Expression*>(this)->getValue();
        } else {
            return "";
        }
    } else {
        return "";
    }
    
    throw std::runtime_error("[ERROR] Could not find ident in expression.");
}