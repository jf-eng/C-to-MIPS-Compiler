#pragma once
#include "ast_node.hpp"
#include <string>


class Enumerator : public Node {

protected:
    std::string type; // either "autoassign" or "expression"
    std::string identifier;

public:

    // Constructor
    Enumerator(std::string* ident);
    Enumerator(std::string* ident, Node* expr);

    std::string getIdentifier();
    std::string getType();

	void compileRec(reg_t destReg, int pointerArithmeticSize);
    
	void scanRec(Scope* parent);
};