#pragma once
#include <map>
#include <string>
#include <vector>
#include "../ast/ast_types.hpp"

typedef std::string reg_t;
typedef std::string label_t;


//  ===== for variable symbol table ====
typedef struct {
	std::string type; // const long long
	reg_t reg; // reg vbl stored in
	unsigned int size; // size of vbl
	unsigned int stackOffset; // where vbl is stored in stack relative to $sp in the respective scope
	bool isFunctionArg;
	bool isGlobal;
	unsigned int arraySize; // size of array (field only initialised for declaration of arrays, = 0 for all others)
	unsigned int pointedSize; // size of vbl pointed to by pointer
} ident_data_t;


//  ==== for function tables ====
typedef struct {
	std::string name;
	std::string type;
	unsigned int size;
	unsigned int stackOffset;
} func_arg_t;

typedef struct {
	std::string returnType; // type returned by function
	unsigned int argSize; // size of arguments
	unsigned int argNum; // no. of arguments
	std::vector<func_arg_t> args; // argument data
	std::vector<std::string> nestedFuncs; // for other function names which are called within the local scope of the current one
} func_data_t;


//  ==== for global variable data ====
typedef struct {
	std::string type;
	unsigned int size;
	unsigned int align;
	std::string val;
	unsigned int arraySize;
	std::vector <GC_t> arrayElements;
} global_var_data_t;


// ==== for structs ====
// value in map, accessed using identifier of struct

// each struct attribute <type, offset>. 
typedef std::pair<std::string, int> struct_attri_t;

typedef struct {
	int size; // this is also stored into primitive size table
	std::map<std::string, struct_attri_t> attributes; // list of struct attributes
} struct_data_t;


// ==== for enums ====
typedef std::map<std::string, int> enum_data_t;


std::vector<std::string> split(const std::string& s, char delimiter);

std::string join(const std::vector<std::string>& v, char joinchar);