#include "ast_declaration.hpp"
#include "ast_declaration_specifier.hpp"
#include "ast_expression.hpp"
#include "ast_primitive_type.hpp"
#include "ast_struct_specifier.hpp"
#include <vector>
#include <cmath>

Declaration::Declaration(Node* nodeptr, std::string* strptr, bool pointer){
    if(pointer){
        classname = "Declaration of pointer";
        this->isPointer = true;
    }
    else{
        classname = "Declaration";
        this->isPointer = false;
    }
    
    this->isGlobal = false;
    this->isArray = false;
    branches.push_back(nodeptr);
    branches.push_back(new Expression(strptr));
}

Declaration::Declaration(Node* nodeptr, std::string* strptr, Node* exprptr){
    classname = "Declaration of array";
    this->isGlobal = false;
    this->isArray = true;
    this->isPointer = false;
    branches.push_back(nodeptr);
    branches.push_back(new Expression(strptr));
    branches.push_back(exprptr);
}

Node* Declaration::getIdentifier(){
    return branches[1];
}

Node* Declaration::getDeclarationSpecifier(){
    return branches[0];
}

unsigned int Declaration::getDeclarationSize(){
    std::string typeName = getDeclarationSpecifierName();
    return Program::primitive_sizes[typeName];
}

void Declaration::compileRec(reg_t destReg, int pointerArithmeticSize){

}


std::string Declaration::getIdentifierName(){
    auto identifier = dynamic_cast<Expression&>(*getIdentifier());
    return identifier.getValue();
}

std::string Declaration::getDeclarationSpecifierName(){
    auto declSpec = dynamic_cast<DeclarationSpecifier&>(*getDeclarationSpecifier());
    return declSpec.getTypename();
}

std::string Declaration::getDeclarationSpecifierStructName(){
    auto declSpec = dynamic_cast<DeclarationSpecifier&>(*getDeclarationSpecifier());
    return declSpec.getStructTypename();
}

// leaf of scanRec
void Declaration::insertVar(Scope* parent){
    this->scope = parent;
	
    auto declSpec = dynamic_cast<DeclarationSpecifier&>(*getDeclarationSpecifier());
    auto identifier = dynamic_cast<Expression&>(*getIdentifier());
    
    // for arrays
    if(this->isArray){
        auto expr = dynamic_cast<Expression&>(*branches[2]);
        // insert into symbolTable
        parent->symbolTable[identifier.getValue()].type = declSpec.getTypename() + " pointer";
        parent->symbolTable[identifier.getValue()].size = 4;
        parent->symbolTable[identifier.getValue()].arraySize = expr.getConstantValue().INT;
        parent->symbolTable[identifier.getValue()].isFunctionArg = false;
        parent->symbolTable[identifier.getValue()].pointedSize = Program::primitive_sizes[declSpec.getTypename()];
        parent->symbolTable[identifier.getValue()].isGlobal = (parent->getParent() == NULL) ? true : false;
        std::cerr << "===== Adding array " << identifier.getValue() << " to " << parent->classname << std::endl;
        parent->localVarsSize += parent->symbolTable[identifier.getValue()].size + parent->symbolTable[identifier.getValue()].size * parent->symbolTable[identifier.getValue()].arraySize;

        // insert into global_var_table if global array
        if(parent->getParent()==NULL){
            global_var_data_t var = {
                parent->symbolTable[identifier.getValue()].type, // type
                parent->symbolTable[identifier.getValue()].size, // size of variable
                0, // alignment
                "", // set to 0 by default (assignment will change it)
                parent->symbolTable[identifier.getValue()].arraySize, // size of array
            };
            Program::globalvarsTable[identifier.getValue()] = var;
        }

        // assign scope for children of x[5]
        for (auto b: branches){
            b->scanRec(parent);
        }
        return;
    }


    // for normal variables
    // if variable is global
    if(parent->getParent() == NULL) {
        global_var_data_t var = {
            getDeclarationSpecifierName(), // type
            getDeclarationSize(), // size of variable
            (unsigned int)std::log2(getDeclarationSize()), // alignment
            "", // set to 0 by default (assignment will change it)
            0,
        };

        Program::globalvarsTable[getIdentifierName()] = var;
    }


    // check struct    
    try {
        auto prim = dynamic_cast<PrimitiveType&>(*declSpec.branches[0]);
        // confirm that its a struct
        if(prim.getType() == "struct"){
            std::cerr << "I AM STRUCT" << std::endl;

            auto structSpec = dynamic_cast<StructSpecifier&>(*prim.branches[0]);
            parent->symbolTable[identifier.getValue()].type = "struct " + structSpec.getIdentifier();
            parent->symbolTable[identifier.getValue()].size = Program::structTable[structSpec.getIdentifier()].size;
            parent->symbolTable[identifier.getValue()].isFunctionArg = false;
            parent->symbolTable[identifier.getValue()].isGlobal = false;
            parent->symbolTable[identifier.getValue()].arraySize = 0;
            // no need to assign free register

            parent->localVarsSize += Program::structTable[structSpec.getIdentifier()].size;

            return;
        }
    } catch (const std::exception& e) {}

    // for typdefed variables
    if(Program::typedefTable.count(declSpec.getTypename()) != 0){
        std::cerr << "I AM TYPEDEF" << std::endl;
        // if the typedef is a struct
        if(Program::typedefTable[declSpec.getTypename()].find("struct") != std::string::npos){
            parent->symbolTable[identifier.getValue()].size = Program::structTable[declSpec.getTypename()].size;
            parent->localVarsSize += Program::primitive_sizes[declSpec.getTypename()] < 4 ? 4 : Program::primitive_sizes[declSpec.getTypename()];

        }
        // it isnt a struct
        else {
            parent->symbolTable[identifier.getValue()].size = Program::primitive_sizes[Program::typedefTable[declSpec.getTypename()]];
            parent->localVarsSize += Program::primitive_sizes[declSpec.getTypename()] < 4 ? 4 : Program::primitive_sizes[declSpec.getTypename()];
        }

    }

    // normal not typedef'ed local variabls
    else {
        // pointer -> size = 4
        if(declSpec.getTypename().find("pointer") != std::string::npos){
            parent->symbolTable[identifier.getValue()].size = 4;
            parent->localVarsSize += 4;
            // parent->symbolTable[identifier.getValue()].pointedSize = Program::primitive_sizes[declSpec.getTypename()];  TODO: clarify this
            Program::primitive_sizes[declSpec.getTypename()] = 4;
        } else { // not pointer
            parent->symbolTable[identifier.getValue()].size = Program::primitive_sizes[declSpec.getTypename()];
            parent->localVarsSize += Program::primitive_sizes[declSpec.getTypename()] < 4 ? 4 : Program::primitive_sizes[declSpec.getTypename()];
        }
    }

    // for regular non-struct variables
    if(this->isPointer){
        parent->symbolTable[identifier.getValue()].type = declSpec.getTypename() + " pointer";
        parent->symbolTable[identifier.getValue()].pointedSize = Program::primitive_sizes[declSpec.getTypename()];
    }
    else{
        // anonymous struct declaration
        if(Program::structTable.count(declSpec.getTypenameRaw()) != 0){
            parent->symbolTable[identifier.getValue()].type = "struct " + declSpec.getTypenameRaw();

        } else {
            parent->symbolTable[identifier.getValue()].type = declSpec.getTypename();
        }

    }
    
    // parent->symbolTable[identifier.getValue()].size = Program::primitive_sizes[declSpec.getTypename()];
    parent->symbolTable[identifier.getValue()].isFunctionArg = false;
    parent->symbolTable[identifier.getValue()].arraySize = 0;
    parent->symbolTable[identifier.getValue()].isGlobal = (parent->getParent() == NULL) ? true : false;

}


void Declaration::insertFunctionVars(Scope* parent, std::string functionName){
    this->scope = parent;

    auto declSpec = dynamic_cast<DeclarationSpecifier&>(*getDeclarationSpecifier());
    auto identifier = dynamic_cast<Expression&>(*getIdentifier());
    
    if(this->isPointer || this->isArray){
        parent->symbolTable[identifier.getValue()].type = declSpec.getTypename() + " pointer";
        parent->symbolTable[identifier.getValue()].size = 4;
        parent->symbolTable[identifier.getValue()].pointedSize = Program::primitive_sizes[declSpec.getTypename()];
        parent->localVarsSize += 4;

    }
    else{
        parent->symbolTable[identifier.getValue()].type = declSpec.getTypename();
        parent->symbolTable[identifier.getValue()].size = Program::primitive_sizes[declSpec.getTypename()];
        parent->localVarsSize += Program::primitive_sizes[declSpec.getTypename()] < 4 ? 4 : Program::primitive_sizes[declSpec.getTypename()];
    }
    parent->symbolTable[identifier.getValue()].isFunctionArg = true;
    parent->symbolTable[identifier.getValue()].isGlobal = false;
    
    func_arg_t arg = {
        identifier.getValue(), // name
        parent->symbolTable[identifier.getValue()].type, // type
        parent->symbolTable[identifier.getValue()].size , // size
        0 // stackOffset
    };

    Program::funcTable[functionName].args.push_back(arg);

    std::cerr << "============= Adding function variable: " << identifier.getValue() << std::endl;
    std::cerr << "============= Adding function variable type: " << parent->symbolTable[identifier.getValue()].type << std::endl;
    std::cerr << "============= Adding " << parent->symbolTable[identifier.getValue()].size << std::endl;
}

void Declaration::scanRec(Scope* parent){
    // only structs declarations use this recursive call path
    this->scope = parent;
    branches[0]->scanRec(parent);
}

