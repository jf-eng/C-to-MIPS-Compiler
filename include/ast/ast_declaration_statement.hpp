#pragma once
#include "ast_node.hpp"
#include <string>


class DeclarationStatement : public Node {

protected:
    bool hasDeclaration;
public:

    // Constructor
    DeclarationStatement(Node* nodeptr);
    Node* getDeclaration();

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    
    void scanRec(Scope* parent);

};