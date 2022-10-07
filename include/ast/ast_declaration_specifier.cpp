#include "ast_declaration_specifier.hpp"
#include "ast_primitive_type.hpp"
#include "ast_struct_specifier.hpp"
#include <string>
#include <vector>

DeclarationSpecifier::DeclarationSpecifier(Node* nodeptr){
	classname = "Declaration Specifier";
    branches.push_back(nodeptr);
}

// gets typename, if the type is typedef'd it will decode it
std::string DeclarationSpecifier::getTypename(){
	std::string rawName = getTypenameRaw();
	// if this name is in the typdef table
	if(Program::typedefTable.count(rawName)){
		std::string trueName = Program::typedefTable[rawName];
		std::string newName = "";
		while(newName != trueName){
			if(Program::typedefTable.count(trueName)){
				newName = Program::typedefTable.at(trueName);
			} else {
				newName = trueName;
			}
		}
		return newName;
	} 
	// normal variable
	else {
		return rawName;
	}
}

// gets raw typename
std::string DeclarationSpecifier::getTypenameRaw(){
	// basecase: there is no more declaration specifier to unwrap
	if(branches.size() == 1){
		return dynamic_cast<PrimitiveType&>(*branches[0]).type;
	}

	auto decl_spec = dynamic_cast<DeclarationSpecifier&>(*branches[0]);
	auto prim_type = dynamic_cast<PrimitiveType&>(*branches[1]);
	
	return decl_spec.getTypename() + " " + prim_type.type; 
}

std::string DeclarationSpecifier::getFullformStructString(){
	auto structSpec = dynamic_cast<StructSpecifier&>(*branches[0]->branches[0]);
	return structSpec.getRawString();
}

std::string DeclarationSpecifier::getStructTypename(){
	try {
		return dynamic_cast<StructSpecifier&>(*branches[0]->branches[0]).getIdentifier();
	} catch (const std::exception& e) {
		throw std::runtime_error("[ERROR] primitive is not a struct");
	}
}

void DeclarationSpecifier::compileRec(reg_t destReg, int pointerArithmeticSize) {
	// ??
}

void DeclarationSpecifier::scanRec(Scope* parent){
	this->scope = parent;

	// for structs
	auto prim = dynamic_cast<PrimitiveType&>(*branches[0]);
	if(prim.type == "struct" || prim.type == "enum"){
		branches[0]->scanRec(parent);
	}

}