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
        // DomTrees[func.name] = NULL;
        // DFTrees[func.name] = NULL;
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

void domDFS(vector<BasicBlock*>* blocks, BasicBlock* block, map<BasicBlock*, bool>* visitMap) {
    if(!(*(visitMap))[block]) {
        (*(visitMap))[block] = true;
        printf("domdfs: visited %d\n", block->index);
        // for(auto id: block->successors) {
        //     printf("suc of %d: %d\n", block->index, id);
        // }
        for(auto suc: block->successors) {
            auto next = (*blocks)[suc];
            printf("suc of %d: %d\n", block->index, next->index);
            if(!(*(visitMap))[next]) {
                // printf("domdfs: new suc %d\n", next->index);
                domDFS(blocks, next, visitMap);
            }
        }
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

        map<BasicBlock*, bool>* visitMap = new map<BasicBlock*, bool>;
        // map<BasicBlock*, BasicBlock*> dom;
        // for(auto b: *dfs[funcName]) {
        //     dom[b] = NULL;
        // }
        for(auto b: *dfs[funcName]) {
            printf("dfs: %d\n", b->index);
            for(auto n: *dfs[funcName])
                (*visitMap)[n] = false;
            (*visitMap)[b] = true;
            domDFS(iter->second, dfs[funcName]->front(), visitMap);

            for(auto bb: *dfs[funcName]) {
                if(!(*(visitMap))[bb]) {
                    if(!(DomTrees[funcName][bb]))
                        DomTrees[funcName][bb] = new vector<BasicBlock*>;
                    (DomTrees[funcName][bb])->push_back(b);
                    // dom[bb] = b;
                }
            }
        }
        // map<BasicBlock*, BasicBlock*>::iterator d_iter = dom.begin(); 
        // while(d_iter != dom.end()) {
        //     printf("key: %d, value: %d\n", d_iter->first->index, d_iter->second == NULL?0:d_iter->second->index);
        //     d_iter++;
        // }
        // DomTrees[funcName] = new map<BasicBlock*, vector<BasicBlock*>*>;
        // for(auto b: *(iter->second)) {
        //     // DomTrees[funcName][b] = new vector<BasicBlock*>;
        //     if(dom[b])
        //         (DomTrees[funcName][dom[b]])->push_back(b);
        // }
        map<BasicBlock*, vector<BasicBlock*>*>::iterator dom_iter = DomTrees[funcName].begin();
        while(dom_iter != DomTrees[funcName].end()) {
            auto dor = dom_iter->first->index;
            printf("dor: %d\n", (*(dom_iter->second)).size());
            for(auto b: *(dom_iter->second)) {
                printf("dom: %d -> %d\n", dor, b->index);
            }
            dom_iter++;
        }
        
        iter++;
    }
}