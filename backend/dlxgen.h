#ifndef DLXGEN_H
#define DLXGEN_H

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

#include "regalloc.h"

using namespace std;

class DLXGenerator {
public:
    vector<int> code;
    map<string, map<string, int>> regallocs;
    string curFunc;
    int regavail;

    DLXGenerator(regAlloc alloc);
    ~DLXGenerator();
    void dlxgen(SSABuilder builder);

    // void genAdd(Instruction* ins);
    // void genSub(Instruction* ins);
    // void genMul(Instruction* ins);
    // void genDiv(Instruction* ins);

    void genWrite(Instruction* ins);

    void dump(string name);
};

#endif