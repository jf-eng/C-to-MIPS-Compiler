#pragma once

class Scope;
#include "../utils/utils.hpp"
#include "../scope/scope.hpp"
#include <map>
#include <set>
#include <queue>
#include <string>


class Program {

private:
    
    // used to generate unique labels
    static unsigned int labelNumber;
    static unsigned int scopeNumber;

    static std::queue<std::string> freeRegisterPool;
    static std::queue<std::string> freeFPRegisterPool;

    static std::vector<Scope*> scopeVect;
    static void clearLocalVarReg();

public:

    static void pushScope(Scope* scope);
    static std::vector<Scope*> getScope();

    // dictionary of all primitive sizes in C
    static std::map<std::string, unsigned int> primitive_sizes;

    // dictionary of globally defined structs
    static std::map<std::string, struct_data_t> structTable;

    // dictionary of globally defined enums
    static std::map<std::string, enum_data_t> enumTable;
    static std::map<std::string, int> globalEnums;
	
    // hashtable for function signatures in global scope
    static std::map<std::string, func_data_t> funcTable;
    static int getMaxNestedArgSize(std::string functionName);

    // vector for all variables (excluding functions) declared in global scope
    static std::map<std::string, global_var_data_t> globalvarsTable;

    // vector for all local temp doubles or floats
    static std::map<double,std::string>localTempDoubles;
    static std::map<float,std::string> localTempFloats;

    // typdef table
    static std::map<std::string, std::string> typedefTable;

    // string labels table
    static std::map<std::string, std::string> stringTable;

    static void insertTypedef(std::string* identifier, std::string declSpec, bool isPointer);


    static label_t getNewLabel(std::string lab);
    static reg_t getFreeRegister();
    static void killRegister(reg_t toKill, Scope* currentScope);
    static reg_t getFreeFPRegister();
    static reg_t incrementFPReg(reg_t even_reg);
    static label_t getScopeLabel();

    static void compileGlobalvars();
    static void compileLocalData();
    static void compileStrings();


    // [DEBUG]
    static void printFuncTable();
    static void printGlobalVars();
    static void printStructTable();
    static void printEnumTable();
    static void printPrimitiveSizes();
    static void printFreeRegisters();
    static void printTypedefTable();
    static void printLocalData();



private:
    // enforce static obj, eg. cannot instantiate obj
    Program(){} 
};

