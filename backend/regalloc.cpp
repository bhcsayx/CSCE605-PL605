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

liveVarAnalysis::liveVarAnalysis(Module mod) {
    for(auto func: mod.funcs) {
        auto funcName = func.name;
        for(auto blk: func.blocks) {
            entry[funcName][blk] = new set<string>;
            exit[funcName][blk] = new set<string>;
            for(auto ins: blk->instructions) {
                pre[ins] = new set<string>;
                post[ins] = new set<string>;
            }
        }
    }
    workList.clear();
    for(auto v: mod.globalNames) {
        // printf("varname: %s\n", v.c_str());
        init_state.push_back(v);
    }
}

liveVarAnalysis::~liveVarAnalysis() {

}

void liveVarAnalysis::computeInstruction(Instruction* ins) {
    for(auto var: *(pre[ins]))
        post[ins]->insert(var);
    if(ins->opcode == OpCode::WRITE)
        post[ins]->insert(ins->op1->name);
    else {
        post[ins]->insert(ins->op1->name);
        post[ins]->insert(ins->op2->name);
        post[ins]->erase(ins->dest->name);
    }
}

void liveVarAnalysis::computeBlock(BasicBlock* block) {
    auto last_ins = block->instructions[block->instructions.size()-1];
}

void liveVarAnalysis::propagate() {

}