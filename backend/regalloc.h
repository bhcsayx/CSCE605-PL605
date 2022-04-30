#ifndef REGALLOC_H
#define REGALLOC_H

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

class liveVarAnalysis {
public:
    vector<BasicBlock*> workList;

    vector<string> init_state;
    map<string, map<BasicBlock*, set<string>*>> entry;
    map<string, map<BasicBlock*, set<string>*>> exit;

    map<Instruction*, set<string>*> pre;
    map<Instruction*, set<string>*> post;

    liveVarAnalysis(Module mod);
    ~liveVarAnalysis();

    void computeBlock(BasicBlock* block);
    void computeInstruction(Instruction* ins);

    void propagate();
    // void dump();
};

#endif