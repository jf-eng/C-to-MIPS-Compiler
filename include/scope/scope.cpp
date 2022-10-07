#include "scope.hpp"

#include <iostream>

typedef std::map<std::string, ident_data_t>::iterator symIt_t;

// constructor
Scope::Scope(Scope* parent){
	this->parent = parent;
	
	if(parent != NULL)
		parent->children.push_back(this);

	this->classname = Program::getScopeLabel();
	this->localVarsSize = 0;
	if(parent){
		this->funcName = parent->funcName;
	}
	Program::pushScope(this);
}

// getters
Scope* Scope::getParent() { return this->parent; }

std::vector<Scope*> Scope::getChildren() { return this->children; };

// searching
ident_data_t& Scope::searchIdent(std::string targetVbl) {
	std::cerr << "[DEBUG] searching for variable " << targetVbl << " in scope " << classname << std::endl;
	for(auto &p : symbolTable){
		if(p.first == targetVbl) {
			std::cerr << "[DEBUG] FOUND variable " << targetVbl << " in scope " << classname << std::endl;
			return symbolTable[targetVbl];
		}
	}
	
	if(parent == NULL){
		throw std::runtime_error("No more parents to search.");
	}
	// recursive search of parent scopes
	return parent->searchIdent(targetVbl);
}

std::string Scope::searchIdentWithReg(reg_t reg){
	std::cerr << "[DEBUG] searching for register " << reg << " in scope " << classname << std::endl;
	for(auto &p : symbolTable){
		if(p.second.reg == reg){
			std::cerr << "[DEBUG] FOUND register " << reg << " in scope " << classname << std::endl;
			return p.first;
		}
	}
	
	if(parent == NULL){
		throw std::runtime_error("No more parents to search.");
	}
	// recursive search of parent scopes
	return parent->searchIdentWithReg(reg);
}


void Scope::printScope(){	
	std::cerr << "\n======= " << classname << " =======" << std::endl;
	if(parent){
		std::cerr << "Parent: " << parent->classname << std::endl;
	}
	std::cerr << "Local Variable Size: " << localVarsSize << std::endl;
	std::cerr << "Symbol Table:" << std::endl;
	for (auto &p : symbolTable){
        std::cerr << p.first << " : [" << std::endl;
		std::cerr << "    Type: " << p.second.type << std::endl;
		std::cerr << "    Register: " << p.second.reg << std::endl;
		std::cerr << "    Size: " << p.second.size << std::endl;
		std::cerr << "    arraySize: " << p.second.arraySize << std::endl;
		std::cerr << "    StackOffset: " << p.second.stackOffset << std::endl;
		std::cerr << "    pointedSize: " << p.second.pointedSize << std::endl;
		std::cerr << std::boolalpha << "    IsFunctionArg: " << p.second.isFunctionArg << std::endl;
		std::cerr << std::boolalpha << "    IsGlobal: " << p.second.isGlobal << std::endl;
		std::cerr << "]" << std::endl;
	}
}

int Scope::getLocalVarsSizeRec(){
	// calculate local var size of scope & nested scopes
	if(children.size() == 0){
		return this->localVarsSize;
	}
	int acc = 0;
	for(auto child : children) acc += child->getLocalVarsSizeRec();
	return localVarsSize + acc;
}

void Scope::getNestedScopesPtr(std::vector<Scope*>& scopePtrs){
	// return vector of all nested scope pointers
	if(children.size() == 0){
		return;
	}

	for(auto child : children){
		scopePtrs.push_back(child);
		child->getNestedScopesPtr(scopePtrs);
	}
}