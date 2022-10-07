#pragma once

#include "ast_node.hpp"
#include "ast_types.hpp"
#include <string>

class Constant : public Node {

public:
	GC_t value;
	std::string type;

    // Constructor
    Constant(short num);
    Constant(unsigned short num);
    Constant(int num);
    Constant(unsigned int num);
    Constant(float num);
    Constant(double num);

	void compileRec(reg_t destReg, int pointerArithmeticSize=0);
	void scanRec(Scope* parent);
    GC_t getConstantValue();

};