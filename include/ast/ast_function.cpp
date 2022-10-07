#include "ast_function.hpp"
#include "ast_expression.hpp"
#include "ast_compound_statement.hpp"
#include "ast_declaration.hpp"
#include "ast_init_declaration_list.hpp"
#include <vector>
#include <iostream>

Node* Function::getDeclaration() { return branches[0]; }
Node* Function::getInitDeclarationList() { return branches[1]; }
Node* Function::getCompoundStatement() { return branches[2]; }

Function::Function(Node* declaration, Node* init_declaration_list, Node* compound_statement){
	classname = "Function";
	branches.push_back(declaration);
	branches.push_back(init_declaration_list);
	branches.push_back(compound_statement);
	function_name = dynamic_cast<Declaration&>(*getDeclaration()).getIdentifierName();
	
	if(compound_statement){
		functionDeclaration = false;
	}
	else{
		functionDeclaration = true;
	}
}

void Function::pushStack(){

	// ra
	// fp
	// localvars
	// saved
	// fp ?
	// (opt args > 4) - not in leaf
	// 4 args - not in leaf

	
	/* ==== Calculation of stack size ==== */
	// get function scope data
	
	int maxNestedArgSize = Program::getMaxNestedArgSize(function_name);
	this->isLeaf = Program::funcTable[function_name].nestedFuncs.size() > 0 ? false : true;

	// return address
	int returnSize= 4;

	int savedIntRegSize = 18 * 4;
	int savedFPRegSize = 11 * 4;

	int nestedArgSize = (maxNestedArgSize > 4*4) ? maxNestedArgSize : 4*4;

	int prePaddedStackSize = 4 + returnSize + scope->getLocalVarsSizeRec() + savedIntRegSize + savedFPRegSize + nestedArgSize;

	std::cerr << "============= Local Function Scope Size: " << scope->getLocalVarsSizeRec() << std::endl;

	this->stack_size = ((prePaddedStackSize / 8) + 1) * 8; // rounds stack up to nearest 8 bytes
	this->padSize = this->stack_size - prePaddedStackSize;

	/* ==== CODEGEN ==== */
	int stackShift = this->stack_size;
	
	// allocate stack frame
	std::cout << "addiu $sp,$sp,-" << stackShift << std::endl;

	stackShift -= 4;
	std::cout << "sw $ra," << stackShift << "($sp)" << std::endl;

	// fp
	stackShift -= 4;
	std::cout << "sw $fp," << stackShift << "($sp)" <<std::endl;

	// move $sp to $fp
	std::cout << "add $fp,$sp,$zero" << std::endl;
	

	stackShift -= this->padSize;
	
	std::vector<Scope*> allScopes = {this->scope};
	scope->getNestedScopesPtr(allScopes);
	std::cerr << "========== Nested scope size: " << allScopes.size() << std::endl;
	for (auto &s : allScopes) {

		for(auto &ident : s->symbolTable){
			// only assign stack shift for non function args stacks
			if(!ident.second.isFunctionArg){
				int identArgSize = (ident.second.size < 4) ? 4 : ident.second.size;
				stackShift -= identArgSize + ident.second.size * ident.second.arraySize;
				std::cerr << "assigning addr " << stackShift << " to " << ident.first << std::endl;
				ident.second.stackOffset = stackShift;
			}
		}
	}
	
	// saved int regs
	this->s0_offset = stackShift - 4;
	for(int i = 0; i < 8; i++){
		stackShift -= 4;
		std::cout << "sw $s" << i << "," << stackShift << "($fp)" << std::endl;
	}

	for(int i = 0; i < 10; i++){
		stackShift -= 4;
		std::cout << "sw $t" << i << "," << stackShift << "($fp)" << std::endl;
	}

	// saved fp regs
	this->f20_offset = stackShift - 4;
	for(int i = 0; i < savedFPRegSize/4; i++){
		stackShift -= 4;
		std::cout << "swc1 $f" << 20 + i << "," << stackShift << "($fp)" << std::endl;
	}

	// allocate the corrent "stackOffset", which will be beyond the stackShift
	// calculated above, as its on the previous stack frame
	bool integralTypeFound = false;
	int floatArgReg = 12;
	int argFrameOffset = 0;
	// get arguments from registers and push to stack based on MIPS ABI
	for (long unsigned int i = 0; i < Program::funcTable[function_name].args.size(); i++){
		func_arg_t arg = Program::funcTable[function_name].args[i];
		scope->symbolTable[arg.name].stackOffset = this->stack_size + argFrameOffset;
		argFrameOffset += (arg.size < 4) ? 4 : arg.size;
		
		if(i<4){
			if(!integralTypeFound){
				if(arg.type.find("double")!=std::string::npos && arg.type.find("pointer") == std::string::npos){
					if(floatArgReg<16){
						std::cout<<"swc1 $f"<<floatArgReg++<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
						std::cout<<"swc1 $f"<<floatArgReg++<<","<<scope->symbolTable[arg.name].stackOffset+4<<"($fp)"<<std::endl;
					}
					else if(i<3){
						std::cout<<"sw $a"<<i++<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
						std::cout<<"sw $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset+4<<"($fp)"<<std::endl;
					}
				}
				else if(arg.type.find("float")!=std::string::npos && arg.type.find("pointer") == std::string::npos){
					if(floatArgReg<16){
						std::cout<<"swc1 $f"<<floatArgReg<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
						floatArgReg+=2;
					}
					else{
						std::cout<<"sw $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
					}
				}
				else{
					if(arg.size==1){
						std::cout<<"sb $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
					}else{
						std::cout<<"sw $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
					}
					integralTypeFound = true;
				}
			}
			else{
				if(arg.type.find("double")!=std::string::npos && arg.type.find("pointer") == std::string::npos){
					if(i<3){
						std::cout<<"sw $a"<<i++<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
						std::cout<<"sw $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset+4<<"($fp)"<<std::endl;
					}
				}
				else{
					if(arg.size==1){
						std::cout<<"sb $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
					}else{
						std::cout<<"sw $a"<<i<<","<<scope->symbolTable[arg.name].stackOffset<<"($fp)"<<std::endl;
					}
				}
			}
		}
	}

	// for all arrays, store the address of the start of the array at stackOffset
	for (auto &s : allScopes) {

		for(auto &ident : s->symbolTable){
			// only done for local arrays
			if(!ident.second.isFunctionArg && ident.second.arraySize>0){
				// addrReg contains address of a[0] of array
				reg_t addrReg = Program::getFreeRegister();
				std::cout<<"addiu "<<addrReg<<",$fp,"<<ident.second.stackOffset<<std::endl;
				std::cout<<"addiu "<<addrReg<<","<<addrReg<<","<<ident.second.size<<std::endl;
				std::cout<<"sw "<<addrReg<<","<<ident.second.stackOffset<<"($fp)"<<std::endl;
				Program::killRegister(addrReg,scope);
			}
		}
	}
}

void Function::popStack(){
	std::cerr << "[DEBUG] popstack" << std::endl;

	// label for early function terminate by return statement
	std::cout << function_name << "_endlabel:" << std::endl;

	// move sp back to fp, in case there was movement of $sp
	std::cout << "add $sp,$fp,$zero" << std::endl;

	// pop saved int regs
	for(int i = 0; i < 8; i++){
		std::cout << "lw $s" << i << "," << this->s0_offset - i*4 << "($sp)" << std::endl;
		std::cout << "nop" << std::endl;
	}

	for(int i = 0; i < 10; i++){
		std::cout << "lw $t" << i << "," << this->s0_offset - 32 - i*4 << "($sp)" << std::endl;
		std::cout << "nop" << std::endl;
	}

	// pop saved fp regs
	for(int i = 0; i < 11; i++){
		std::cout << "lwc1 $f" << i+20 << "," << this->f20_offset - i*4 << "($sp)" << std::endl;
		std::cout << "nop" << std::endl;
	}
	
	// pop $ra
	std::cout << "lw $ra," << this->stack_size - 4 << "($sp)" << std::endl;
	std::cout << "nop" << std::endl;

	// pop $fp
	std::cout << "lw $fp," << this->stack_size - 8 << "($sp)" << std::endl;
	std::cout << "nop" << std::endl;

	// move back sp
	std::cout << "addiu $sp,$sp," << this->stack_size << std::endl;
	std::cout << "jr $31" << std::endl;
	std::cout << "nop" << std::endl;
}

void Function::compileRec(reg_t destReg, int pointerArithmeticSize){
	std::cerr << "[DEBUG] Compiling " << classname << std::endl;

	if(!functionDeclaration){
		std::cout << ".globl "<< function_name << std::endl;
		std::cout << function_name << ":" << std::endl;
		
		pushStack();

		getCompoundStatement()->compileRec("$v0");
		
		popStack();
	}
}

void Function::scanRec(Scope* parent){

	// this causes scope to include main as int type with size 4
	// getDeclaration()->scanRec(parent);

	// inserts function signature into the hashmap
	insertFunctionSymbol();
	Scope* scp = new Scope(parent);
	scp->funcName = function_name;
	this->scope = scp; // set scope of Function to be pointing to the function scope (not the parent)
	if(getInitDeclarationList()){
		auto &init_decl = dynamic_cast<InitDeclarationList&>(*getInitDeclarationList());
		init_decl.functionName = function_name;
		init_decl.scanRec(scp);
	}

	if(getCompoundStatement()){
		auto& compStatement = dynamic_cast<CompoundStatement&>(*getCompoundStatement());
		compStatement.scope = scp;
		if(compStatement.getStatementList()){
			compStatement.getStatementList()->scanRec(scp);
		}
	}
}

void Function::insertFunctionSymbol(){

	auto decl = dynamic_cast<Declaration&>(*getDeclaration());
	auto currentReturnType = decl.getDeclarationSpecifierName();
	
	unsigned int currentArgSize = 0, currentArgNum = 0;
	if(getInitDeclarationList()){
		auto initList = dynamic_cast<InitDeclarationList&>(*getInitDeclarationList());
		currentArgSize = initList.getArgSize();
		currentArgNum = initList.getArgNum();
	}

	Program::funcTable[function_name].returnType = currentReturnType;
	Program::funcTable[function_name].argSize = currentArgSize;
	Program::funcTable[function_name].argNum = currentArgNum;
}


