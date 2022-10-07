#pragma once
#include "ast_node.hpp"
#include <string>


class IterationStatement : public Node {
protected:
    std::string type;

    std::string startLabel;
    std::string endLabel;
    std::string continueLabel;

public:

    // Constructor
    IterationStatement(std::string type, Node* condition, Node* body, Node* initialStatement, Node* incrementStatement);
    
    Node* getCondition();
    Node* getBody();
    Node* getInitialStatement();
    Node* getIncrementStatement();

    std::string getType();

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};