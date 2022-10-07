#include "ast_primitive_type.hpp"
#include "ast_struct_specifier.hpp"
#include "ast_enum_specifier.hpp"
#include <string>

PrimitiveType::PrimitiveType(std::string* strptr){
	classname = "Primitive Type";
    type = *strptr;
	branches.push_back(NULL);
}

PrimitiveType::PrimitiveType(Node* structOrEnum){

	if(dynamic_cast<StructSpecifier*>(structOrEnum)){
		classname = "Primitive Struct";
		type = "struct";
	}

	else if(dynamic_cast<EnumSpecifier*>(structOrEnum)){
		classname = "Primitive Enum";
		type = "enum";
	}

	branches.push_back(structOrEnum);
}

std::string PrimitiveType::getType(){
	return type;
}


void PrimitiveType::compileRec(reg_t destReg, int pointerArithmeticSize) {
	// ??
}

void PrimitiveType::scanRec(Scope* parent){
	this->scope = parent;
	// for structs
	if(type == "struct" || type == "enum"){
		branches[0]->scanRec(parent);
	}

}