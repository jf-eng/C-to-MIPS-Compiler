#include "ast_assignment_expression.hpp"
#include "ast_assignment_statement.hpp"

AssignmentExpression::AssignmentExpression(Node* expr){
    classname = "Assignment Expression (Pure Expr)";
    branches.push_back(expr);
}

AssignmentExpression::AssignmentExpression(Node* prefixExpr, Node* assignOp, Node* expr){
    classname = "Assignment Expression (Assign Expr)";
    
    // wrap it as an assignment statement
    branches.push_back(new AssignmentStatement(prefixExpr, assignOp, expr));
}


void AssignmentExpression::compileRec(reg_t destReg, int pointerArithmeticSize){
    std::cerr << "======= scope name " << scope->classname << std::endl;
    branches[0]->compileRec(destReg);
}

void AssignmentExpression::scanRec(Scope* parent){
    this->scope = parent;
    branches[0]->scanRec(parent);
}