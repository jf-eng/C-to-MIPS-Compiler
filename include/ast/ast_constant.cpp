#include "ast_constant.hpp"

Constant::Constant(short num){
	classname = "Constant (short)";
	type="short";
	value.SHORT = num;
}

Constant::Constant(unsigned short num){
	classname = "Constant (ushort)";
	type="ushort";
	value.USHORT = num;
}

Constant::Constant(int num){
	classname = "Constant (int)";
	type="int";
	value.INT = num;
}

Constant::Constant(unsigned int num){
	classname = "Constant (uint)";
	type="uint";
	value.UINT = num;
}

Constant::Constant(float num){
	classname = "Constant (float)";
	type="float";
	value.FLOAT = num;
}

Constant::Constant(double num){
	classname = "Constant (double)";
	type="double";
	value.DOUBLE = num;
}

void Constant::compileRec(reg_t destReg, int pointerArithmeticSize){

}

void Constant::scanRec(Scope* parent){
	this->scope = parent;

	// if not in global scope
	if(parent->getParent()!=NULL){
		if(this->type=="float"){
			Program::localTempFloats[value.FLOAT]=Program::getNewLabel("float");
		}
		else if(this->type=="double"){
			Program::localTempDoubles[value.DOUBLE]=Program::getNewLabel("double");
		}
	}
}

GC_t Constant::getConstantValue(){
	return this->value;
}