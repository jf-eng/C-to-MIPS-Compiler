#include "ast_enum_specifier.hpp"
#include "ast_enum_list.hpp"

//******* 3 cases *******//
// 1. enum TRAFFIC_LIGHTS { RED, YELLOW, GREEN }
// 2. enum { RED, YELLOW, GREEM }
// 3. enum TRAFFIC_LIGHTS


EnumSpecifier::EnumSpecifier(std::string* ident, Node* enumList) {
    classname = "Enum Specifier";
    this->identifier = *ident;
    branches.push_back(enumList);

    // 2. anonymous enum
    if(ident == NULL){
        type = "anonymous";
    }

    // 3. referencing enum, ie type declarator case
    else if(enumList == NULL) {
        type = "reference";
    }

    // 1. full declaration of enum
    else {
        type = "full";
    }

}

Node* EnumSpecifier::getEnumList(){
    return branches[0];
}

std::string EnumSpecifier::getIdentifier(){
    return identifier;
}


void EnumSpecifier::compileRec(reg_t destReg, int pointerArithmeticSize){

}


void EnumSpecifier::scanRec(Scope* parent) {
    this->scope = parent;
    // if enum is anonymous or a reference, then it doesn't
    // need to be put into the table

    // but if the enum is of type "full" declaration, then it needs to be
    // put into the enum table & primitive sizes
    std::cerr << "***************HERE" << std::endl;
    if(type == "full"){
        auto enumList = dynamic_cast<EnumList&>(*getEnumList());
        enumList.insertNewEnumTableScanRec(parent, identifier);
        enumList.scanRec(parent);
    } else if (type == "anonymous"){
        // struct 
    } else { // type == "reference"
        std::cerr << "reference to enum: " << identifier << std::endl;
        
    }
    
}