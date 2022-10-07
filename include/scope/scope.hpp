#pragma once

#include "../utils/utils.hpp"

// resolve circular dep
#include "../program/program.hpp"
class Node;

#include "../ast/ast_node.hpp"
#include <map>
#include <string>
#include <iostream>
#include <vector>

class Scope {

protected:
	Scope* parent;
	std::vector<Scope*> children;
	
public:
	Scope(Scope* parent);

	std::string funcName;
	std::string entryJumpLabel;
	std::string exitJumpLabel;
	std::string continueJumpLabel;

    std::string classname;
	// key is variable name; value is ident_data_t
	std::map<std::string, ident_data_t> symbolTable;
	unsigned int localVarsSize;

	// getters
	Scope* getParent();
	std::vector<Scope*> getChildren();
	
	// searching
	// searches scope & parents and returns a ident_data_t object for that variable
	ident_data_t& searchIdent(std::string targetVbl);

	// searches scope & parents scopes for identifier name which is allocated to reg
	std::string searchIdentWithReg(reg_t reg);

	// [DEBUG]
	void printScope();

	// calculate local var size
	int getLocalVarsSizeRec();

	// return vector of identifiers declared in scope (excluding current scope function args) and nested scopes
	void getNestedScopesPtr(std::vector<Scope*>& scopePtrs);
};


