#pragma once

#include "ast_node.hpp"
#include <string>



class DeclarationSpecifier : public Node {

public:

    // Constructor
    DeclarationSpecifier(Node* nodeptr);
    std::string getTypename();
    std::string getTypenameRaw();

    std::string getStructTypename();
    
    // for unwrapping typedef macros
    std::string getFullformStructString();

	
    void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};