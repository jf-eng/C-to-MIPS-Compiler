#include "ast_jump_statement.hpp"

JumpStatement::JumpStatement(std::string type){
    classname = "Jump Statement - " + type;
    this->type = type;
}

std::string JumpStatement::getType(){
    return this->type;
}

void JumpStatement::compileRec(reg_t destReg, int pointerArithmeticSize){
    if(type == "continue"){
        std::cout << "j " << this->scope->continueJumpLabel << std::endl;
        std::cout << "nop" << std::endl;
    }
    else if (type == "break"){
        std::cout << "j " << this->scope->exitJumpLabel << std::endl;
        std::cout << "nop" << std::endl;
    }
}

void JumpStatement::scanRec(Scope* parent){
    this->scope = parent;
}