#pragma once
#include "ast_node.hpp"
#include <string>


class LabelledStatement : public Node {
protected:
    std::string type;

public:
    std::string label;

    // Constructor
    LabelledStatement(Node* expr, Node* statement);

    Node* getExpression();
    Node* getStatement();

    std::string getType();

	void compileRec(reg_t destReg, int pointerArithmeticSize);
	void scanRec(Scope* parent);

};