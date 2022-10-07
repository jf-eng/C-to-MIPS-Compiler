#include "ast_branch_statement.hpp"
#include "ast_statement.hpp"
#include "ast_compound_statement.hpp"
#include "ast_expression.hpp"
#include "../program/program.hpp"


BranchStatement::BranchStatement(Node* condition, Node* true_branch, Node* false_branch){
	classname = "Branch Statement - If else";
	type = "ifelse";
	branches.insert(branches.end(), { condition, true_branch, false_branch });
}

BranchStatement::BranchStatement(Node* condition, Node* true_branch){
	classname = "Branch Statement - If only";
	type = "if";
	branches.insert(branches.end(), { condition, true_branch });
}

BranchStatement::BranchStatement(std::string type, Node* condition, Node* statement){
	classname = "Branch Statement - Switch";
	this->type = "switch";
	branches.insert(branches.end(), { condition, statement });
}

Node* BranchStatement::getCondition() { return branches[0]; }

Node* BranchStatement::getTrueBranch() { return branches[1]; }

Node* BranchStatement::getFalseBranch() { return branches[2]; }

void BranchStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
	reg_t cond_reg = Program::getFreeRegister();
	label_t else_label = Program::getNewLabel("else");
	label_t endif_label = Program::getNewLabel("endif");

	// condition
	Node* cond = getCondition();
	cond->compileRec(cond_reg);

	// NOT SWITCH
	if(type != "switch"){

		std::cout << "beq " << cond_reg << ",$zero," << else_label << std::endl;
		std::cout << "nop" << std::endl;
		
		// true branch
		Node* true_br = getTrueBranch();
		true_br->compileRec(destReg);

		if(type == "ifelse"){
			std::cout << "j " << endif_label << std::endl;
			std::cout << "nop" << std::endl;
		}

		// false branch
		std::cout << else_label << ":" << std::endl;

		if(type == "ifelse"){
			Node* false_br = getFalseBranch();
			false_br->compileRec(destReg);
			std::cout << endif_label << ":" << std::endl;
		}
	}

	// SWITCH STATEMENT
	else {

		reg_t tempVal = Program::getFreeRegister();

		for(auto l : switchJumpLabels){
			if(l->getType() == "case"){
				auto expr = dynamic_cast<Expression&>(*l->branches[0]);
				std::cout << "addi " << tempVal << ",$zero," << expr.getConstantValue().INT << std::endl;
				std::cout << "beq " << cond_reg << "," << tempVal << "," << l->label << std::endl;
			} else { // default
				std::cout << "beq $zero,$zero," << l->label << std::endl;
			}
		}

		Program::killRegister(tempVal, scope);

		// compile statement section recursively
		branches[1]->compileRec(destReg);

		std::cout << exitSwitchLabel << ":" << std::endl;

	}

}

void BranchStatement::scanRec(Scope* parent){
	this->scope = parent;

	for(auto b : branches){
		b->scanRec(parent);
	}
	
	// need to look ahead at all statements
	if(type == "switch"){
		auto statement = dynamic_cast<Statement&>(*branches[1]);
		statement.getAllLabelled(switchJumpLabels);
		std::cerr << "**********SIZE OF SWITCH JUMP LABELS: " << switchJumpLabels.size() << std::endl;

		std::string uniqueSwitchId = Program::getNewLabel("switch");

		for(auto l : switchJumpLabels){
			// set labels
			l->label = Program::getNewLabel(uniqueSwitchId);
		}

		exitSwitchLabel = Program::getNewLabel("exit_switch");
		
		// compound statement switch
		if(dynamic_cast<CompoundStatement*>(statement.branches[0])){
			auto compound = dynamic_cast<CompoundStatement&>(*statement.branches[0]);
			compound.scope->exitJumpLabel = exitSwitchLabel;
		} 
	}

}