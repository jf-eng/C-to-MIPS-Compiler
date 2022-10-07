#pragma once
#include "ast_node.hpp"
#include <string>


class EnumSpecifier : public Node {

protected:
    std::string type;
    std::string identifier;

public:

    // Constructor
    EnumSpecifier(std::string* ident, Node* enumList);

    Node* getEnumList();
    std::string getIdentifier();


	void compileRec(reg_t destReg, int pointerArithmeticSize);
	void scanRec(Scope* parent);

};