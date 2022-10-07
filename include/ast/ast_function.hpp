#pragma once
#include "ast_node.hpp"
#include <string>
#include "../utils/utils.hpp"

// FUNCTION	: DECLARATION '(' ')' ';' 
// 			| DECLARATION '(' INIT_DECLARATION_LIST ')' ';'
// 			| DECLARATION '(' ')' COMPOUND_STATEMENT 
// 			| DECLARATION '(' INIT_DECLARATION_LIST ')' COMPOUND_STATEMENT

class Function : public Node {

protected:
	std::string function_name;
	int stack_size;
	unsigned int s0_offset;
	unsigned int f20_offset;
	unsigned int padSize;
	unsigned int localVarsSize;
	bool functionDeclaration; // true for lines 6,7. false for lines 8,9
	bool isLeaf;

public:

    // Constructor
    Function(Node* declaration, Node* init_declaration_list,
		Node* compound_statement);

	Node* getDeclaration();
	Node* getInitDeclarationList();
	Node* getCompoundStatement();


	void assignLocalVarStackShiftsRec(int stackShift);
	
	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void scanRec(Scope* parent);
	void pushStack();
	void popStack();
	void insertFunctionSymbol();

};