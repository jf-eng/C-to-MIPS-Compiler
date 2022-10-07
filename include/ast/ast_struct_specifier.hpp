#pragma once
#include "ast_node.hpp"
#include <string>


class StructSpecifier : public Node {

protected:
    std::string type;
    std::string identifier = "";
    std::string anonIdent;

public:

    // Constructor
    StructSpecifier(std::string* ident, Node* declList);

    Node* getDeclarationStatementList();

    void setIdentifier(std::string* ident);
    std::string getIdentifier();
    std::string getRawString();
    std::string getType();


	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);
    void anonymousScanRec(Scope* parent, std::string structIdent);

};