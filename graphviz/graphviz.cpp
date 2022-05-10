#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../IR/IR.h"
#include "../IR/SSA.h"

#include "graphviz.h"
#include "Dot.h"

using namespace std;

string val2txt(Value* val) {
    switch(val->type) {
        case Type::def: {
            return val->name;
            break;
        }
        case Type::empty: {
            string res("");
            return res;
        }
        case Type::insRes: {
            // string res("%");
            // auto cstr = std::to_string(val->index);
            // res.append(cstr.c_str());
            // printf("val name: %s\n", val->name.c_str());
            string res = val->name;
            return res;
        }
        case Type::constVal: {
            auto cstr = std::to_string(val->value);
            string res(cstr.c_str());
            return res;
        }
        default: {
            string res("");
            return res;
        }
    }
}

string op2txt(enum OpCode op) {
    switch(op) {
        case OpCode::ADD: {
            string res("add");
            return res;
        }
        case OpCode::SUB: {
            string res("sub");
            return res;
        }
        case OpCode::NEG: {
            string res("neg");
            return res;
        }
        case OpCode::MUL: {
            string res("mul");
            return res;
        }
        case OpCode::DIV: {
            string res("div");
            return res;
        }
        case OpCode::CMP: {
            string res("cmp");
            return res;
        }
        case OpCode::MOVE: {
            string res("move");
            return res;
        }
        case OpCode::PHI: {
            string res("phi");
            return res;
        }
        case OpCode::END: {
            string res("end");
            return res;
        }
        case OpCode::BRA: {
            string res("bra");
            return res;
        }
        case OpCode::BNE: {
            string res("bne");
            return res;
        }
        case OpCode::BEQ: {
            string res("beq");
            return res;
        }
        case OpCode::BLE: {
            string res("ble");
            return res;
        }
        case OpCode::BLT: {
            string res("blt");
            return res;
        }
        case OpCode::BGE: {
            string res("bge");
            return res;
        }
        case OpCode::BGT: {
            string res("bgt");
            return res;
        }
        case OpCode::READ: {
            string res("read");
            return res;
        }
        case OpCode::WRITE: {
            string res("write");
            return res;
        }
        case OpCode::WRITENL: {
            string res("writenl");
            return res;
        }
        case OpCode::CALL: {
            string res("call");
            return res;
        }
        case OpCode::RET: {
            string res("ret");
            return res;
        }
        default: {
            string res("default");
            return res;
        }
    }
}

vector<string> dump2txt(Module mod) {
    vector<string> res;
    string tabs("\n\t");
    for(auto func: mod.funcs) {
        res.push_back(func.name);
        printf("func: %s\n", func.name.c_str());
        int blkIdx = 0;
        for(auto blk: func.blocks) {
            res.push_back(tabs);
            string name("bb");
            name.append(std::to_string(blkIdx).c_str());
            blkIdx++;
            // printf("blk length: %d\n", blk.instructions.size());
            res.push_back(name);
            for (auto ins: blk->instructions) {
                // printf("instruction: %d\n", ins->opcode);
                string ins_str("");
                // printf("dest name: %s\n", ins->dest->name.c_str());
                if(ins->opcode != OpCode::MOVE) {
                    ins_str.append(ins->dest->name); 
                    ins_str.append(" = ");
                }
                ins_str.append(op2txt(ins->opcode).c_str());
                ins_str.append(" ");
                if(ins->opcode == OpCode::CALL) {
                    ins_str.append(val2txt(ins->op1).c_str());
                    ins_str.append(" ");
                    for(auto arg: ins->callArgs) {
                        ins_str.append(val2txt(arg).c_str());
                        ins_str.append(" ");
                    }
                }
                else {
                    ins_str.append(val2txt(ins->op1).c_str());
                    ins_str.append(" ");
                    ins_str.append(val2txt(ins->op2).c_str());
                    ins_str.append(" ");
                }
                res.push_back(ins_str);
            }
            // for (auto pre: blk->predecessors) {
            //     printf("pre of %d: -> %d\n", blk->index, pre);
            // }
            for (auto suc: blk->successors) {
                printf("suc of %d: -> %d\n", blk->index, suc);
            }
        }
    }
    for(auto s:res) {
        std::cout << s << endl;
    }
    return res;
}

void dump2dot(SSABuilder builder, string name) {
    Dot dot;
    // auto blks_iter = builder.blocks.begin();
    // while(blks_iter != builder.blocks.end()) {
    //     auto funcName = blks_iter->first;
        // if(blks_iter->second->size() == 0) {
        //     blks_iter++; continue;
        // }
    for(auto funcName: builder.funcNames) {
        // printf("funcname: %s\n", funcName.c_str());
        // for(auto blk: *(blks_iter->second)) {
        for(auto blk: *(builder.blocks[funcName])) {
            string name = funcName;
            name.append("_BB"); name.append(to_string(blk->index));
            dot.addNode(name);
            for(auto ins: blk->instructions)
                dot.addInstruction(name, ins);
            if(blk->successors.size() > 0) {
                if(blk->successors.size() == 2) {
                    string br1 = funcName;
                    br1.append("_BB"); br1.append(to_string(blk->successors[0]));
                    dot.addCFGEdge(name, br1, "fall-through");
                    string br2 = funcName;
                    br2.append("_BB"); br2.append(to_string(blk->successors[1]));
                    dot.addCFGEdge(name, br2, "branch");
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