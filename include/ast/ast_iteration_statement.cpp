#include "ast_iteration_statement.hpp"
#include "ast_compound_statement.hpp"

IterationStatement::IterationStatement(std::string type, Node* condition, Node* body, Node* initialStatement, Node* incrementStatement) {
	classname = "Iteration Statement";
	this->type = type;
	branches.push_back(condition);
	branches.push_back(body);
	branches.push_back(initialStatement);
	branches.push_back(incrementStatement);
}

Node* IterationStatement::getCondition() { return branches[0]; }
Node* IterationStatement::getBody() { return branches[1]; }
Node* IterationStatement::getInitialStatement() { return branches[2]; }
Node* IterationStatement::getIncrementStatement() { return branches[3]; }

std::string IterationStatement::getType() { return this->type; }

void IterationStatement::compileRec(reg_t destReg, int pointerArithmeticSize) {


	reg_t cond_reg = Program::getFreeRegister();

	if(type=="while"){
		

		std::cout << startLabel << ":" << std::endl;
		getCondition()->compileRec(cond_reg);
		std::cout << "beq " << cond_reg << ",$zero," << endLabel << std::endl;
		std::cout << "nop" << std::endl;
		
		getBody()->compileRec(destReg);
		std::cout << "j " << startLabel << std::endl;
		std::cout << "nop" << std::endl;
		std::cout << endLabel << ":" << std::endl;
	}

	else if(type=="dowhile"){

		std::cout << startLabel << ":" << std::endl;
		getBody()->compileRec(destReg);
		getCondition()->compileRec(cond_reg);

		std::cout << continueLabel << ":" << std::endl;

		std::cout << "beq " << cond_reg << ",$zero," << endLabel << std::endl;
		std::cout << "nop" << std::endl;


		std::cout << "j " << startLabel << std::endl;
		std::cout << "nop" << std::endl;
		std::cout << endLabel << ":" << std::endl;
	}

	else if(type=="for"){

		// do initialisation
		if(getInitialStatement() != NULL){
			getInitialStatement()->compileRec(destReg);
		}

		std::cout << startLabel << ":" << std::endl;

		getCondition()->compileRec(cond_reg);
		std::cout << "beq " << cond_reg << ",$zero," << endLabel << std::endl;
		std::cout << "nop" << std::endl;

		getBody()->compileRec(destReg);

		std::cout << continueLabel << ":" << std::endl;

		if(getIncrementStatement() != NULL){
			getIncrementStatement()->compileRec(destReg);
		}

		std::cout << "j " << startLabel << std::endl;
		std::cout << "nop" << std::endl;
		std::cout << endLabel << ":" << std::endl;	
	}

}

void IterationStatement::scanRec(Scope* parent){
	// for loops need to make the scope, eg int i = 0; part is in local scope
	if(type == "for"){
		this->scope = parent;

		getInitialStatement()->scanRec(parent);
		getCondition()->scanRec(parent);

		if(getIncrementStatement() != NULL){
			getIncrementStatement()->scanRec(parent);
		}
		
		getBody()->scanRec(parent);
		
		this->startLabel = Program::getNewLabel("start_for");
		this->endLabel = Program::getNewLabel("exit_for");
		this->continueLabel = Program::getNewLabel("continue_for");

		// try to cast into compound statement
		if(dynamic_cast<CompoundStatement*>(getBody()->branches[0])){
			auto compound = dynamic_cast<CompoundStatement&>(*getBody()->branches[0]);
			compound.scope->entryJumpLabel = this->startLabel;
			compound.scope->exitJumpLabel = this->endLabel;
			compound.scope->continueJumpLabel = this->continueLabel;
		}

	}
	
	// while and do-while loops can just use compound statement to make scope
	else {

		this->scope = parent;
		getCondition()->scanRec(parent);
		getBody()->scanRec(parent);


		if(type == "while"){
			this->startLabel = Program::getNewLabel("start_while");
			this->endLabel = Program::getNewLabel("exit_while");
			this->continueLabel = this->startLabel;
		} 
		
		else if (type == "dowhile"){
			this->startLabel = Program::getNewLabel("start_dowhile");
			this->endLabel = Program::getNewLabel("exit_dowhile");
			this->continueLabel = Program::getNewLabel("continue_dowhile");
		}

		if(dynamic_cast<CompoundStatement*>(getBody()->branches[0])){
			auto compound = dynamic_cast<CompoundStatement&>(*getBody()->branches[0]);
			compound.scope->entryJumpLabel = this->startLabel;
			compound.scope->exitJumpLabel = this->endLabel;
			compound.scope->continueJumpLabel = this->continueLabel;
		}
	}
}