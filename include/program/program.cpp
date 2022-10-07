#include "program.hpp"


unsigned int Program::labelNumber = 0;
unsigned int Program::scopeNumber = 0;

std::vector<Scope*> Program::scopeVect;
std::map<std::string, func_data_t> Program::funcTable;
std::map<std::string, global_var_data_t> Program::globalvarsTable;
std::map<std::string, struct_data_t> Program::structTable;
std::map<std::string, enum_data_t> Program::enumTable;
std::map<std::string, int> Program::globalEnums;
std::map<std::string, std::string> Program::typedefTable;
std::map<double,std::string> Program::localTempDoubles;
std::map<float,std::string> Program::localTempFloats;
std::map<std::string, std::string> Program::stringTable;



std::queue<std::string> Program::freeRegisterPool({
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
	"$t8", "$t9"
});

std::queue<std::string> Program::freeFPRegisterPool({
	"$f4", "$f6", "$f8", "$f16", "$f18", "$f20", 
	"$f22", "$f24", "$f26", "$f28"
});


// used godbolt to check sizesscanRec
// with mips gcc 5.4 -ansi (C90)S
std::map<std::string, unsigned int> Program::primitive_sizes = {
	// chars are packed on the stack
	// use store byte
	{"char", 1},
	{"signed char", 1},
	{"unsigned char", 1},

	// shorts and aliases
	// shorts are packed on the stack (use store halfword)
	{"short", 2},
	{"short int", 2},
	{"signed short", 2},
	{"signed short int", 2},
	{"unsigned short", 2},
	{"unsigned short int", 2},

	// 32-bit ints	
	{"int", 4}, 
	{"signed", 4}, 
	{"signed int", 4}, 
	{"unsigned", 4}, 
	{"unsigned int", 4},

	// same as int
	{"long", 4},
	{"long int", 4},
	{"signed long", 4},
	{"signed long int", 4},
	{"unsigned long", 4},
	{"unsigned long int", 4},

	// 64-bit longs
	{"long long", 8},
	{"long long int", 8},
	{"signed long long", 8},
	{"signed long long int", 8},
	{"unsigned long long", 8},
	{"unsigned long long int", 8},

	// IEEE 754 Floating point
	{"float", 4},
	{"double", 8},
	{"long double", 8},
};


void Program::insertTypedef(std::string* identifier, std::string declSpec, bool isPointer){
	typedefTable[*identifier] = (isPointer) ? "pointer " + declSpec : declSpec;
}


int Program::getMaxNestedArgSize(std::string functionName){
	// returns the max arg size required by nested functions
	int maxNestedArgSize = 0;
	for(auto func : funcTable[functionName].nestedFuncs){
		if(funcTable[func].argSize > maxNestedArgSize){
			maxNestedArgSize = funcTable[func].argSize;
		}
	}

	return maxNestedArgSize;
}

void Program::killRegister(reg_t regToKill, Scope* currentScope){
	// temporary registers are not assigned to any variable in symbol table
	// so no need to modify their "reg" field
	try {
		std::string ident = currentScope->searchIdentWithReg(regToKill);
		ident_data_t& data = currentScope->searchIdent(ident);
		data.reg = ""; // uninitalise register field
	} catch (const std::exception& e) {
		std::cerr << "[DEBUG] Tried to kill register which is not a variable, only temp" << std::endl;
		std::cerr << e.what() << std::endl;
	}
	if(regToKill.find("f") != std::string::npos){
		freeFPRegisterPool.push(regToKill);
	}
	else{
		freeRegisterPool.push(regToKill);
	}
	printFreeRegisters();
	std::cerr << "[DEBUG] Register " << regToKill << " killed." << std::endl;
}

reg_t Program::getFreeRegister(){
	printFreeRegisters();
	// if there are still free registers
	if(!freeRegisterPool.empty()){
		reg_t reg = freeRegisterPool.front();
		std::cerr << "[DEBUG] Assigned register " << reg << std::endl;
		freeRegisterPool.pop();
		return reg;
	}
	else{
		clearLocalVarReg();
		return getFreeRegister();
	}
}

reg_t Program::getFreeFPRegister(){
	// if there are still free registers
	if(!freeFPRegisterPool.empty()){
		reg_t reg = freeFPRegisterPool.front();
		std::cerr << "[DEBUG] Assigned register " << reg << std::endl;
		freeFPRegisterPool.pop();
		return reg;
	}
	else{
		clearLocalVarReg();
		return getFreeFPRegister();
	}
}

reg_t Program::incrementFPReg(reg_t even_reg){
	reg_t oddReg = even_reg;
	oddReg.back() = oddReg.back() + 1;
	return oddReg;
}

void Program::clearLocalVarReg(){
	// empties all registers containing local variables and add them back into the pool
	for(auto &s : scopeVect){
		for(auto &p : s->symbolTable){
			if(p.second.reg != ""){
				if(p.second.reg.find("f")!=std::string::npos){
					freeFPRegisterPool.push(p.second.reg);
				}
				else{
					freeRegisterPool.push(p.second.reg);
				}
				p.second.reg = "";
			}
		}
	}
}

label_t Program::getNewLabel(std::string lab){
    return "_" + std::to_string(labelNumber++) + lab;
}

label_t Program::getScopeLabel(){
	std::cerr << "[DEBUG] Assigned new scope: " << "scope_" + std::to_string(scopeNumber) << std::endl;
	return "scope_" + std::to_string(scopeNumber++);
}


void Program::pushScope(Scope* scope){
	scopeVect.push_back(scope);
}

std::vector<Scope*> Program::getScope(){
	return scopeVect;
}

void Program::compileGlobalvars(){
	std::cout << ".text" << std::endl;
	for(auto& var : globalvarsTable)
		std::cout << ".globl " << var.first << std::endl;

	std::cout << ".data" << std::endl;
	for(auto &var : globalvarsTable){
		std::cout << ".align " << var.second.align << std::endl;
		std::cout << ".size " << var.first <<", " << var.second.size << std::endl;
		std::cout << var.first << ":" << std::endl;

		if(var.second.type.find("pointer")!= std::string::npos){
			// if global array
			for(auto b: var.second.arrayElements){
				if(var.second.size == 4){
					std::cout << ".word " << b.INT << std::endl;
				}
				else if(var.second.size == 8){
					std::cout << ".word " << var.second.val << std::endl;
				}
			}
		}
		else if(var.second.type.find("float")!=std::string::npos){
			std::cout<<".float "<<var.second.val<<std::endl;
		}
		else if(var.second.type.find("double")!=std::string::npos){
			std::cout<<".double "<<var.second.val<<std::endl;
		}
		else{
			// int
			std::cout << ".word " << var.second.val << std::endl;
		}
	}
}

// print out data required for local temp floats, doubles, strings
void Program::compileLocalData(){
	// print out local temp floats
	for(auto float_val : localTempFloats){
		std::cout<<float_val.second<<":"<<std::endl;
		std::cout<<".float "<< float_val.first<<std::endl;
	}
	// print out local temp doubles
	for(auto double_val : localTempDoubles){
		std::cout<<double_val.second<<":"<<std::endl;
		std::cout<<".double "<< double_val.first<<std::endl;
	}
}

void Program::compileStrings(){
	std::cout << ".data" << std::endl;
	for(auto &var : stringTable){
		std::cout << var.first << ":" << std::endl;
		std::cout << ".ascii \"" << var.second << "\\000\"" << std::endl;
	}
}

// [DEBUG Functions]
void Program::printLocalData(){
	// print out local temp floats
	std::cerr<< "============= Floats ============" << std::endl;
	for(auto float_val : localTempFloats){
		std::cerr<<float_val.second<<": "<<float_val.first<<std::endl;
	}
	std::cerr<<std::endl;
	std::cerr<< "============= Doubles ============" << std::endl;
	// print out local temp doubles
	for(auto double_val : localTempDoubles){
		std::cerr<<double_val.second<<": "<<double_val.first<<std::endl;
	}

	std::cerr<<std::endl;
}

void Program::printGlobalVars(){
	std::cerr << "\n";
	for(auto &p : globalvarsTable){
		std::cerr << p.first << ": [" << std::endl;
		std::cerr << "    Type: " << p.second.type << std::endl;
		std::cerr << "    Size: " << p.second.size << std::endl;
		std::cerr << "    Align: " << p.second.align << std::endl;
		std::cerr << "    Val: " << p.second.val << std::endl;
		std::cerr << "    arraySize: " << p.second.arraySize << std::endl;
		std::cerr << "]" << std::endl;
	}
}

void Program::printFuncTable(){	
	std::cerr << "\n";
	for (auto &p : funcTable){
        std::cerr << p.first << " : [" << std::endl;
		std::cerr << "    returnType: " << p.second.returnType << std::endl;
		std::cerr << "    argSize: " << p.second.argSize << std::endl;
		std::cerr << "    argNum: " << p.second.argNum << std::endl;
		std::cerr << "    args: " << p.second.argNum << std::endl;
		for(func_arg_t arg : p.second.args){
			std::cerr << "        name: " << arg.name << std::endl;
			std::cerr << "        type: " << arg.type << std::endl;
			std::cerr << "        size: " << arg.size << std::endl;
			std::cerr << "        stackOffset: " << arg.stackOffset << std::endl;
		}
		std::cerr << "    nestedFunctions: " << std::endl;
		for(std::string f : p.second.nestedFuncs){
			std::cerr << "        " << f << std::endl;
		}
		std::cerr << "]" << std::endl;
	}
}

void Program::printStructTable(){
	std::cerr << "\n";
	for(auto &p : structTable){
		std::cerr << p.first << ": [" << std::endl;
		std::cerr << "    Atrributes:" << std::endl;
		for(auto &structAtrrib : p.second.attributes){
			std::cerr << "        " << structAtrrib.first << " : type=" << structAtrrib.second.first << ", offset=" << structAtrrib.second.second << std::endl;
		}
		std::cerr << "    Size: " << p.second.size << std::endl;
		std::cerr << "]" << std::endl;
	}
}

void Program::printEnumTable(){
	std::cerr << "\n";
	for(auto &p : enumTable){
		std::cerr << p.first << ": [" << std::endl;
		for(auto &e : p.second) {
			std::cerr << "    " << e.first << " : " << e.second << std::endl;
		}
		std::cerr << "]" << std::endl;
	}
	std::cerr << "\n\n==== Global Enum List:" << std::endl;
	for(auto &p : globalEnums) {
		std::cerr << p.first << " : " << p.second << std::endl;
	}
}

void Program::printPrimitiveSizes(){
	std::cerr << "\n";
	for(const auto& p : primitive_sizes){
		std::cerr << p.first << " : " << p.second << std::endl;
	}
}

void Program::printTypedefTable(){
	std::cerr << "\n";
	for(const auto& p : typedefTable){
		std::cerr << "[" << p.first << "] -> [" << p.second << "]" << std::endl;
	}
	std::cerr << std::endl;
}

void Program::printFreeRegisters(){
	std::cerr << "\nFree Registers:" << std::endl;
	auto dupQueue = freeRegisterPool;

	while(!dupQueue.empty()){
		std::cerr << dupQueue.front() << "\t";
		dupQueue.pop();
	}
	std::cerr << std::endl;
}