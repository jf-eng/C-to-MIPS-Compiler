#include "ast_struct_specifier.hpp"
#include "ast_declaration_statement_list.hpp"

//******* 3 cases *******//
// 1. struct Book { std::string title; int pages; }
// 2. struct { std::string title; int pages; }
// 3. struct Book


StructSpecifier::StructSpecifier(std::string* ident, Node* declList) {
    classname += "Struct Specifier";
    if(ident != NULL){
        this->identifier = *ident;
    }
    branches.push_back(declList);

    // 2. anonymous struct
    // if the ident is null
    // the identifier will have been set externally
    if(ident == NULL){
        type = "anonymous";
    }

    // 3. referencing struct, ie type declarator case
    // eg. (struct Book) b;
    else if(declList == NULL) {
        type = "reference";
    }

    // 1. full declaration of struct
    else {
        type = "full";
    }
    classname += " (" + type + ")";
}

Node* StructSpecifier::getDeclarationStatementList(){
    return branches[0];
}

std::string StructSpecifier::getIdentifier(){
    return identifier;
}

std::string StructSpecifier::getType(){
    return type;
}


void StructSpecifier::setIdentifier(std::string* ident){
    std::cerr << "****** Set anonymous struct name to " << *ident << std::endl;
    this->anonIdent = *ident;
}

// for expanding typedef
std::string StructSpecifier::getRawString(){
    auto declList = dynamic_cast<DeclarationStatementList&>(*getDeclarationStatementList());
    auto v = declList.getStatementRawString();
    std::string s = "struct { ";
    for (std::string str : v){
        s += str + "; ";
    }
    s += "}";
    return s;
}


void StructSpecifier::compileRec(reg_t destReg, int pointerArithmeticSize){

}


void StructSpecifier::scanRec(Scope* parent) {
    this->scope = parent;
    // if struct is anonymous or a reference, then it doesn't
    // need to be put into the table

    // but if the struct is of type "full" declaration, then it needs to be
    // put into the struct table & primitive sizes
    if(type == "full"){
        auto declList = dynamic_cast<DeclarationStatementList&>(*getDeclarationStatementList());
        declList.insertNewStructTableScanRec(parent, identifier);
        // declList.scanRec(parent); statements in full declaration dont really have scope
    } else if (type == "anonymous"){
        std::cerr << "********** Setting anonymous struct declaration to " << this->anonIdent << std::endl;
        auto declList = dynamic_cast<DeclarationStatementList&>(*getDeclarationStatementList());
        declList.insertNewStructTableScanRec(parent, this->anonIdent);

    } else { // type == "reference"
        std::cerr << "reference to struct: " << identifier << std::endl;
        
    }
    
}
