#pragma once
#include "ast_node.hpp"
#include <string>


class DeclarationStatementList : public Node {

public:

    // Constructor
    DeclarationStatementList(Node* initialDeclaration);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    
	void scanRec(Scope* parent);
    void insertNewStructTableScanRec(Scope* parent, std::string structIdent);
    std::vector<std::string> getStatementRawString();

};