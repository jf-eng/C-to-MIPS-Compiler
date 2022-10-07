#include "ast_statement.hpp"
#include "ast_statement_list.hpp"
#include "ast_compound_statement.hpp"
#include <vector>

Statement::Statement(Node* nodeptr){
	classname = "Statement";
	branches.push_back(nodeptr);
}

void Statement::compileRec(reg_t destReg, int pointerArithmeticSize){
	branches[0]->compileRec(destReg);
}

void Statement::scanRec(Scope* parent){
	this->scope = parent;

	branches[0]->scanRec(parent);
}

void Statement::getAllLabelled(std::vector<LabelledStatement*>& v){

	// singlar labeled statement of switch
	if(dynamic_cast<LabelledStatement*>(branches[0])){
		auto labelled = dynamic_cast<LabelledStatement*>(branches[0]);
		v.push_back(labelled);
	}

	// compound statement switch
	else {
		auto compound = dynamic_cast<CompoundStatement&>(*branches[0]);
		if(compound.branches[0] != NULL){
			// statement list exists
			auto statementList = dynamic_cast<StatementList&>(*compound.branches[0]);
			for(auto s : statementList.branches){
				// check if its labelled

				if(dynamic_cast<LabelledStatement*>(s->branches[0])){
					// if yes, push to v
					auto labelled = dynamic_cast<LabelledStatement*>(s->branches[0]);
					v.push_back(labelled);
				}
			}
		}
	}

}