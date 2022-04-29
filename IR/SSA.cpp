#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>

#include "../frontend/scanner.h"
#include "IR.h"
#include "SSA.h"

using namespace std;

extern Global glob;

SSABuilder::SSABuilder(Module mod) {
    for(auto func: mod.funcs) { 
        vector<BasicBlock *>* tmp = new vector<BasicBlock *>;
        for(auto blkPtr: func.blocks) {
            tmp->push_back(blkPtr);
        }
        blocks[func.name] = tmp;
        dfs[func.name] = NULL;
        funcNames.push_back(func.name);
        // printf("SSA func: %s\n", func.name.c_str());
        // DomTrees[func.name] = NULL;
        // DFTrees[func.name] = NULL;
    }
    // printf("var length: %d\n", mod.varNames.size());
    for(auto name: mod.varNames) {
        // printf("new name: %s\n", name.c_str());
        varNames.push_back(name);
        stack[name] = new vector<int>;
        stack[name]->push_back(0);
        counter[name] = 0;
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
        // printf("domdfs: visited %d\n", block->index);
        // for(auto id: block->successors) {
        //     printf("suc of %d: %d\n", block->index, id);
        // }
        for(auto suc: block->successors) {
            auto next = (*blocks)[suc];
            // printf("suc of %d: %d\n", block->index, next->index);
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
        // printf("name: %s\n", funcName.c_str());
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        dfs[funcName] = new vector<BasicBlock*>; int mask = 0;
        // printf("blocks len: %d\n", iter->second->size());
        recurDFS(iter->second, iter->second->front(), dfs[funcName], mask);

        map<BasicBlock*, bool>* visitMap = new map<BasicBlock*, bool>;
        for(auto b: *dfs[funcName]) {
            // printf("dfs: %d\n", b->index);
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

void SSABuilder::computeDFTree() {
    map<string, vector<BasicBlock*>*>::iterator iter = blocks.begin();
    
    while(iter != blocks.end()) {
        auto funcName = iter->first;
        // printf("name: %s\n", funcName.c_str());
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        for(auto blk: *blocks[funcName]) {
            printf("blk: %d\n", blk->index);
            auto doms = DomTrees[funcName][blk];
            // printf("doms: %d\n", doms);
            if(DFTrees[funcName][blk] == NULL)
                DFTrees[funcName][blk] = new vector<BasicBlock*>;
            if(doms) {
                for(auto d: *doms) {
                    if(DFTrees[funcName][d] == NULL)
                        DFTrees[funcName][d] = new vector<BasicBlock*>;
                    printf("%d domed by %d\n", blk->index, d->index);
                    for(auto suc: blk->successors) {
                        auto suc_block = (*blocks[funcName])[suc];
                        auto suc_dom = DomTrees[funcName][suc_block];
                        printf("%d pred %d domed by %d\n", suc, blk->index, d->index);
                        if(suc != d->index && find((*suc_dom).begin(), (*suc_dom).end(), d) == (*suc_dom).end()) {
                            printf("%d df %d\n", suc, d->index);
                            (DFTrees[funcName][d])->push_back(suc_block);
                        }
                        if(find((*suc_dom).begin(), (*suc_dom).end(), blk) == (*suc_dom).end()) {
                            if(find(((DFTrees[funcName][blk]))->begin(), ((DFTrees[funcName][blk]))->end(), suc_block) == ((DFTrees[funcName][blk]))->end()) {
                                printf("%d df %d\n", suc, blk->index);
                                (DFTrees[funcName][blk])->push_back(suc_block);
                            }
                        }
                    }

                }
            }
        }
        map<BasicBlock*, vector<BasicBlock*>*>::iterator df_iter = DFTrees[funcName].begin();
        while(df_iter != DFTrees[funcName].end()) {
            auto dor = df_iter->first->index;
            // printf("dor: %d\n", (*(df_iter->second)).size());
            for(auto b: *(df_iter->second)) {
                printf("df: %d -> %d\n", dor, b->index);
            }
            df_iter++;
        }
        iter++;
    }    
}

void SSABuilder::getBlocksofVar() {
    map<string, vector<BasicBlock*>*>::iterator iter = blocks.begin();
    
    while(iter != blocks.end()) {
        auto funcName = iter->first;
        printf("func name: %s\n", funcName.c_str());
        // printf("blocks size:%d\n", iter->second->size());
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        for(auto v: varNames)
            blocksOf[funcName][v] = new vector<BasicBlock*>;
        for(auto blk: *blocks[funcName]) {
            for(auto ins : blk->instructions) {
                if(ins->opcode != OpCode::MOVE)
                    continue;
                // if(ins->op1->name != "" && find(varNames.begin(), varNames.end(), ins->op1->name) != varNames.end() && find(blocksOf[funcName][ins->op1->name]->begin(), blocksOf[funcName][ins->op1->name]->end(), blk) == blocksOf[funcName][ins->op1->name]->end())
                //     blocksOf[funcName][ins->op1->name]->push_back(blk);
                if(ins->op2->name != "" && find(varNames.begin(), varNames.end(), ins->op2->name) != varNames.end() && find(blocksOf[funcName][ins->op2->name]->begin(), blocksOf[funcName][ins->op2->name]->end(), blk) == blocksOf[funcName][ins->op2->name]->end())
                    blocksOf[funcName][ins->op2->name]->push_back(blk);
                // if(ins->dest->name != "" && find(varNames.begin(), varNames.end(), ins->dest->name) != varNames.end() && find(blocksOf[funcName][ins->dest->name]->begin(), blocksOf[funcName][ins->dest->name]->end(), blk) == blocksOf[funcName][ins->dest->name]->end())
                //     blocksOf[funcName][ins->dest->name]->push_back(blk);
            }
        }
        // for(auto v: varNames) {
        //     printf("blocksof %s: ", v.c_str());
        //     for(auto b: *blocksOf[funcName][v]) {
        //         printf("%d ", b->index);
        //     }
        //     printf("\n");
        // }
        iter++;
    }
}

void SSABuilder::insertPhiNode() {
    map<string, vector<BasicBlock*>*>::iterator iter = blocks.begin();
    
    // while(iter != blocks.end()) {
    //     auto funcName = iter->first;
    //     // printf("name: %s\n", funcName.c_str());
    //     // printf("blocks size:%d\n", iter->second->size());
    //     if(iter->second->size() == 0) {
    //         iter++;
    //         continue;
    //     }
    //     for(auto blk: *blocks[funcName]) {
    //         for(auto ins: blk->instructions) {
    //             if(ins->opcode == OpCode::MOVE) {
    //                 // printf("find assign to %s in blk %d\n", ins->op2->name.c_str(), blk->index);
    //                 if(ins->op2->name != "") {
    //                     auto dfts = DFTrees[funcName][blk];
    //                     for(auto df: *dfts) {
    //                         if(!phiNodes[funcName][df])
    //                             phiNodes[funcName][df] = new vector<string>;
    //                         if(find(blocksOf[funcName][ins->op2->name]->begin(), blocksOf[funcName][ins->op2->name]->end(), df) != blocksOf[funcName][ins->op2->name]->end()) {
    //                             if(find(phiNodes[funcName][df]->begin(), phiNodes[funcName][df]->end(), ins->op2->name) == phiNodes[funcName][df]->end()) {
    //                                 printf("phi: %s in block %d\n", ins->op2->name.c_str(), df->index);
    //                                 phiNodes[funcName][df]->push_back(ins->op2->name);
    //                                 Value* v1 = new Value(); Value* v2 = new Value();
    //                                 Instruction *place = new Instruction();
    //                                 v1->name = ins->op2->name; v2->name = ins->op2->name;
    //                                 place->op1 = v1; place->op2 = v2;
    //                                 place->opcode = OpCode::PHI;
    //                                 place->dest = new Value();
    //                                 glob.addValue(place->dest);
    //                                 place->dest->name = ins->op2->name;
    //                                 df->instructions.insert(df->instructions.begin(), place);
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    //     iter++;
    // }

    while(iter != blocks.end()) {
        auto funcName = iter->first;
        // printf("name: %s\n", funcName.c_str());
        // printf("blocks size:%d\n", iter->second->size());
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        for(auto v: varNames) {
            vector<BasicBlock*> hasAlready, everOnWorkList, workList;
            hasAlready.clear(); everOnWorkList.clear(); workList.clear();
            for(auto blk: *(blocksOf[funcName][v])) {
                everOnWorkList.push_back(blk);
                workList.push_back(blk);
            }
            while(workList.size() > 0) {
                // printf("worklist size: %d\n", workList.size());
                auto cur = workList.back();
                workList.pop_back();
                for(auto df: *(DFTrees[funcName][cur])) {
                    if(find(hasAlready.begin(), hasAlready.end(), df) == hasAlready.end()) {
                        Instruction* place = new Instruction();
                        Value* v1 = new Value(); Value* v2 = new Value(); Value* dest = new Value();
                        dest->name = v1->name = v2->name = v; place->opcode = OpCode::PHI;
                        place->op1 = v1; place->op2 = v2; place->dest = dest;
                        df->instructions.insert(df->instructions.begin(), place);
                        hasAlready.push_back(df);
                        if(find(everOnWorkList.begin(), everOnWorkList.end(), df) == everOnWorkList.end()) {
                            everOnWorkList.push_back(df);
                            workList.push_back(df);
                        }
                    }
                }
            }
        }
        iter++;
    }    
}

void SSABuilder::renameVarinBlk(string funcName, string name, BasicBlock* blk) {
    for(auto ins: blk->instructions) {
        if(ins->opcode == OpCode::PHI) {
            if(ins->dest->name == name) {
                printf("phi of var %s\n", name.c_str());
                counter[name]++;
                ins->dest->name.append("^");
                ins->dest->name.append(to_string(counter[name]));
                stack[name]->push_back(counter[name]);
            }
        }
        else if(ins->opcode != OpCode::MOVE){
            if(ins->op1->name == name) {
                printf("use of var %s\n", name.c_str());
                ins->op1->name = name;
                ins->op1->name.append("^");
                ins->op1->name.append(to_string(stack[name]->back()));
            }
            if(ins->op2->name == name) {
                printf("use of var %s\n", name.c_str());
                ins->op2->name = name;
                ins->op2->name.append("^");
                ins->op2->name.append(to_string(stack[name]->back()));
                printf("%s %d\n", ins->op2->name.c_str(), ins->op2);
            }
        }
        else if(ins->opcode == OpCode::MOVE) {
            printf("found assign to: %s %d \n", ins->op2->name.c_str(), ins->op2);
            if(ins->op2->name == name) {
                // printf("def of var %s at val %d\n", name.c_str(), ins->op2);
                counter[name]++;
                ins->op2->name.append("^");
                ins->op2->name.append(to_string(counter[name]));
                stack[name]->push_back(counter[name]);
            }
        }
    }

    for(auto suc_idx: blk->successors) {
        auto suc = (*blocks[funcName])[suc_idx];
        for(auto suc_ins: suc->instructions) {
            if(suc_ins->opcode == OpCode::PHI) {
                if(suc_ins->op1->name == name) {
                    suc_ins->op1->name.append("^");
                    suc_ins->op1->name.append(to_string((*(stack[name])).back()));
                }
                else if(suc_ins->op2->name == name) {
                    suc_ins->op2->name.append("^");
                    suc_ins->op2->name.append(to_string((*(stack[name])).back()));
                }
            }
        }
    }

    for(auto block: *blocks[funcName]) {
        if(DomTrees[funcName][block] == NULL)
            continue;
        if(find(DomTrees[funcName][block]->begin(), DomTrees[funcName][block]->end(), blk) != DomTrees[funcName][block]->end()) {
            printf("find %d's domee %d\n", blk->index, block->index);
            renameVarinBlk(funcName, name, block);
        }
    }

    // for(auto ins: blk->instructions) {
    //     if(ins->opcode == OpCode::PHI || ins->opcode == OpCode::MOVE) {
    //         if(ins->dest->name == name) {
    //             stack[name]->pop_back();
    //         }
    //     }
    // }
}

void SSABuilder::renameVar(string name) {
    // map<string, vector<BasicBlock*>*>::iterator iter = dfs.begin(); int insCnt = 0;
    // while(iter != dfs.end()) {
    //     auto funcName = iter->first;
    //     if(!(iter->second) || iter->second->size() == 0) {
    //         iter++;
    //         continue;
    //     }
    //     idStack.clear(); int counter = 0; idStack.push_back(counter);
    //     for(auto blk: *(iter->second)){
    //         for(auto ins: blk->instructions) {
    //             if(find(blocksOf[funcName][name]->begin(), blocksOf[funcName][name]->end(), blk) != blocksOf[funcName][name]->end()) {
    //                 if(ins->opcode != OpCode::PHI && ins->opcode != OpCode::MOVE) {
    //                     if(ins->op1->name == name) {
    //                         ins->op1->name.append("_");
    //                         ins->op1->name.append(to_string(idStack.back()));
    //                     }
    //                     if(ins->op2->name == name) {
    //                         ins->op2->name.append("_");
    //                         ins->op2->name.append(to_string(idStack.back()));
    //                     }
    //                     // ins->dest->name = "%";
    //                     // ins->dest->name.append(to_string(insCnt));
    //                 }
    //                 if(ins->opcode == OpCode::MOVE) {
    //                     if(ins->op2->name == name) {
    //                         counter++; idStack.push_back(counter);
    //                         ins->op2->name.append("_");
    //                         ins->op2->name.append(to_string(idStack.back()));
    //                     }
    //                     // ins->dest->name = "%";
    //                     // ins->dest->name.append(to_string(insCnt));
    //                 }
    //                 if(ins->opcode == OpCode::PHI) {
    //                     if(ins->dest->name == name) {
    //                         counter++; idStack.push_back(counter);
    //                         ins->dest->name.append("_");
    //                         ins->dest->name.append(to_string(idStack.back()));
    //                     }
    //                 }
    //                 for(auto suc: blk->successors) {
    //                     auto suc_block = (*blocks[funcName])[suc];
    //                     for(auto suc_ins: suc_block->instructions) {
    //                         if(suc_ins->opcode == OpCode::PHI) {
    //                             if(suc_ins->op1->name == name) {
    //                                 suc_ins->op1->name.append("_");
    //                                 suc_ins->op1->name.append(to_string(idStack.back()));
    //                             }
    //                             else if(suc_ins->op2->name == name){
    //                                 suc_ins->op2->name.append("_");
    //                                 suc_ins->op2->name.append(to_string(idStack.back()));
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //             insCnt++;
    //         }
    //     }
    //     iter++;
    // }
    map<string, vector<BasicBlock*>*>::iterator iter = blocks.begin();
    while(iter != blocks.end()) {
        auto funcName = iter->first;
        // printf("name: %s\n", funcName.c_str());
        // printf("blocks size:%d\n", iter->second->size());
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        counter[name] = -1;
        stack[name]->clear();
        stack[name]->push_back(counter[name]);
        renameVarinBlk(funcName, name, (*(iter->second))[0]);
        iter++;
    }
}

void SSABuilder::transform() {
    computeDomTree();
    computeDFTree();
    getBlocksofVar();
    insertPhiNode();
    for(auto var: varNames)
        renameVar(var);
}