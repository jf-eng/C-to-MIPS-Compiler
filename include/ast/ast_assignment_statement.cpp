#include "ast_assignment_statement.hpp"
#include "ast_assignment_operator.hpp"
#include "ast_declaration.hpp"
#include "ast_expression.hpp"
#include "ast_array_initializer_list.hpp"
#include "ast_constant.hpp"
#include "ast_types.hpp"
#include <vector>

AssignmentStatement::AssignmentStatement(Node* dest, Node* op, Node* expr){
	if(dynamic_cast<Expression*>(dest)){
		// if not an expression
		classname = "Assignment to ";
		hasDeclaration = false;
	}
	else{
		classname = "Assignment Statement w/ Declaration";
		hasDeclaration = true;
	}
	branches.push_back(dest);
	branches.push_back(op);
	branches.push_back(expr);
	assignmentOp = dynamic_cast<AssignmentOperator&>(*op).getAssignmentOperator();
	arrayInit = false;
	pointerArithmeticSize = 0;

}

// For array init list, int x[5] = {1,2,3,4,5};
AssignmentStatement::AssignmentStatement(Node* dest, Node* expr){
	classname = "Assignment of array init";
	hasDeclaration = true;
	arrayInit = true;
	branches.push_back(dest);
	branches.push_back(expr);
	pointerArithmeticSize = 0;
}

Node* AssignmentStatement::getLeftOperand(){
	return branches[0];
}

Node* AssignmentStatement::getRightOperand(){
	return branches[2];
}

Node* AssignmentStatement::getDeclaration() {
	return branches[0];
}

Node* AssignmentStatement::getIdentifier(){
	return branches[0];
}

void AssignmentStatement::scanRec(Scope* parent){
	this->scope = parent;
	if(hasDeclaration){
		auto decl = dynamic_cast<Declaration&>(*getDeclaration());
		decl.insertVar(parent);
	}
	else{
		getIdentifier()->scanRec(parent);
	}

	// global scope assignment
	if(parent->getParent() == NULL){
		// if its a global scope assignment, whats on the right of assignment
		// operator must be a constant
		if(arrayInit){
			// if global array initialiser list
			auto decl = dynamic_cast<Declaration&>(*getDeclaration());
			// ARRAY_INITIALIZER_LIST -> EXPRESSION_LIST -> branches of EXPRESSION_LIST
			for(auto b: branches[1]->branches[0]->branches){
				Program::globalvarsTable[decl.getIdentifierName()].arrayElements.push_back(dynamic_cast<Expression&>(*b).getConstantValue());
			}
		}
		else{
			// do mini interpreter
			auto con = dynamic_cast<Constant&>(*getRightOperand()->branches[0]);

			auto decl = dynamic_cast<Declaration&>(*getDeclaration());
			global_var_data_t &var = Program::globalvarsTable[decl.getIdentifierName()];
			
			if(var.type.find("int")!=std::string::npos){
				var.val = std::to_string(con.value.INT);
			}
			else if(var.type.find("double")!=std::string::npos){
				var.val = std::to_string(con.value.DOUBLE);
			}
			else if(var.type.find("float")!=std::string::npos){
				var.val = std::to_string(con.value.FLOAT);
			}
		}
	}
	else{
		// attach scope to Expression of AssignmentStatement
		if(!arrayInit){
			getRightOperand()->scanRec(parent);
		}
		else{
			branches[1]->scanRec(parent);
		}
	}
}

// examines left expression and gets its type and base name. (assigned to member attribute)
void AssignmentStatement::setLeftOperandType(){
	if(hasDeclaration){
		auto decl = dynamic_cast<Declaration&>(*getDeclaration());
		this->leftOperandBaseName = decl.getIdentifierName();
		std::string type = decl.getDeclarationSpecifierName();
		
		bool structCondition = 	type.find("struct") != std::string::npos || 
								Program::structTable.count(type) != 0;
		
		if(structCondition){
			this->leftOperandType = "struct";
		}
		else if(scope->searchIdent(this->leftOperandBaseName).type.find("pointer") != std::string::npos){
			// (ie int *p = &a + 1;)
			this->leftOperandType = "pointer";

			std::string ptrType = scope->searchIdent(this->leftOperandBaseName).type;
			std::string type = ptrType.substr(0, ptrType.length()-8);
			
			if(Program::typedefTable.count(type)){
				type = Program::typedefTable[type];
			}

			this->pointerArithmeticSize = Program::primitive_sizes[type];
		}
		else if(scope->searchIdent(this->leftOperandBaseName).type.find("double") != std::string::npos){
			this->leftOperandType = "double";
		}
		else if(scope->searchIdent(this->leftOperandBaseName).type.find("float") != std::string::npos){
			this->leftOperandType = "float";
		}
		else{
			this->leftOperandType = "identifier";
		}
	}
	else{
		auto leftExpr = dynamic_cast<Expression&>(*getIdentifier());
		this->leftOperandBaseName = leftExpr.getValue();
		this->leftOperandOp = leftExpr.getOpr();
		bool structCondition;
		
		// if left operand is not a simple x 
		if(this->leftOperandBaseName == ""){
			// if left operand is x[0],x.a,x->a, get base x
			if(dynamic_cast<Expression*>(leftExpr.branches[0])){
				this->leftOperandBaseName = dynamic_cast<Expression&>(*leftExpr.branches[0]).getValue();
			}
			// if is dereference pointer, get pointer identifer (ie *x = 20;)
			else{
				this->leftOperandBaseName = branches[0]->getIdent();
			}

			std::string leftOpType = scope->searchIdent(this->leftOperandBaseName).type;
			structCondition = 	leftOpType.find("struct") != std::string::npos || 
									Program::structTable.count(leftOpType) != 0;

			if(structCondition){
				this->leftOperandType = "struct";
			}
			else if(scope->searchIdent(this->leftOperandBaseName).type.find("pointer") != std::string::npos){
				// dereference pointer (ie *p = 30;)
				this->leftOperandType = "pointer";
				this->pointerArithmeticSize = 0;
			}
		}
		else{
			if(scope->searchIdent(this->leftOperandBaseName).type.find("pointer") != std::string::npos){
				// pointer arithmetic (ie p = p+1; where p is a pointer)
				this->leftOperandType = "pointer";
				std::string ptrType = scope->searchIdent(this->leftOperandBaseName).type;
				std::string type = ptrType.substr(0, ptrType.length()-8);
				
				if(Program::typedefTable.count(type)){
					type = Program::typedefTable[type];
				}

				this->pointerArithmeticSize = Program::primitive_sizes[type];
			}
			else if(scope->searchIdent(this->leftOperandBaseName).type.find("double") != std::string::npos){
			this->leftOperandType = "double";
			}
			else if(scope->searchIdent(this->leftOperandBaseName).type.find("float") != std::string::npos){
				this->leftOperandType = "float";
			}
			else{
				this->leftOperandType = "identifier";
			}
		}
	}

}

void AssignmentStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
	setLeftOperandType();
	reg_t rightOperandReg;
	reg_t leftOperandReg;

	if(this->leftOperandType == "float" || this->leftOperandType == "double"){
		rightOperandReg = Program::getFreeFPRegister();
	}
	else{
		rightOperandReg = Program::getFreeRegister();
	}

	std::string structBase;
	std::string structMember;
	std::string structType;
	int structOffset;

	// set appropriate registers
	if(hasDeclaration){
		std::string type = dynamic_cast<Declaration&>(*getDeclaration()).getDeclarationSpecifierName();
		if(this->leftOperandType == "float" || this->leftOperandType == "double"){
			leftOperandReg = Program::getFreeFPRegister();
		}
		else{
			leftOperandReg = Program::getFreeRegister();
		}

		// left operand is not of struct type
		if(type.find("struct") == std::string::npos){
			// change reg field of newly made live variable
			this->scope->symbolTable[this->leftOperandBaseName].reg = leftOperandReg;
		}
	}
	else{

		leftOperandReg = scope->searchIdent(this->leftOperandBaseName).reg;

		if(leftOperandReg == ""){
			if(this->leftOperandType == "float" || this->leftOperandType == "double"){
				leftOperandReg = Program::getFreeFPRegister();
			}
			else{
				leftOperandReg = Program::getFreeRegister();
			}			
		}
	}

	/* HANDLE ARRAY INIT LIST, x[5] = {1,2,3,4,5*5}; */
	if(arrayInit){
		int arrayStackOffset = this->scope->searchIdent(this->leftOperandBaseName).stackOffset;
		int arrayElementSize = this->scope->searchIdent(this->leftOperandBaseName).pointedSize;
		int arrayNumOfElements = this->scope->searchIdent(this->leftOperandBaseName).arraySize;
		auto arrayInitListPtr = dynamic_cast<ArrayInitializerList&>(*branches[1]);
		reg_t addrReg = Program::getFreeRegister();
		reg_t sizeOfTypeReg = Program::getFreeRegister();

		// PREFIX EXPR -> EXPRESSION ; ie (x[a+1]), retrieving a+1
		// Calculate the largest offset for the array; Offset of last element
		branches[0]->branches[2]->compileRec(addrReg);

		std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
		std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
		std::cout << "mflo " << addrReg << std::endl;
		std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
		std::cout << "nop"<<std::endl;
		std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;

		// push to stack
		for (int i = arrayNumOfElements - 1; i >= 0; i--){
			arrayInitListPtr.getExpressionAtIndex(i)->compileRec(rightOperandReg);
			std::cout << "addi " << addrReg << "," << addrReg << ",-"<<arrayElementSize<<std::endl;
			std::cout << "sw " << rightOperandReg << ",0"<<"("<<addrReg<<")"<<std::endl;
		}
		
		Program::killRegister(leftOperandReg, this->scope);
		Program::killRegister(rightOperandReg, this->scope);
		Program::killRegister(sizeOfTypeReg, this->scope);
		Program::killRegister(addrReg, this->scope);

		return;
	}

	// compute new value of local vbl
	if(assignmentOp == "="){
		getRightOperand()->compileRec(leftOperandReg,this->pointerArithmeticSize);
	}
	else{
		getLeftOperand()->compileRec(leftOperandReg,this->pointerArithmeticSize);
		getRightOperand()->compileRec(rightOperandReg, this->pointerArithmeticSize);

		if(this->leftOperandType == "double" || this->leftOperandType == "float"){
			if(assignmentOp == "+="){
				std::cout << "add.d " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "-="){
				std::cout << "sub.d " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "*="){
				std::cout << "mul.d " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
				
			}
			else if(assignmentOp == "/="){
				std::cout << "div.d " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
		}
		else{
			if(assignmentOp == "+="){
				std::cout << "add " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "-="){
				std::cout << "sub " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "*="){
				std::cout << "mult " << leftOperandReg << "," << rightOperandReg << std::endl;
				std::cout << "mflo " << leftOperandReg << std::endl;
			}
			else if(assignmentOp == "/="){
				std::cout << "div " << leftOperandReg << "," << rightOperandReg << std::endl;
				std::cout << "mflo " << leftOperandReg << std::endl;
			}
			else if(assignmentOp == "%="){
				std::cout << "div " << leftOperandReg << "," << rightOperandReg << std::endl;
				std::cout << "mfhi " << leftOperandReg << std::endl;
			}
			else if(assignmentOp == "&="){
				std::cout << "and " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "|="){
				std::cout << "or " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "^="){
				std::cout << "xor " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == "<<="){
				std::cout << "sllv " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}
			else if(assignmentOp == ">>="){
				std::cout << "srlv " << leftOperandReg << "," << leftOperandReg << "," << rightOperandReg << std::endl;
			}

		}
	}


	/* PUSH TO STACK */
	// is struct
	if(this->leftOperandType == "struct"){

		unsigned int stackOffset = scope->searchIdent(this->leftOperandBaseName).stackOffset;
		std::string structType = scope->searchIdent(this->leftOperandBaseName).type;


		auto postfix = dynamic_cast<Expression&>(*getDeclaration());
		structMember = dynamic_cast<Expression&>(*(postfix.branches[1])).getValue();

		
		if(structType.find("struct") != std::string::npos){
			structType = structType.substr(7, std::string::npos);
		}


		std::cerr << "**************** (ASSIGN STATEMENT) STRUCT TYPE: " << structType << std::endl;
		std::cerr << "**************** (ASSIGN STATEMENT) STRUCT MEMBER: " << structMember << std::endl;
		std::cerr << "**************** (ASSIGN STATEMENT) STRUCT MEMBER OFFSET: " << Program::structTable[structType].attributes[structMember].second << std::endl;


		structOffset = stackOffset + Program::structTable[structType].attributes[structMember].second;
		
		std::cout<< "sw "<<leftOperandReg<<","<<structOffset<<"($fp)" << std::endl;
	}
	else if(this->leftOperandType == "pointer"){
		ident_data_t ident_data = this->scope->searchIdent(this->leftOperandBaseName);

		// if p[2], indexing operation
		if(this->leftOperandOp=="[]" && !hasDeclaration){

			reg_t addrReg = Program::getFreeRegister();
			reg_t sizeOfTypeReg = Program::getFreeRegister();
		// PREFIX EXPR -> EXPRESSION ; ie (x[a+1]), retrieving a+1
			branches[0]->branches[1]->compileRec(addrReg);

			int arrayStackOffset = ident_data.stackOffset;
			int arrayElementSize = ident_data.size;
			std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
			std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
			std::cout << "mflo " << addrReg << std::endl;

			if(!ident_data.isGlobal){
				std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
				std::cout << "nop"<<std::endl;
				std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;
				if(arrayElementSize == 1) {
					std::cout << "sb " << leftOperandReg << ",0("<<addrReg<<")"<<std::endl;
				} else if (arrayElementSize == 2) {
					std::cout << "sh " << leftOperandReg << ",0("<<addrReg<<")"<<std::endl;
				} else {
					std::cout << "sw " << leftOperandReg << ",0("<<addrReg<<")"<<std::endl;
				}
				
			}
			else{
				// rightOperandReg below is just a convenient temp reg used to store the start address of global var
				std::cout<<"lui "<<rightOperandReg<<",\%hi("<<this->leftOperandBaseName<<")"<<std::endl;
				std::cout<<"addiu "<<rightOperandReg<<","<<rightOperandReg<<",\%lo("<<this->leftOperandBaseName<<")"<<std::endl;
				std::cout << "add " << rightOperandReg << "," << addrReg << ","<<rightOperandReg<< std::endl;

				std::cout << "sw " << leftOperandReg << ",0(" <<rightOperandReg<<")"<<std::endl;
			}

			Program::killRegister(sizeOfTypeReg, this->scope);
			Program::killRegister(addrReg, this->scope);
		}
		// if *p = 30; *(p+1) = 3;
		else if(this->leftOperandOp == "*" && !hasDeclaration){
			// updating local vbl at address pointed to by p
			int leftPointerArithmeticSize = ident_data.pointedSize;
			branches[0]->branches[1]->compileRec(rightOperandReg, leftPointerArithmeticSize);
			std::cout<<"sw "<<leftOperandReg<<",0("<<rightOperandReg<<")"<<std::endl;
		}
		// int *p = &a + 1; or p = p+1; where p is a pointer)
		else{
			// update p
			unsigned int stackOffset = scope->searchIdent(this->leftOperandBaseName).stackOffset;
			std::cout<< "sw "<<leftOperandReg<<","<<stackOffset<<"($fp)" << std::endl;
		}
	}
	else if(this->leftOperandType == "float"){
		if(!this->scope->searchIdent(this->leftOperandBaseName).isGlobal){
			unsigned int stackOffset = scope->searchIdent(this->leftOperandBaseName).stackOffset;
			std::cout<<"cvt.s.d "<<leftOperandReg<<","<<leftOperandReg<<std::endl;
			std::cout<<"swc1 "<<leftOperandReg<<","<<stackOffset<<"($fp)"<<std::endl;
		}
		else{
			reg_t tempAddrReg = Program::getFreeRegister();
			std::cout<<"cvt.s.d "<<leftOperandReg<<","<<leftOperandReg<<std::endl;
			std::cout<<"lui "<<tempAddrReg<<",\%hi("<<this->leftOperandBaseName<<")"<<std::endl;
			std::cout<<"swc1 "<<leftOperandReg<<",\%lo("<<this->leftOperandBaseName<<")("<<tempAddrReg<<")"<<std::endl;
			Program::killRegister(tempAddrReg,scope);
		}
	}
	else if(this->leftOperandType == "double"){
		if(!this->scope->searchIdent(this->leftOperandBaseName).isGlobal){
			unsigned int stackOffset = scope->searchIdent(this->leftOperandBaseName).stackOffset;
			std::cout<<"swc1 "<<leftOperandReg<<","<<stackOffset<<"($fp)"<<std::endl;
			std::cout<<"swc1 "<<Program::incrementFPReg(leftOperandReg)<<","<<stackOffset+4<<"($fp)"<<std::endl;
		}
		else{
			reg_t tempAddrReg = Program::getFreeRegister();
			std::cout<<"lui "<<tempAddrReg<<",\%hi("<<this->leftOperandBaseName<<")"<<std::endl;
			std::cout<<"swc1 "<<leftOperandReg<<",\%lo("<<this->leftOperandBaseName<<"+4)("<<tempAddrReg<<")"<<std::endl;
			std::cout<<"swc1 "<<Program::incrementFPReg(leftOperandReg)<<",\%lo("<<this->leftOperandBaseName<<")("<<tempAddrReg<<")"<<std::endl;
			Program::killRegister(tempAddrReg,scope);
		}
	}
	else{
		// update value of local vbl on stack
		if(!this->scope->searchIdent(this->leftOperandBaseName).isGlobal){
			// if not in global scope, push to stack
			unsigned int stackOffset = scope->searchIdent(this->leftOperandBaseName).stackOffset;
			std::cout<< "sw "<<leftOperandReg<<","<<stackOffset<<"($fp)" << std::endl;
		}
		else{
			// rightOperandReg below is just a convenient temp reg used to store the start address of global var
			std::cout<<"lui "<<rightOperandReg<<",\%hi("<<this->leftOperandBaseName<<")"<<std::endl;
			std::cout<<"sw "<<leftOperandReg<<",\%lo("<<this->leftOperandBaseName<<")("<<rightOperandReg<<")"<<std::endl;
		}
	}

	Program::killRegister(rightOperandReg, this->scope);
	Program::killRegister(leftOperandReg, this->scope);
}

