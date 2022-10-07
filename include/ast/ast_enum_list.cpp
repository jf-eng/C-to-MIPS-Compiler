#include "ast_enum_list.hpp"
#include "ast_enumerator.hpp"
#include "ast_expression.hpp"

EnumList::EnumList(Node* initialEnumerator){
    classname = "Enum List";
    branches.push_back(initialEnumerator);
}


void EnumList::compileRec(reg_t destReg, int pointerArithmeticSize){

}


void EnumList::scanRec(Scope* parent){
    this->scope = parent;
}

void EnumList::insertNewEnumTableScanRec(Scope* parent, std::string enumIdent){
    this->scope = parent;

    enum_data_t enumData;

    int enumValue = 0; // default value

    for(auto b : branches) {
        auto enumerator = dynamic_cast<Enumerator&>(*b);

        if(enumerator.getType() == "autoassign") {
            enumData[enumerator.getIdentifier()] = enumValue;
            Program::globalEnums[enumerator.getIdentifier()] = enumValue;
        }

        else { // enumerator.type == "expression"
            auto expr = dynamic_cast<Expression&>(*enumerator.branches[0]);
            enumData[enumerator.getIdentifier()] = expr.getConstantValue().INT;
            enumValue = expr.getConstantValue().INT;
            Program::globalEnums[enumerator.getIdentifier()] = enumValue;
        }

        enumValue++;
    }

    Program::enumTable[enumIdent] = enumData;

}
