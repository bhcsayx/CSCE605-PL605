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
    
    for(auto funcName: builder.funcNames) {
        printf("global size: %d\n", builder.globalNames[funcName]->size());
        for(auto v: *(builder.globalNames[funcName])) {
            printf("varname: %s\n", v.c_str());
        }
    }

    iter = builder.blocks.begin();
    while(iter != builder.blocks.end()) {
        auto funcName = iter->first;
        init(funcName, builder);
        propagate(funcName, builder);
        iter++;
    }
}

liveVarAnalysis::~liveVarAnalysis() {

}

void liveVarAnalysis::computeInstruction(Instruction* ins) {
    for(auto var: *(pre[ins]))
        post[ins]->insert(var);
    if(ins->opcode == OpCode::WRITE && ins->op1->type != Type::constVal)
        post[ins]->insert(ins->op1->name);
    else if(ins->opcode == OpCode::MOVE) {
        if(ins->op1->type != Type::constVal)
            post[ins]->insert(ins->op1->name);
        post[ins]->erase(ins->op2->name);
    }
    else {
        if(ins->opcode != OpCode::CALL) {
            if(ins->op1->name != "")
                post[ins]->insert(ins->op1->name);
            if(ins->op2->name != "")
                post[ins]->insert(ins->op2->name);
            if(ins->dest->name != "")
                post[ins]->erase(ins->dest->name);
        }
    }
}

void liveVarAnalysis::computeBlock(string funcName, SSABuilder builder, BasicBlock* block) {
    if(block->instructions.size() == 0) {
        for(auto i: *(entry[funcName][block]))
            exit[funcName][block]->insert(i);
        return;
    }
    // set entry states of block
    auto cur = entry[funcName][block];
    for(auto suc_idx: block->successors) {
        auto suc = (*(builder.blocks[funcName]))[suc_idx];
        for(auto e: *(exit[funcName][suc]))
            cur->insert(e);
    }
    // set last instruction of block states
    auto last_ins = block->instructions.back();
    for(auto i: *(entry[funcName][block]))
        pre[last_ins]->insert(i);
    // iterate instructions
    for(auto rev_iter=block->instructions.rbegin(); rev_iter!=block->instructions.rend(); rev_iter++) {
        computeInstruction(*(rev_iter));
        if(rev_iter!=block->instructions.rend()-1) {
            for(auto s: *(post[*rev_iter])) {
                pre[*(rev_iter+1)]->insert(s);
            }
        }
    }
        
    // set exit states of block
    auto first_ins = block->instructions[0];
    for(auto i: *(post[first_ins]))
        exit[funcName][block]->insert(i);
}

void liveVarAnalysis::init(string funcName, SSABuilder builder) {
    printf("init func:%s\n", funcName.c_str());
    workList.clear();
    for(auto block: *(builder.blocks[funcName])) {
        if(block->successors.size() == 0) {
            workList.push_back(block);
            for(auto s: *(builder.globalNames[funcName])) {
                printf("init var: %s\n", s.c_str());
                entry[funcName][block]->insert(s);
            }
        }
    }
    // for(auto b: workList)
    //     printf("worklist: %d\n", b->index);    
}

void liveVarAnalysis::propagate(string funcName, SSABuilder builder) {
    while(workList.size() != 0) {
        // for(auto b: workList)
        //     printf("worklist: %d\n", b->index);    
        auto blk = workList.back();
        workList.pop_back();
        everOnWorkList.push_back(blk);
        computeBlock(funcName, builder, blk);
        for(auto pred_idx: blk->predecessors) {
            auto blockV = *(builder.blocks[funcName]);
            BasicBlock* pred = blockV[pred_idx];
            if(find(everOnWorkList.begin(), everOnWorkList.end(), pred) == everOnWorkList.end())
                workList.push_back(pred);
        }
    }
}

void liveVarAnalysis::addInstructionwithRes(Dot* dot, string name, Instruction* ins) {
    // Node node = dot.nodes[name];
    string res;

    if(ins->opcode != OpCode::PHI) {
        res.append(to_string((dot->counter)++));
        res.append(": ");
    }
    // res.append(ins->dest->name.substr(1, ins->dest->name.length()));
    // res.append(": ");
    
    switch (ins->opcode) {
        case OpCode::NEG: {
            res.append("NEG ");
            break;
        }
        case OpCode::ADD: {
            res.append("ADD ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::SUB: {
            res.append("SUB ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::MUL: {
            res.append("MUL ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::DIV: {
            res.append("DIV ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else
                res.append(ins->op1->name.c_str());
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::CMP: {
            res.append("CMP ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            else {
                res.append(ins->op1->name.c_str());
            }
                
            res.append(" ");
            if(ins->op2->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op2->value));
            }
            else
                res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::MOVE: {
            res.append("MOVE ");
            if(ins->op1->type == Type::constVal) {
                res.append("#");
                res.append(to_string(ins->op1->value));
            }
            // else if(ins->op1->type == Type::insRes) {
            //     res.append("%");
            //     res.append(to_string(ins->op1->index));
            // }
            else {
                res.append(ins->op1->name.c_str());
            }
            res.append(" ");
            res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::PHI: {
            res.append(ins->dest->name.c_str());
            res.append(" = PHI ");
            res.append(ins->op1->name.c_str());
            res.append(" ");
            res.append(ins->op2->name.c_str());
            break;
        }
        case OpCode::END: {
            res.append("END ");
            break;
        }
        case OpCode::BRA: {
            res.append("BRA [");
            res.append(to_string(ins->op1->value));
            res.append("]");
            break;
        }
        case OpCode::BEQ: {
            res.append("BEQ [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BNE: {
            res.append("BNE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BLT: {
            res.append("BLT [");
            // res.append(ins->op2->name);
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BGT: {
            res.append("BGT [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BLE: {
            res.append("BLE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::BGE: {
            res.append("BGE [");
            res.append(to_string(ins->op2->value));
            res.append("]");
            break;
        }
        case OpCode::READ: {
            res.append("READ ");
            break;
        }
        case OpCode::WRITE: {
            res.append("WRITE ");
            res.append(ins->op1->name);
            break;
        }
        case OpCode::WRITENL: {
            res.append("WRITENL ");
            break;
        }
        case OpCode::CALL: {
            res.append("CALL ");
            res.append(ins->op1->name);
            for(auto arg: ins->callArgs) {
                res.append(" ");
                if(arg->type == Type::constVal) {
                    res.append("#");
                    res.append(to_string(arg->value));
                }
                else
                    res.append(arg->name);
            }
            break;
        }
        case OpCode::RET: {
            res.append("RET ");
            if(ins->op1->type != Type::empty) {
                if(ins->op1->type == Type::constVal) {
                    res.append("#");
                    res.append(to_string(ins->op1->value));
                }
            else
                res.append(ins->op1->name.c_str());
            }
            break;
        }
        
    }

    res.append(" preAnalysis=\\{");
    for(auto s: *(pre[ins])) {
        res.append(s);
        res.append(" ");
    }
    res.append("\\} postAnalysis=\\{");
    for(auto s: *(post[ins])) {
        res.append(s);
        res.append(" ");
    }
    res.append("\\}");
        

    res.append("|");

    dot->nodes[name].label.append(res.c_str());
}

void liveVarAnalysis::dump2dot(SSABuilder builder, string name) {
    Dot dot;
    for(auto funcName: builder.funcNames) {
        // printf("funcname: %s\n", funcName.c_str());
        // for(auto blk: *(blks_iter->second)) {
        for(auto blk: *(builder.blocks[funcName])) {
            string name = funcName;
            name.append("_BB"); name.append(to_string(blk->index));
            dot.addNode(name);
            for(auto ins: blk->instructions)
                addInstructionwithRes(&dot, name, ins);
            if(blk->successors.size() > 0) {
                if(blk->successors.size() == 2) {
                    string br1 = funcName;
                    br1.append("_BB"); br1.append(to_string(blk->successors[0]));
                    dot.addCFGEdge(name, br1, "branch");
                    string br2 = funcName;
                    br2.append("_BB"); br2.append(to_string(blk->successors[1]));
                    dot.addCFGEdge(name, br2, "fall-through");
                }
                else {
                    string br1 = funcName;
                    br1.append("_BB"); br1.append(to_string(blk->successors[0]));
                    dot.addCFGEdge(name, br1, "fall-through");
                }
            }
        }

        for(auto dom: builder.DomTrees[funcName]) {
            string lname = funcName;
            lname.append("_BB"); lname.append(to_string(dom.first->index));
            if(dom.second == NULL)
                continue;
            for(auto r: *(dom.second)) {
                string rname = funcName;
                rname.append("_BB"); rname.append(to_string(r->index));
                dot.addDomEdge(lname, rname);
            }
        }

        for(auto df: builder.DFTrees[funcName]) {
            string lname = funcName;
            lname.append("_BB"); lname.append(to_string(df.first->index));
            if(df.second == NULL)
                continue;
            for(auto r: *(df.second)) {
                string rname = funcName;
                rname.append("_BB"); rname.append(to_string(r->index));
                dot.addDFEdge(lname, rname);
            }
        }
        // blks_iter++;
    }
    dot.dump(name);
}

regAlloc::regAlloc(SSABuilder builder, liveVarAnalysis LVA, int k) {
    rig.clear();

    map<string, vector<BasicBlock*>*>::iterator iter = builder.blocks.begin();
    while(iter != builder.blocks.end()) {
        auto funcName = iter->first;
        rig[funcName].clear();
        if(iter->second->size() == 0) {
            iter++;
            continue;
        }
        for(auto blk: *(iter->second)) {
            for(auto ins: blk->instructions) {
                for(auto key: *(LVA.post[ins])) {
                    // if(key == "")
                    //     continue;
                    if(!rig[funcName].count(key))
                        rig[funcName][key] = new set<string>;
                    for(auto other: *(LVA.post[ins])) {
                        if(!rig[funcName].count(other))
                            rig[funcName][other] = new set<string>;
                        if(key != other) {
                            rig[funcName][key]->insert(other);
                            rig[funcName][other]->insert(key);
                        }
                    }
                }
            }
        }
        for(auto rig_iter=rig[funcName].begin(); rig_iter!=rig[funcName].end(); rig_iter++) {
            auto key = rig_iter->first;
            printf("rig key: %s\n", key.c_str());
            for(auto value: *(rig_iter->second)) {
                printf("\trig value: %s\n", value.c_str());
            }
        }
        color(funcName, k);
        iter++;
    }

}

void regAlloc::color(string funcName, int k) {
    auto tmp_rig = rig[funcName];
    map<string, bool> deleted;

    int num_deleted = 0;
    for(auto key: tmp_rig)
        deleted[key.first] = false;
    
    while(num_deleted < tmp_rig.size()) {
        auto rig_iter = tmp_rig.begin();
        while(rig_iter != tmp_rig.end()) {
            if(rig_iter->second->size() < k && !deleted[rig_iter->first])
                break;
            rig_iter++;
        }
        if(rig_iter != tmp_rig.end()) {
            // printf("deleting node: %s\n", rig_iter->first.c_str());
            deleted[rig_iter->first] = true;
            order[funcName].push_back(rig_iter->first);
        }
        else {
            for(auto iter=tmp_rig.begin(); iter!=tmp_rig.end(); iter++) {
                if(!deleted[iter->first]) {
                    // printf("deleting node bigger than k: %s\n", iter->first.c_str());
                    deleted[iter->first] = true;
                    order[funcName].push_back(iter->first);
                    break;
                }
            }
        }
        num_deleted++;
    }
    // for(auto s: order[funcName]) {
    //     printf("node: %s\n", s.c_str());
    // }
    for(auto order_iter=order[funcName].rbegin(); order_iter!=order[funcName].rend(); order_iter++) {
        int i;
        // printf("coloring %s\n", (*(order_iter)).c_str());
        for(i=1;i<k+1;i++) {
            bool able = true;
            for(auto s: *(tmp_rig[*(order_iter)])) {
                if(res[funcName][s]==i && !deleted[s]) {
                    able = false; break;
                }
            }
            if(!able) continue;
            else break;
        }
        if(i != k+1) res[funcName][*(order_iter)] = i;
        else res[funcName][*(order_iter)] = -1;
        deleted[*(order_iter)] = false;
    }

    for(auto s: order[funcName]) {
        printf("reg alloc for %s: %d\n", s.c_str(), res[funcName][s]);
    }
}