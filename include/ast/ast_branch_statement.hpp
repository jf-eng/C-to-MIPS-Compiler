#pragma once
#include "ast_node.hpp"
#include "ast_labelled_statement.hpp"

#include <string>


class BranchStatement : public Node {
protected:
	std::string type;
	std::vector<LabelledStatement*> switchJumpLabels;
	std::string exitSwitchLabel;
	
public:
    // Constructor
    BranchStatement(Node* condition, Node* true_branch, Node* false_branch);
	BranchStatement(Node* condition, Node* true_branch);
	BranchStatement(std::string type, Node* condition, Node* statement);


	Node* getCondition();
	Node* getTrueBranch();
	Node* getFalseBranch();

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);

};