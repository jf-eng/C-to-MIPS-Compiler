#pragma once
#include "ast_node.hpp"
#include <string>

class PrimitiveType : public Node {

public:
    std::string type;

    // Constructor
    PrimitiveType(std::string* strptr);

    // structs
    PrimitiveType(Node* structSpec);


    std::string getType();
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};