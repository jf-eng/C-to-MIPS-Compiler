#pragma once
#include "ast_node.hpp"
#include <string>


class AssignmentStatement : public Node {

protected:
    bool hasDeclaration;
    bool arrayInit;
    int pointerArithmeticSize;
    std::string assignmentOp;
    std::string leftOperandType;
    std::string leftOperandBaseName;
    std::string leftOperandOp;

public:

    // for assignment w/ declaration
    AssignmentStatement(Node* dest, Node* op, Node* expr);
	AssignmentStatement(Node* dest, Node* expr);

    // for assignment to existing identifier
    AssignmentStatement(std::string* dest, Node* op, Node* expr);
    Node* getDeclaration();
    Node* getIdentifier();
    Node* getLeftOperand();
    Node* getRightOperand();

    void setLeftOperandType();
    void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
};