#pragma once
#include "ast_node.hpp"
#include "ast_labelled_statement.hpp"
#include <string>


class Statement : public Node {

public:
    // Constructor
    Statement(Node* nodeptr);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);
    void getAllLabelled(std::vector<LabelledStatement*>& v);

};