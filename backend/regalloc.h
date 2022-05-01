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
    vector<BasicBlock*> everOnWorkList;

    vector<string> init_state;
    map<string, map<BasicBlock*, set<string>*>> entry;
    map<string, map<BasicBlock*, set<string>*>> exit;

    map<Instruction*, set<string>*> pre;
    map<Instruction*, set<string>*> post;

    liveVarAnalysis(SSABuilder builder);
    ~liveVarAnalysis();

    void computeBlock(string funcName, SSABuilder builder, BasicBlock* block);
    void computeInstruction(Instruction* ins);

    void init(string funcName, SSABuilder builder);
    void propagate(string funcName, SSABuilder builder);

    void addInstructionwithRes(Dot* dot, string name, Instruction* ins);
    void dump2dot(SSABuilder builder, string name);
};

class regAlloc {
public:
    map<string, map<string, set<string>*>>rig;
    map<string, int> res;

    regAlloc(SSABuilder builder, liveVarAnalysis LVA);
};

#endif