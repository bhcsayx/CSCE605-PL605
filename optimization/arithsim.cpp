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

#include "arithsim.h"

using namespace std;

arithSim::arithSim() {}
arithSim::~arithSim() {}

void arithSim::run(SSABuilder builder) {
    auto map_iter = builder.blocks.begin();
    while(map_iter != builder.blocks.end()) {
        auto funcName = map_iter->first;
        for(auto blk: *(builder.blocks[funcName])) {
            for(auto ins: blk->instructions) {
                if(ins->opcode == OpCode::ADD | ins->opcode == OpCode::SUB) {
                    if(ins->op2->type == Type::constVal && ins->op2->value == 0) {
                        Instruction* _new = new Instruction();
                        _new->op1 = ins->op1;
                        _new->op2 = ins->dest;
                        _new->opcode = OpCode::MOVE;
                        simplified[ins] = _new;
                    }
                }
                if(ins->opcode == OpCode::MUL | ins->opcode == OpCode::DIV) {
                    if(ins->op2->type == Type::constVal && ins->op2->value == 1) {
                        Instruction* _new = new Instruction();
                        _new->op1 = ins->op1;
                        _new->op2 = ins->dest;
                        _new->opcode = OpCode::MOVE;
                        simplified[ins] = _new;
                    }
                    else if(ins->opcode == OpCode::MUL && ins->op2->type == Type::constVal && ins->op2->value == 0) {
                        Instruction* _new = new Instruction();
                        _new->op1 = new Value();
                        _new->op1->value = 0;
                        _new->op1->type = Type::constVal;
                        _new->op2 = ins->dest;
                        _new->opcode = OpCode::MOVE;
                        simplified[ins] = _new;
                    }

                }
            }
        }
        map_iter++;
    }
}

void arithSim::addInstructionwithRes(Dot* dot, string name, string funcName, Instruction* ins) {
    string res;

    if(ins->opcode != OpCode::PHI) {
        if(simplified[ins] != NULL)
            res.append("eliminated-");
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
    
    res.append("|");

    dot->nodes[name].label.append(res.c_str());

    if(simplified[ins] != NULL)
        addInstructionwithRes(dot, name, funcName, simplified[ins]);
}

void arithSim::dump(SSABuilder builder, string name) {
    Dot dot;
    for(auto funcName: builder.funcNames) {
        // printf("funcname: %s\n", funcName.c_str());
        // for(auto blk: *(blks_iter->second)) {
        for(auto blk: *(builder.blocks[funcName])) {
            string name = funcName;
            name.append("_BB"); name.append(to_string(blk->index));
            dot.addNode(name);
            for(auto ins: blk->instructions)
                addInstructionwithRes(&dot, name, funcName, ins);
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