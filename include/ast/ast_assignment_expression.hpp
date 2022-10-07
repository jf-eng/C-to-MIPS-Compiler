#pragma once
#include "ast_node.hpp"
#include <string>


class AssignmentExpression : public Node {

public:

    // Constructors

    // only expression, eg i++;
    AssignmentExpression(Node* expr);

    // assignment expression (assignment statement, but without ';')
    // eg. i = i + 1  at end of for loop
    AssignmentExpression(Node* prefixExpr, Node* assignOp, Node* expr);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};