#include "ast_expression.hpp"
#include "ast_expression_list.hpp"
#include "ast_constant.hpp"
#include "ast_binary_operator.hpp"
#include "ast_declaration_specifier.hpp"
#include "ast_unary_operator.hpp"
#include "string.h"
#include <vector>
#include <string>
#include <iostream>

Expression::Expression(char* c){
    classname = "Expression (Char)";
    type = "char";
    this->isFunction = false;
    value = "";
    this->character = *c;
}

Expression::Expression(bool dummy, std::string* s){
    classname = "Expression (String Literal)";
    type = "string";
    this->stringLit = *s;
    stringLit = stringLit.substr(1, stringLit.length()-2);
    this->isFunction = false;
    value = "";
}

Expression::Expression(Node* constant){
    classname = "Expression (Constant)";
    type = "constant";
    branches.push_back(constant);
    constantValue = dynamic_cast<Constant&>(*constant).getConstantValue();
    value = "";
    this->isFunction = false;
}

Expression::Expression(const std::string* ident){
    classname = "Expression (Identifier): " + *ident;
    type = "identifier";
    value = *ident;
    this->isFunction = false;
}

// UNARY_OPERATOR PREFIX_EXPRESSION 		
Expression::Expression(Node* unary_op, Node* nodeptr){ 
    classname = "Expression (Unary)";
    type = "unary";
    branches.push_back(unary_op);
    branches.push_back(nodeptr);
    this->opr = dynamic_cast<UnaryOperator&>(*branches[0]).getOp();
    value = "";
    this->isFunction = false;
}

// INC_OP PREFIX_EXPRESSION				 
// DEC_OP PREFIX_EXPRESSION				 
// SIZEOF PREFIX_EXPRESSION				
// SIZEOF '(' DECLARATION_SPECIFIER ')' 	
Expression::Expression(std::string opr, Node* nodeptr){
    classname = "Expression (Prefix)";
    type = "prefix";
    this->opr = opr;
    branches.push_back(NULL);
    branches.push_back(nodeptr);
    value = "";
    this->isFunction = false;
}

Expression::Expression(Node* lnodeptr, std::string opr, Node* rnodeptr){
    classname = "Expression (Binary)";
    type = "binary";
    this->opr = opr;
    branches.push_back(lnodeptr);
    branches.push_back(rnodeptr);
    value = "";
    this->isFunction = false;
}

// POSTFIX_EXPRESSION '(' EXPRESSION_LIST ')'
// POSTFIX_EXPRESSION '[' EXPRESSION ']' 
Expression::Expression(Node* postExpr, Node* expr, std::string opr){
    classname = "Expression (Postfix)";
    type = "postfix";
    this->opr = opr;
    branches.push_back(postExpr);
    branches.push_back(expr);
    value = "";
    if(opr == "()"){
        this->isFunction = true;
    }
    else{
        this->isFunction = false;
    }
}

// POSTFIX_EXPRESSION PTR_OP IDENTIFIER
// POSTFIX_EXPRESSION '.' IDENTIFIER	
Expression::Expression(Node* postExpr, std::string* ident, std::string opr){
    classname = "Expression (Postfix)";
    type = "postfix";
    this->opr = opr;
    this->isFunction = false;
    value = "";
    branches.push_back(postExpr);
    branches.push_back(new Expression(ident));
}

// POSTFIX_EXPRESSION DEC_OP
// POSTFIX_EXPRESSION INC_OP
// POSTFIX_EXPRESSION '(' ')'
Expression::Expression(Node* postExpr, std::string opr){
    classname = "Expression (Postfix)";
    type = "postfix";
    this->opr = opr;
    value = "";
    branches.push_back(postExpr);
    branches.push_back(NULL);

    if(opr == "()"){
        this->isFunction = true;
    }
    else{
        this->isFunction = false;
    }
}

Expression::Expression(Node* cond, Node* trueptr, Node* falseptr){
    classname = "Expression (Ternary)";
    type = "ternary";
    this->opr="?";
    this->isFunction = false;
    value="";
    branches.push_back(cond);
    branches.push_back(trueptr);
    branches.push_back(falseptr);
}


std::string Expression::getValue(){
    return value;
}

GC_t Expression::getConstantValue(){
    return constantValue;
}

std::string Expression::getType(){
    return type;
}

std::string Expression::getOpr(){
    return this->opr;
}

void Expression::compileRec(reg_t destReg, int pointerArithmeticSize){
	std::cerr << "[DEBUG] Compiling " << classname << std::endl;

    // if floating point operation, execute compileFPRec instead.
    if(destReg.find("f")!=std::string::npos){
        compileFPRec(destReg);
        return;
    }

    if(type=="constant") {
        auto c = dynamic_cast<Constant&>(*branches[0]);
        if(c.type == "int"){
            std::cout << "li " << destReg << "," << c.value.INT << std::endl;
        }
        else if(c.type == "uint") {
            std::cout << "li " << destReg << "," << c.value.UINT << std::endl;
        }
        if(pointerArithmeticSize>0){
            reg_t sizeOfDataReg = Program::getFreeRegister();
            std::cout<<"li "<<sizeOfDataReg<<","<<pointerArithmeticSize<<std::endl;
            std::cout<<"mult "<<destReg<<","<<sizeOfDataReg<<std::endl;
            std::cout<<"mflo "<<destReg<<std::endl;

            Program::killRegister(sizeOfDataReg,scope);
        }
    }
    else if(type=="char"){
        std::cout << "li " << destReg << "," << (int)this->character << std::endl;
    }
    else if(type=="string"){
        std::cout << "lui " << destReg << ",\%hi(" << this->strLabel << ")" << std::endl;
        std::cout << "addiu " << destReg << "," << destReg << ",\%lo(" << this->strLabel << ")" << std::endl;
    }
    else if(type=="identifier") {
        ident_data_t ident_data;
        int enumVal;
        bool isEnum = false;
        
        try {
            ident_data = this->scope->searchIdent(value);
        } catch (const std::exception& e) {
            
            std::cerr << "Finding variable [" << value << "] in global enum table." << std::endl;
            if(Program::globalEnums.count(value) != 0){
                isEnum = true;
                enumVal = Program::globalEnums[value];
            } else {
                throw std::runtime_error("[ERROR] Target not found in all scopes + global enum table.");
            }

        }

        if(!isEnum){
            if(ident_data.isGlobal){
                std::cout << "lui " << destReg <<",%hi(" << value << ")" << std::endl;
                std::cout << "lw " << destReg <<",%lo(" << value << ")(" << destReg << ")" << std::endl;
                std::cout << "nop" << std::endl;
            }   
            // not global variable
            else {
                if(ident_data.size==1){
                    std::cout << "lb " << destReg << "," << ident_data.stackOffset << "($fp)" << std::endl;
                }else{
                    std::cout << "lw " << destReg << "," << ident_data.stackOffset << "($fp)" << std::endl;
                }
                std::cout << "nop" << std::endl;
            }
        } 
        // is an enum
        else {
            std::cout << "addiu " << destReg << ",$zero," << enumVal << std::endl; 
        }

    }
    else if(type=="unary") {
        reg_t targetReg = Program::getFreeRegister();
        
        if(opr=="-"){
            branches[1]->compileRec(targetReg, pointerArithmeticSize); // print out assembly for expression
            std::cout << "sub " << destReg << ",$zero," << targetReg << std::endl;
        }
        else if(opr=="!"){
            branches[1]->compileRec(targetReg, pointerArithmeticSize); // print out assembly for expression
            std::cout << "sltiu " << destReg << "," << destReg << ",1" << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        }
        else if(opr=="~"){
            branches[1]->compileRec(targetReg, pointerArithmeticSize); // print out assembly for expression
            std::cout << "nor " << destReg << ",$zero," << targetReg << std::endl;
        }
        else if(opr=="+"){ // does nothing?
            branches[1]->compileRec(destReg, pointerArithmeticSize);
        }
        /* POINTER OPERATIONS */
        else{ 
            std::string identifier = this->getIdent();
            ident_data_t ident_data = this->scope->searchIdent(identifier);
            int stackOffset = this->branches[1]->getStackOffSet();

            if(opr=="&"){
                std::cout<<"addiu "<<destReg<<",$fp,"<<stackOffset<<std::endl;
            }
            else if(opr=="*"){ // dereference
                // to handle cases like return *(p+2), *(p[2]);
                if(ident_data.type.find("pointer")!=std::string::npos){
                    branches[1]->compileRec(destReg, pointerArithmeticSize);
                }
                // for *(x.a)
                else{
                    std::cout<<"lw "<<destReg<<","<<stackOffset<<"($fp)"<<std::endl;
                }

                if(ident_data.type.find("char")!=std::string::npos){
                    std::cout<<"lb "<<destReg<<",0("<<destReg<<")"<<std::endl;
                }else{
                    std::cout<<"lw "<<destReg<<",0("<<destReg<<")"<<std::endl;
                }
                std::cout<<"nop"<<std::endl;
            }
        } 

        Program::killRegister(targetReg, scope);
    }
    else if(type=="prefix") {
        reg_t targetReg = Program::getFreeRegister();

        if(opr=="++"){
            std::string identifier = branches[1]->getIdent();
            // if p++;, p is pointer
            if(this->scope->searchIdent(identifier).type.find("pointer")!=std::string::npos && 
                !this->scope->searchIdent(identifier).arraySize>0){
                branches[1]->compileRec(targetReg, this->scope->searchIdent(identifier).pointedSize);
                std::cout << "addi " << destReg << "," << targetReg << ","<<this->scope->searchIdent(identifier).pointedSize<< std::endl;
            }
            else{
                branches[1]->compileRec(targetReg, pointerArithmeticSize);
                std::cout << "addi " << destReg << "," << targetReg << ",1" << std::endl;
            }

            // update value of local vbl on stack and update reg storing the vbl
            if(!this->scope->searchIdent(identifier).isGlobal){
                // if not in global scope, push to stack
                if(dynamic_cast<Expression&>(*branches[1]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    reg_t sizeOfTypeReg = Program::getFreeRegister();
                    branches[1]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;

                    std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
                    std::cout << "nop"<<std::endl;
                    std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;
                    if(arrayElementSize == 1) {
                        std::cout << "sb " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    } else if (arrayElementSize == 2) {
                        std::cout << "sh " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    } else {
                        std::cout << "sw " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    }
                    Program::killRegister(addrReg,scope);
                    Program::killRegister(sizeOfTypeReg,scope);
                }
                else{
                    unsigned int stackOffset = branches[1]->getStackOffSet();
                    std::cout<<"sw "<<destReg<<","<<stackOffset<<"($fp)" << std::endl;
                }
            }
            else{
                if(dynamic_cast<Expression&>(*branches[1]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    branches[1]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << targetReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << targetReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;
                    std::cout<<"lui "<<targetReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"addiu "<<targetReg<<","<<targetReg<<",\%lo("<<identifier<<")"<<std::endl;
                    std::cout << "add " << targetReg << "," << addrReg << ","<<targetReg<< std::endl;

                    std::cout << "sw " << destReg << ",0(" <<targetReg<<")"<<std::endl;
                    Program::killRegister(addrReg,scope);
                }
                else{
                    std::cout<<"lui "<<targetReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"sw "<<destReg<<",\%lo("<<identifier<<")("<<targetReg<<")"<<std::endl;
                }
            }

        }
        else if(opr=="--") {
            std::string identifier = branches[1]->getIdent();
            if(this->scope->searchIdent(identifier).type.find("pointer")!=std::string::npos && 
                !this->scope->searchIdent(identifier).arraySize>0){
                branches[1]->compileRec(targetReg, this->scope->searchIdent(identifier).pointedSize);
                std::cout << "addi " << destReg << "," << targetReg << ",-"<<this->scope->searchIdent(identifier).pointedSize<< std::endl;
            }
            else{
                branches[1]->compileRec(targetReg, pointerArithmeticSize);
                std::cout << "addi " << destReg << "," << targetReg << ",-1" << std::endl;
            }

            // update value of local vbl on stack and update reg storing the vbl
            if(!this->scope->searchIdent(identifier).isGlobal){
                // if not in global scope, push to stack
                if(dynamic_cast<Expression&>(*branches[1]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    reg_t sizeOfTypeReg = Program::getFreeRegister();
                    branches[1]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;

                    std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
                    std::cout << "nop"<<std::endl;
                    std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;
                    if(arrayElementSize == 1) {
                        std::cout << "sb " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    } else if (arrayElementSize == 2) {
                        std::cout << "sh " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    } else {
                        std::cout << "sw " << destReg << ",0("<<addrReg<<")"<<std::endl;
                    }
                    Program::killRegister(addrReg,scope);
                    Program::killRegister(sizeOfTypeReg,scope);
                }
                else{
                    unsigned int stackOffset = branches[1]->getStackOffSet();
                    std::cout<<"sw "<<destReg<<","<<stackOffset<<"($fp)" << std::endl;
                }
            }
            else{
                if(dynamic_cast<Expression&>(*branches[1]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    branches[1]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << targetReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << targetReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;
                    std::cout<<"lui "<<targetReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"addiu "<<targetReg<<","<<targetReg<<",\%lo("<<identifier<<")"<<std::endl;
                    std::cout << "add " << targetReg << "," << addrReg << ","<<targetReg<< std::endl;

                    std::cout << "sw " << destReg << ",0(" <<targetReg<<")"<<std::endl;
                    Program::killRegister(addrReg,scope);
                }
                else{
                    std::cout<<"lui "<<targetReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"sw "<<destReg<<",\%lo("<<identifier<<")("<<targetReg<<")"<<std::endl;
                }
            }
        }
        else if(opr=="sizeof"){
            int size;
            if(dynamic_cast<DeclarationSpecifier*>(branches[1])){
                // SIZEOF '(' DECLARATION_SPECIFIER ')'
                auto declspec = dynamic_cast<DeclarationSpecifier&>(*branches[1]);
                
                size = Program::primitive_sizes[declspec.getTypename()];
            }
            else{
                // SIZEOF PREFIX_EXPRESSION    prefixExpr->posfixExpr
                auto postfix = dynamic_cast<Expression&>(*branches[1]);
                
                // size of 1 member
                // postfix is primary expression here
                if(postfix.getValue() != ""){
                    size = scope->searchIdent(postfix.getValue()).size;
                } 
                // depth 2 book.length
                else {
                    // primary expression base
                    std::string base = dynamic_cast<Expression&>(*postfix.branches[0]).getValue();
                    std::string member = dynamic_cast<Expression&>(*branches[1]).getValue();

                    std::string type = scope->searchIdent(base).type;
                    std::string memberType = Program::structTable[type].attributes[member].first;
                    
                    size = Program::primitive_sizes[memberType];
                }
            
                
            }
            std::cout << "addiu " << destReg << ",$zero," << size << std::endl;
        }

        Program::killRegister(targetReg, scope);
    }
    else if(type=="postfix") {
        if(opr=="[]"){
            // POSTFIX_EXPRESSION '[' EXPRESSION ']' 
            std::string identifier = dynamic_cast<Expression&>(*branches[0]).getValue();
            ident_data_t ident_data = this->scope->searchIdent(identifier);
            reg_t sizeOfTypeReg = Program::getFreeRegister();
            reg_t addrReg = Program::getFreeRegister();
            
            branches[1]->compileRec(addrReg, pointerArithmeticSize);
            int stackOffset = ident_data.stackOffset;

            std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<ident_data.size<<std::endl;
            std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
            std::cout << "mflo " << addrReg << std::endl;

            if(!ident_data.isGlobal){
                    std::cout << "lw " << destReg << ","<<stackOffset<<"($fp)"<<std::endl;
                    std::cout << "nop" << std::endl;
                    std::cout<<"add "<<addrReg<<","<<addrReg<<","<<destReg<<std::endl;
                    
                    if(ident_data.type.find("char") != std::string::npos){ // lb
                        std::cout <<"add "<<destReg<<",$zero,$zero"<< std::endl;
                        std::cout<<"lb "<<destReg<<",0("<<addrReg<<")"<<std::endl;
                    } else { // lw
                        std::cout<<"lw "<<destReg<<",0("<<addrReg<<")"<<std::endl;
                    }
                    std::cout << "nop" << std::endl;
            }
            else{
                // load element from global array
                std::cout<<"lui "<<destReg<<",\%hi("<<identifier<<")"<<std::endl;
                std::cout<<"addiu "<<destReg<<","<<destReg<<",\%lo("<<identifier<<")"<<std::endl;
                std::cout << "add " << destReg << "," << addrReg << ","<<destReg<< std::endl;

                std::cout<<"lw "<<destReg<<",0("<<destReg<<")"<<std::endl;
                std::cout << "nop" << std::endl;
            }
            Program::killRegister(sizeOfTypeReg, scope);
            Program::killRegister(addrReg, scope);
        }
        else if(opr=="++"){
            reg_t targetReg = Program::getFreeRegister();
            std::string identifier = branches[0]->getIdent();
            if(this->scope->searchIdent(identifier).type.find("pointer")!=std::string::npos && 
                !this->scope->searchIdent(identifier).arraySize>0){
                branches[0]->compileRec(destReg, this->scope->searchIdent(identifier).pointedSize);
                std::cout << "addi " << targetReg << "," << destReg << ","<<this->scope->searchIdent(identifier).pointedSize<< std::endl;
            }
            else{
                branches[0]->compileRec(destReg, pointerArithmeticSize);
                std::cout << "addi " << targetReg << "," << destReg << ",1" << std::endl;
            }


            // if not in global scope, push to stack
            if(!this->scope->searchIdent(identifier).isGlobal){
                if(dynamic_cast<Expression&>(*branches[0]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    reg_t sizeOfTypeReg = Program::getFreeRegister();
                    branches[0]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;

                    std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
                    std::cout << "nop"<<std::endl;
                    std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;
                    if(arrayElementSize == 1) {
                        std::cout << "sb " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    } else if (arrayElementSize == 2) {
                        std::cout << "sh " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    } else {
                        std::cout << "sw " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    }
                    Program::killRegister(addrReg,scope);
                    Program::killRegister(sizeOfTypeReg,scope);
                }
                else{
                    unsigned int stackOffset = branches[0]->getStackOffSet();
                    std::cout<<"sw "<<targetReg<<","<<stackOffset<<"($fp)" << std::endl;
                }
            }
            else{
                if(dynamic_cast<Expression&>(*branches[0]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    branches[0]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << destReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << destReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;
                    std::cout<<"lui "<<destReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"addiu "<<destReg<<","<<destReg<<",\%lo("<<identifier<<")"<<std::endl;
                    std::cout << "add " << destReg << "," << addrReg << ","<<destReg<< std::endl;

                    std::cout << "sw " << targetReg << ",0(" <<destReg<<")"<<std::endl;
                    Program::killRegister(addrReg,scope);
                }
                else{
                    std::cout<<"lui "<<destReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"sw "<<targetReg<<",\%lo("<<identifier<<")("<<destReg<<")"<<std::endl;
                }
            }
            Program::killRegister(targetReg,scope);

        }
        else if(opr=="--") {
            reg_t targetReg = Program::getFreeRegister();
            std::string identifier = branches[0]->getIdent();
            if(this->scope->searchIdent(identifier).type.find("pointer")!=std::string::npos && 
                !this->scope->searchIdent(identifier).arraySize>0){
                branches[0]->compileRec(destReg, this->scope->searchIdent(identifier).pointedSize);
                std::cout << "addi " << targetReg << "," << destReg << ",-"<<this->scope->searchIdent(identifier).pointedSize<< std::endl;
            }
            else{
                branches[0]->compileRec(destReg, pointerArithmeticSize);
                std::cout << "addi " << targetReg << "," << destReg << ",-1" << std::endl;
            }


            // update value of local vbl on stack and update reg storing the vbl
            // if not in global scope, push to stack
            if(!this->scope->searchIdent(identifier).isGlobal){
                if(dynamic_cast<Expression&>(*branches[0]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    reg_t sizeOfTypeReg = Program::getFreeRegister();
                    branches[0]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << sizeOfTypeReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << sizeOfTypeReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;

                    std::cout << "lw "<<sizeOfTypeReg<<","<<arrayStackOffset<<"($fp)"<<std::endl;
                    std::cout << "nop"<<std::endl;
                    std::cout << "add " << addrReg << "," << addrReg << ","<<sizeOfTypeReg<< std::endl;
                    if(arrayElementSize == 1) {
                        std::cout << "sb " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    } else if (arrayElementSize == 2) {
                        std::cout << "sh " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    } else {
                        std::cout << "sw " << targetReg << ",0("<<addrReg<<")"<<std::endl;
                    }
                    Program::killRegister(addrReg,scope);
                    Program::killRegister(sizeOfTypeReg,scope);
                }
                else{
                    unsigned int stackOffset = branches[0]->getStackOffSet();
                    std::cout<<"sw "<<targetReg<<","<<stackOffset<<"($fp)" << std::endl;
                }
            }
            else{
                if(dynamic_cast<Expression&>(*branches[0]).getOpr()=="[]"){
                    reg_t addrReg = Program::getFreeRegister();
                    branches[0]->branches[1]->compileRec(addrReg);

                    int arrayStackOffset = this->scope->searchIdent(identifier).stackOffset;
                    int arrayElementSize = this->scope->searchIdent(identifier).size;
                    std::cout << "addiu " << destReg << ",$zero,"<<arrayElementSize<<std::endl;
                    std::cout << "mult " << destReg << "," << addrReg << std::endl;
                    std::cout << "mflo " << addrReg << std::endl;
                    std::cout<<"lui "<<destReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"addiu "<<destReg<<","<<destReg<<",\%lo("<<identifier<<")"<<std::endl;
                    std::cout << "add " << destReg << "," << addrReg << ","<<destReg<< std::endl;

                    std::cout << "sw " << targetReg << ",0(" <<destReg<<")"<<std::endl;
                    Program::killRegister(addrReg,scope);
                }
                else{
                    std::cout<<"lui "<<destReg<<",\%hi("<<identifier<<")"<<std::endl;
                    std::cout<<"sw "<<targetReg<<",\%lo("<<identifier<<")("<<destReg<<")"<<std::endl;
                }
            }
            Program::killRegister(targetReg,scope);
        }
        else if(opr=="()"){
            // no arguments function call
            std::string nestedFunctionName = dynamic_cast<Expression&>(*branches[0]).getValue();
            if(branches[1] == NULL){
                std::cout << "jal " << nestedFunctionName << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "add "<<destReg<<","<<"$v0,$zero"<<std::endl;
            }
            // function call has args
            else {
                auto exprList = dynamic_cast<ExpressionList&>(*branches[1]);

                // for $a0-3
                for(int i = 0; i < 4 && i < exprList.branches.size(); i++){
                    if(exprList.branches[i]){
                        reg_t res = Program::getFreeRegister();
                        exprList.branches[i]->compileRec(res, pointerArithmeticSize);
                        std::cout << "add $a" << i << "," << res << ",$zero" << std::endl;
                        Program::killRegister(res, scope);
                    }
                }
                if (Program::funcTable[nestedFunctionName].argNum > 4){
                    int offset = 16;
                    for(int i = 4; i < Program::funcTable[nestedFunctionName].argNum; i++){
                        reg_t res = Program::getFreeRegister();
                        exprList.branches[i]->compileRec(res, pointerArithmeticSize);
                        std::cout << "sw " << res << "," << offset << "($fp)" << std::endl;
                        offset += 4;
                        Program::killRegister(res, scope);
                    }
                }

                std::cout << "jal " << nestedFunctionName << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "add "<<destReg<<","<<"$v0,$zero"<<std::endl;
            }
        }
        else if(opr=="."){
            // only 1 level (assume that postfix expr value is ident)
            std::string base = dynamic_cast<Expression&>(*branches[0]).getValue();
            std::string member = dynamic_cast<Expression&>(*branches[1]).getValue();
            
            std::string type = scope->searchIdent(base).type;
            int stackOffset = scope->searchIdent(base).stackOffset;

            if(type.find("struct") != std::string::npos){
                type = type.substr(7, std::string::npos);
            }

            std::cerr << "**************** (EXPR) STRUCT TYPE: " << type << std::endl;
            std::cerr << "**************** (EXPR) STRUCT MEMBER: " << member << std::endl;
            std::cerr << "**************** (EXPR) STRUCT MEMBER OFFSET: " << Program::structTable[type].attributes[member].second << std::endl;

            int structOffset = stackOffset + Program::structTable[type].attributes[member].second;
            
            
            std::cout << "lw " << destReg << "," << structOffset << "($fp)" << std::endl;
        }
        else if(opr=="->"){
            
        }
    }
    else if(type=="binary") {
        reg_t leftReg = Program::getFreeRegister();
        reg_t rightReg = Program::getFreeRegister();
        branches[0]->compileRec(leftReg, pointerArithmeticSize);
        branches[1]->compileRec(rightReg, pointerArithmeticSize);

        if(opr=="+") {
            std::cout << "add " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr=="-") {
            std::cout << "sub " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr=="*") {
            std::cout << "mult " << leftReg << "," << rightReg << std::endl;
            std::cout << "mflo " << destReg << std::endl;
        } else if (opr=="/") {
            std::cout << "div " << leftReg << "," << rightReg << std::endl;
            std::cout << "mflo " << destReg << std::endl;
        } else if (opr=="%") {
            std::cout << "div " << leftReg << "," << rightReg << std::endl;
            std::cout << "mfhi " << destReg << std::endl;
        } else if (opr=="||") {
            std::string left_true_label = Program::getNewLabel("left_true_or");
            std::string right_false_label = Program::getNewLabel("right_false_or");
            std::string exit_or_label = Program::getNewLabel("exit_or");

            std::cout << "bne " << leftReg << ",$zero," << left_true_label << std::endl;
            std::cout << "nop" << std::endl;
            std::cout << "beq " << rightReg << ",$zero," << right_false_label << std::endl;

            std::cout << left_true_label << ":" << std::endl;
            std::cout << "li " << destReg << ",1" << std::endl;
            std::cout << "j " << exit_or_label << std::endl;
            std::cout << "nop" << std::endl;
            
            std::cout << right_false_label << ":" << std::endl;
            std::cout << "li " << destReg << ",0" << std::endl;

            std::cout << exit_or_label << ":" << std::endl;
        } else if (opr=="&&") {
            std::string false_label = Program::getNewLabel("false_and");
            std::string exit_and_label = Program::getNewLabel("exit_and");

            std::cout << "beq " << leftReg << ",$zero," << false_label << std::endl;
            std::cout << "nop" << std::endl;
            std::cout << "beq " << rightReg << ",$zero," << false_label << std::endl;
            std::cout << "li " << destReg << ",1" << std::endl;
            std::cout << "j " << exit_and_label << std::endl;

            std::cout << false_label << ":" << std::endl;
            std::cout << "li " << destReg << ",0" << std::endl;

            std::cout << exit_and_label << ":" << std::endl;
        } else if (opr=="<") {
            std::cout << "slt " << destReg << "," << leftReg <<"," << rightReg << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr==">") {
             std::cout << "slt " << destReg << "," << rightReg <<"," << leftReg << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr=="==") {
            std::cout << "xor " << destReg << "," << leftReg << "," << rightReg << std::endl;
            std::cout << "sltiu " << destReg << "," << destReg << ",1" << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr=="!=") {
            std::cout << "xor " << destReg << "," << leftReg << "," << rightReg << std::endl;
            std::cout << "sltu " << destReg << ",$zero," << destReg << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr=="<=") {
            std::cout << "slt " << destReg << "," << rightReg << "," << leftReg << std::endl;
            std::cout << "xori " << destReg << "," << destReg << ",1" << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr==">=") {
            std::cout << "slt " << destReg << "," << leftReg << "," << rightReg << std::endl;
            std::cout << "xori " << destReg << "," << destReg << ",1" << std::endl;
            std::cout << "andi " << destReg << "," << destReg << ",0x00ff" << std::endl;
        } else if (opr=="&") {
            std::cout << "and " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr=="|") {
            std::cout << "or " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr=="^") {
            std::cout << "xor " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr=="<<") {
            std::cout << "sllv " << destReg << "," << leftReg << "," << rightReg << std::endl;
        } else if (opr==">>") {
            std::cout << "srlv " << destReg << "," << leftReg << "," << rightReg << std::endl;
        }
             
        Program::killRegister(leftReg, scope);
        Program::killRegister(rightReg, scope);
    }
    else if(type=="ternary"){
        reg_t condReg = Program::getFreeRegister();
        branches[0]->compileRec(condReg, pointerArithmeticSize);
        std::string true_label = Program::getNewLabel("true_ternary");
        std::string exit_label = Program::getNewLabel("exit_ternary");

        std::cout << "bne " << condReg << ",$zero," << true_label << std::endl;
        std::cout << "nop" << std::endl;
        branches[2]->compileRec(destReg, pointerArithmeticSize);
        std::cout<<"j "<<exit_label<<std::endl;
        std::cout<<"nop"<<std::endl;

        std::cout<< true_label<<":"<<std::endl;
        branches[1]->compileRec(destReg, pointerArithmeticSize);
        std::cout<<exit_label<<":"<<std::endl;

        Program::killRegister(condReg,scope);
    }
}

void Expression::scanRec(Scope* parent){
	this->scope = parent;

    // if is a function call, update in Program::funcTable
    if(this->isFunction){
        std::string nestedFunctionName = dynamic_cast<Expression&>(*branches[0]).getValue();
        Program::funcTable[this->scope->funcName].nestedFuncs.push_back(nestedFunctionName);
    }

    if(this->type == "string"){
        this->strLabel = Program::getNewLabel("str");
        Program::stringTable[this->strLabel] = this->stringLit;
    }

    for(auto b : branches){
        if(b){
            b->scanRec(parent);
        }
    }
}

void Expression::compileFPRec(reg_t destFPReg){
    if(type=="identifier"){
        ident_data_t ident_data = this->scope->searchIdent(this->value);
        if(ident_data.type.find("float") != std::string::npos){
            if(ident_data.isGlobal){
                reg_t tempAddrReg = Program::getFreeRegister();
                std::cout<<"lui "<<tempAddrReg<<",\%hi("<<this->value<<")"<<std::endl;
                std::cout<<"lwc1 "<<destFPReg<<",\%lo("<<this->value<<")("<<tempAddrReg<<")"<<std::endl;
                std::cout<<"nop"<<std::endl;
                std::cout<<"cvt.d.s "<<destFPReg<<","<<destFPReg<<std::endl;
                Program::killRegister(tempAddrReg, scope);
            }
            else{
                std::cout<<"lwc1 "<<destFPReg<<","<<ident_data.stackOffset<<"($fp)"<<std::endl;
                std::cout<<"nop"<<std::endl;
                std::cout<<"cvt.d.s "<<destFPReg<<","<<destFPReg<<std::endl;
            }
        }
        else if(ident_data.type.find("double") != std::string::npos){
            if(ident_data.isGlobal){
                reg_t tempAddrReg = Program::getFreeRegister();
                std::cout<<"lui "<<tempAddrReg<<",\%hi("<<this->value<<")"<<std::endl;
                std::cout<<"lwc1 "<<destFPReg<<",\%lo("<<this->value<<"+4)("<<tempAddrReg<<")"<<std::endl;
                std::cout<<"nop"<<std::endl;
                std::cout<<"lwc1 "<<Program::incrementFPReg(destFPReg)<<",\%lo("<<this->value<<")("<<tempAddrReg<<")"<<std::endl;
                std::cout<<"nop"<<std::endl;
                Program::killRegister(tempAddrReg, scope);
            }
            else{
                std::cout<<"lwc1 "<<destFPReg<<","<<ident_data.stackOffset<<"($fp)"<<std::endl;
                std::cout<<"nop"<<std::endl;
                std::cout<<"lwc1 "<<Program::incrementFPReg(destFPReg)<<","<<ident_data.stackOffset+4<<"($fp)"<<std::endl;
                std::cout<<"nop"<<std::endl;
            }
        }
        else{
            throw std::runtime_error("Identifier not float/double but compileFPRec() is called!");
        }
    }
    else if(type=="constant"){
        //TODO: need to load from data section, also need to print out .float or .double in data
        // Prob can be done in scanRec, scan thru all the constants and keep track of it
        auto constant = dynamic_cast<Constant&>(*branches[0]);
        if(constant.type=="float"){
            reg_t tempAddrReg = Program::getFreeRegister();
            std::string label = Program::localTempFloats[constant.getConstantValue().FLOAT];
            std::cout<<"lui "<<tempAddrReg<<",%hi("<<label<<")"<<std::endl;
            std::cout<<"lwc1 "<<destFPReg<<",%lo("<<label<<")("<<tempAddrReg<<")"<<std::endl;
            std::cout<<"nop"<<std::endl;
            std::cout<<"cvt.d.s "<<destFPReg<<","<<destFPReg<<std::endl;

            Program::killRegister(tempAddrReg, scope);
        }
        else if(constant.type=="double"){
            reg_t tempAddrReg = Program::getFreeRegister();
            std::string label = Program::localTempDoubles[constant.getConstantValue().DOUBLE];
            std::cout<<"lui "<<tempAddrReg<<",%hi("<<label<<")"<<std::endl;
            //TODO: check if +4 here is correct.
            std::cout<<"lwc1 "<<destFPReg<<",%lo("<<label<<"+4)("<<tempAddrReg<<")"<<std::endl;
            std::cout<<"nop"<<std::endl;
            std::cout<<"lwc1 "<<Program::incrementFPReg(destFPReg)<<",%lo("<<label<<")("<<tempAddrReg<<")"<<std::endl;
            std::cout<<"nop"<<std::endl;

            Program::killRegister(tempAddrReg, scope);
        }
        else{
            throw std::runtime_error("Constant not float/double but compileFPRec() is called!");
        }
    }
    else if(type=="binary"){
        reg_t leftReg = Program::getFreeFPRegister();
        reg_t rightReg = Program::getFreeFPRegister();
        dynamic_cast<Expression&>(*branches[0]).compileFPRec(leftReg);
        dynamic_cast<Expression&>(*branches[1]).compileFPRec(rightReg);

        if(opr=="+"){
            std::cout<<"add.d "<<destFPReg<<","<<leftReg<<","<<rightReg<<std::endl;
        } else if(opr=="-"){
            std::cout<<"sub.d "<<destFPReg<<","<<leftReg<<","<<rightReg<<std::endl;
        } else if(opr=="*"){
            std::cout<<"mul.d "<<destFPReg<<","<<leftReg<<","<<rightReg<<std::endl;
        } else if(opr=="/"){
            std::cout<<"div.d "<<destFPReg<<","<<leftReg<<","<<rightReg<<std::endl;
        } else if(opr=="<"){
            
        } else if(opr==">"){
            
        }
        //TODO: check if C floats support more operators

        Program::killRegister(leftReg, scope);
        Program::killRegister(rightReg, scope);
    }
}