#pragma once
#include "ast_node.hpp"
#include <string>


class TypedefStatement : public Node {

public:
    // Constructor
    TypedefStatement(Node* declSpec, std::string* ident, bool isPointer);

	void compileRec(reg_t destReg, int pointerArithmeticSize);
	void scanRec(Scope* parent);

};