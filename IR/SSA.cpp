#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../frontend/scanner.h"
#include "IR.h"
#include "SSA.h"

using namespace std;

SSABuilder::SSABuilder(Module mod) {
    for(auto func: mod.funcs) { 
        vector<BasicBlock *>* tmp = new vector<BasicBlock *>;
        for(auto blkPtr: func.blocks) {
            tmp->push_back(blkPtr);
        }
        blocks[func.name] = tmp;
        dfs[func.name] = NULL;
        DomTrees[func.name] = NULL;
        DFTrees[func.name] = NULL;
    }
    // printf("var length: %d\n", mod.varNames.size());
    for(auto name: mod.varNames) {
        // printf("new name: %s\n", name.c_str());
        varNames.push_back(name);
    }
    // for(auto n: varNames) {
    //     printf("new name: %s\n", n.c_str());
    // }
}

void SSABuilder::recurDFS(vector<BasicBlock*>* blocks, BasicBlock* cur, vector<BasicBlock*>* dest, int& mask) {
    if((mask & (1 << (cur->index))))
        return;
    dest->push_back(cur);
    mask |= (1 << (cur->index));
    for(auto suc: cur->successors) {
        // printf("going suc: %d\n", suc);
        auto next = (*blocks)[suc];
        recurDFS(blocks, next, dest, mask);
    }
}

void SSABuilder::computeDomTree() {
    map<string, vector<BasicBlock*>*>::iterator iter = blocks.begin();
    while(iter != blocks.end()) {
        auto funcName = iter->first;
        printf("name: %s\n", funcName.c_str());
        dfs[funcName] = new vector<BasicBlock*>; int mask = 0;
        // printf("blocks len: %d\n", iter->second->size());
        recurDFS(iter->second, iter->second->front(), dfs[funcName], mask);
        for(auto b: *dfs[funcName]) {
            printf("dfs: %d\n", b->index);
        }
        iter++;
    }
}