#pragma once

#include "../scope/scope.hpp"
#include "../utils/utils.hpp"
#include "../program/program.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <memory>


class Node {
public:
    std::string classname;
    Scope* scope;
    std::vector<Node*> branches;

    virtual void compileRec(reg_t destReg, int pointerArithmeticSize=0)=0;
    virtual void scanRec(Scope* parent)=0;

    std::vector<Node*> getBranches();
    void PrettyPrint(std::ostream &dst, std::string indent="");
    void ScopePrettyPrint(std::ostream &dst, std::string indent="");

    void listAppend(Node* nodeptr); // Refactor maybe?
    virtual void listAppend(Node* nodeptr, std::string type);
    virtual void listAppend(std::string* ident, std::string type);

    int getStackOffSet();
    std::string getIdent();
};

