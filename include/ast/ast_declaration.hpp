#pragma once
#include "ast_node.hpp"


class Declaration : public Node {

public:
    bool isGlobal;
    bool isArray;
    bool isPointer;

    // Constructor
    Declaration(Node* nodeptr, std::string* strptr, bool pointer);
    Declaration(Node* nodeptr, std::string* strptr, Node* exprptr);
	
    void compileRec(reg_t destReg, int pointerArithmeticSize=0);

    // symbol table insertion, this is scanRec leaf
    void insertVar(Scope* parent);
    void insertFunctionVars(Scope* parent, std::string functionName);

    void scanRec(Scope* parent);
    void insertStruct(Scope* parent, std::string identifier);

    unsigned int getDeclarationSize();

    Node* getDeclarationSpecifier();
    Node* getIdentifier();
    std::string getIdentifierName();
    std::string getDeclarationSpecifierName();
    std::string getDeclarationSpecifierStructName();
};