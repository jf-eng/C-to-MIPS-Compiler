#pragma once
#include "ast_node.hpp"
#include <string>


class EnumList : public Node {

public:

    // Constructor
    EnumList(Node* initialEnumerator);

	void compileRec(reg_t destReg, int pointerArithmeticSize);
    
	void scanRec(Scope* parent);
    void insertNewEnumTableScanRec(Scope* parent, std::string enumIdent);

};