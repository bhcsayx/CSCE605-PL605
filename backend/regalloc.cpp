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

liveVarAnalysis::liveVarAnalysis(SSABuilder builder) {
    auto iter = builder.blocks.begin();
    while(iter != builder.blocks.end()) {
        auto funcName = iter->first;
        for(auto blk: *(iter->second)) {
            entry[funcName][blk] = new set<string>;
            exit[funcName][blk] = new set<string>;
            for(auto ins: blk->instructions) {
                pre[ins] = new set<string>;
                post[ins] = new set<string>;
            }
        }
        iter++;
    }
    workList.clear();
    
    for(auto v: builder.globalNames) {
        printf("varname: %s\n", v.c_str());
        init_state.push_back(v);
    }

    iter = builder.blocks.begin();
    while(iter != builder.blocks.end()) {
        auto funcName = iter->first;
        init(funcName, builder);
        propagate();
        iter++;
    }
}

liveVarAnalysis::~liveVarAnalysis() {

}

void liveVarAnalysis::computeInstruction(Instruction* ins) {
    for(auto var: *(pre[ins]))
        post[ins]->insert(var);
    if(ins->opcode == OpCode::WRITE)
        post[ins]->insert(ins->op1->name);
    else if(ins->opcode == OpCode::MOVE) {
        post[ins]->insert(ins->op1->name);
        post[ins]->erase(ins->op2->name);
    }
    else {
        post[ins]->insert(ins->op1->name);
        post[ins]->insert(ins->op2->name);
        post[ins]->erase(ins->dest->name);
    }
}

void liveVarAnalysis::computeBlock(BasicBlock* block) {
    // for(auto suc_idx: block->successors) {
    //     auto suc = 
    // }
    auto last_ins = block->instructions[block->instructions.size()-1];
    // for(auto i: *(entry[funcName][block]))
    //     pre[last_ins]->insert(i);
}

void liveVarAnalysis::init(string funcName, SSABuilder builder) {
    printf("init func:%s\n", funcName.c_str());
    workList.clear();
    for(auto block: *(builder.blocks[funcName])) {
        if(block->successors.size() == 0)
            workList.push_back(block);
    }
    for(auto b: workList)
        printf("worklist: %d\n", b->index);    
}

void liveVarAnalysis::propagate() {
    
}

// void liveVarAnalysis::LVAEntry() {
//     // for()
// }