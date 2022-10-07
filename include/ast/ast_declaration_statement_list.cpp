#include "ast_declaration_statement_list.hpp"
#include "ast_declaration_statement.hpp"
#include "ast_declaration.hpp"

DeclarationStatementList::DeclarationStatementList(Node* initialDeclaration){
    classname = "Declaration Statement List";
    branches.push_back(initialDeclaration);
}


void DeclarationStatementList::compileRec(reg_t destReg, int pointerArithmeticSize){

}


void DeclarationStatementList::scanRec(Scope* parent){
    this->scope = parent;
}

void DeclarationStatementList::insertNewStructTableScanRec(Scope* parent, std::string structIdent){
    this->scope = parent;

    int size = 0;

    std::map<std::string, struct_attri_t> struct_attribs;

    // change global struct table
    for(auto b : branches){
        auto declState = dynamic_cast<DeclarationStatement&>(*b);
        if(declState.getDeclaration() == NULL) continue;
        auto decl = dynamic_cast<Declaration&>(*declState.getDeclaration());
        
        struct_attri_t data;
        data.first = decl.getDeclarationSpecifierName(); // type
        data.second = size; // offset
        struct_attribs[decl.getIdentifierName()] = data;

        size += decl.getDeclarationSize();

    }

    struct_data_t d;
    d.size = size;
    d.attributes = struct_attribs;

    Program::primitive_sizes["struct " + structIdent] = size;
    Program::structTable[structIdent] = d;

}

// to facilitate typedef macro expansion
std::vector<std::string> DeclarationStatementList::getStatementRawString(){
    std::vector<std::string> v;
    for(auto b : branches){
        auto declState = dynamic_cast<DeclarationStatement&>(*b);
        if(declState.branches[0] != NULL){
            // not empty decl
            auto decl = dynamic_cast<Declaration&>(*declState.branches[0]);
            
            std::string s = "";

            s += decl.getDeclarationSpecifierName() + " " + decl.getIdentifierName();
            v.push_back(s);
        }
    }
    return v;
}