#pragma once
#include "ast_node.hpp"
#include "ast_types.hpp"

#include <string>

// { $$ = new Expression($1); } identifier is a strptr
// { $$ = new Expression($1); } constant is a double

class Expression : public Node {
protected:
    std::string type;
    std::string value;
    std::string opr;
    bool isFunction;
    char character;
    GC_t constantValue;
    std::string stringLit;    
    std::string strLabel;

public:

    // Constructors
    Expression(char* c);
    Expression(bool dummy, std::string* s);

    Expression(Node* constant);
    Expression(const std::string* identifier);
    
    Expression(Node* unary_op, Node* nodeptr);
    Expression(std::string opr, Node* nodeptr);
    
    Expression(Node* lnodeptr, std::string opr, Node* rnodeptr);

    // postfix
    Expression(Node* postExpr, Node* expr, std::string opr); // for array[expr] f(args)
    Expression(Node* postExpr, std::string* ident, std::string opr); // for struct.member structptr->member
    Expression(Node* postExpr, std::string opr); // for ++ -- f()
    Expression(Node* cond, Node* trueptr, Node* falseptr); // for ternary

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
    void compileFPRec(reg_t destReg);
	void scanRec(Scope* parent);

    std::string getValue();
    std::string getType();
    std::string getOpr();
    GC_t getConstantValue();
};