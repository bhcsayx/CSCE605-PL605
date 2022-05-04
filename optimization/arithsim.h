#ifndef ARITHSIM_H
#define ARITHSIM_H

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "../IR/IR.h"
#include "../IR/SSA.h"

#include "../graphviz/Dot.h"
#include "../graphviz/graphviz.h"

using namespace std;

class arithSim {
public:
    map<Instruction*, Instruction*> simplified;
    arithSim();
    ~arithSim();
    void run(SSABuilder builder);
    void addInstructionwithRes(Dot* dot, string name, string funcName, Instruction* ins);
    void dump(SSABuilder builder, string name);
};

#endif