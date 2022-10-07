#include "ast_translation_unit.hpp"
#include "ast_declaration.hpp"
#include "ast_function.hpp"
#include "ast_assignment_statement.hpp"
#include <vector>
#include <iostream>

TranslationUnit::TranslationUnit(Node* nodeptr){
	classname = "Translation Unit";
	branches.push_back(nodeptr);
	level0 = new Scope(NULL);
}

void TranslationUnit::compileRec(reg_t destReg, int pointerArithmeticSize){
	// initialise data section (global vars)
	Program::compileGlobalvars();
	// initialise data section for local vars
	Program::compileLocalData();
	
	Program::compileStrings();

	std::cout << ".text" << std::endl;
	// compile each function in global scope
	for(auto b : branches){
		try{
			auto func = dynamic_cast<Function&>(*b);
			func.compileRec(destReg);
		} catch(const std::exception& e) {
			std::cerr << "[DEBUG] Failed while trying to cast global var into function" << std::endl;
			std::cerr << e.what() << std::endl;
		}
	}
}

void TranslationUnit::scanRec(Scope* parent){
	this->scope = parent;
	for(auto b : branches){

		// try to dynamic cast into declaration or assignment

		try {
			auto decl = dynamic_cast<Declaration&>(*b);
			decl.isGlobal = true; // TODO: I don't think this is needed anymore
		}
		catch(const std::exception& e) {
			std::cerr << "[DEBUG] Failed while trying to cast global into declaration" << std::endl;
			std::cerr << e.what() << '\n';
		}
		
		b->scanRec(level0);
	}
}