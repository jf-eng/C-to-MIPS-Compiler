#include "inc.hpp"
#include <iostream>
#include <iomanip>
#include <map>


int main()
{   
    // Parse the AST
    Node *ast=parseAST();
    
	std::cerr << "\n\n================= [DEBUG]: AST =================" << std::endl;
	ast->PrettyPrint(std::cerr);

	ast->scanRec(NULL);
	
	std::cerr << "\n\n================= [DEBUG]: AST_SCOPES =================" << std::endl;
	ast->ScopePrettyPrint(std::cerr);

	std::cerr << "\n\n================= [DEBUG]: GLOBAL_VARS =================" << std::endl;
	Program::printGlobalVars();
	
	std::cerr << "\n\n================= [DEBUG]: LOCAL_TEMP_DATA =================" << std::endl;
	Program::printLocalData();

	std::cerr << "\n\n=============== [DEBUG]: TYPEDEF TABLE ===============" << std::endl;
	Program::printTypedefTable();

	std::cerr << "\n\n================= [DEBUG]: FUNC_TABLE =================" << std::endl;
	Program::printFuncTable();

	std::cerr << "\n\n================= [DEBUG]: STRUCT_TABLE =================" << std::endl;
	Program::printStructTable();

	std::cerr << "\n\n================= [DEBUG]: ENUM_TABLE =================" << std::endl;
	Program::printEnumTable();

	std::cerr << "\n\n================= [DEBUG]: SCOPES =================" << std::endl;
	
	auto v = Program::getScope();
	for(auto scp : v) {
		scp->printScope();
	}

	std::cerr << "\n\n================= [DEBUG]: MIPS =================" << std::endl;
	ast->compileRec("");
	
	std::cerr << "\n\n=============== [DEBUG]: SCOPES after pushStack() ===============" << std::endl;
	
	auto y = Program::getScope();
	for(auto scp : y) {
		scp->printScope();
	}

	std::cerr << "\n\n=============== [DEBUG]: PRIMITIVE SIZES ===============" << std::endl;
    Program::printPrimitiveSizes();


	std::cerr << "\n\n*** PROGRAM RAN TO COMPLETION ***" << std::endl;
	
	return 0;
}

