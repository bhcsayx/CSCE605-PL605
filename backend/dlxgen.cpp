#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include "../IR/IR.h"
#include "../IR/SSA.h"

#include "../graphviz/Dot.h"
#include "../graphviz/graphviz.h"

#include "dlxgen.h"

using namespace std;

DLXGenerator::DLXGenerator(SSABuilder builder, regAlloc alloc) {
    for(auto res_iter: alloc.res) {
        auto funcName = res_iter.first;
        for(auto set_iter: res_iter.second) {
            auto valName = set_iter.first;
            regallocs[funcName][valName] = set_iter.second;
        }
    }

    short int glob_offset = 0;
    for(auto iter: builder.globalNames) {
        for(auto s: *(iter.second)) {
            if(!globals.count(s)) {
                // printf("global: %s, %d\n", s.c_str(), glob_offset);
                globals[s] = glob_offset--;
            }
        }
    }
    for(auto iter: regallocs) {
        for(auto s: iter.second) {
            if(!globals.count(s.first)) {
                // printf("global: %s, %d\n", s.first.c_str(), glob_offset);
                globals[s.first] = glob_offset--;
            }
        }
    }
}

DLXGenerator::~DLXGenerator() {

}

void DLXGenerator::genAdd(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    int regdest = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : -1;
    // printf("reg for add: %d %d %d\n", reg1, reg2, regdest);

    if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
            // printf("add ld1: %d\n", add1);
            reg1 = 27;
        }
    }
    else if(reg1 == -1) {
        // printf("loading from memory...%d\n", globals[ins->op1->name]);
        reg1 = 27;
        int ld1 = (32 << 26);
        ld1 |= (27 << 21);
        ld1 |= (30 << 16);
        int offset = globals[ins->op1->name] & 0xffff;
        ld1 |= offset;
        code.push_back(ld1);
    }
    if(reg2 == 0) {
        if(ins->op2->type == Type::constVal) {
            int add2 = ins->op2->value>=0 ? (16 << 26) : (17 << 26);
            add2 |= (26 << 21);
            add2 |= ins->op2->value>=0 ? ins->op2->value : -(ins->op2->value);
            // printf("add ld2: %d\n", add2);
            code.push_back(add2);
            reg2 = 26;
        }
    }
    else if(reg2 == -1) {
        // printf("need to load from memory...\n", globals[ins->op2->name]);
        reg2 = 26;
        int ld2 = (32 << 26);
        ld2 |= (26 << 21);
        ld2 |= (30 << 16);
        int offset = globals[ins->op2->name] & 0xffff;
        ld2 |= offset;
        code.push_back(ld2);
    }
    int add = 0;
    if(regdest != -1) {
        add |= (regdest << 21);
    }
    else {
        regdest = 27;
        add |= (27 << 21);
    }
    add |= (reg1 << 16);
    add |= reg2;
    // printf("reg for add after: %d %d %d\n", reg1, reg2, regdest);
    code.push_back(add);

    if(regdest == 27) {
        // printf("adding store at add...\n");
        int store = (36 << 26);
        store |= (27 << 21);
        store |= (30 << 16);
        int offset = globals[ins->dest->name] & 0xffff;
        store |= offset;
        code.push_back(store);
        // printf("store at add: %d\n", store);
    }
}

void DLXGenerator::genSub(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    int regdest = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : -1;
    // printf("reg for sub: %d %d %d\n", reg1, reg2, regdest);
    if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
            // printf("add ld1: %d\n", add1);
            reg1 = 27;
        }
    }
    else if(reg1 == -1) {
        // printf("loading from memory...%d\n", globals[ins->op1->name]);
        reg1 = 27;
        int ld1 = (32 << 26);
        ld1 |= (27 << 21);
        ld1 |= (30 << 16);
        int offset = globals[ins->op1->name] & 0xffff;
        ld1 |= offset;
        code.push_back(ld1);
    }
    if(reg2 == 0) {
        if(ins->op2->type == Type::constVal) {
            int add2 = ins->op2->value>=0 ? (16 << 26) : (17 << 26);
            add2 |= (26 << 21);
            add2 |= ins->op2->value>=0 ? ins->op2->value : -(ins->op2->value);
            // printf("add ld2: %d\n", add2);
            code.push_back(add2);
            reg2 = 26;
        }
    }
    else if(reg2 == -1) {
        // printf("need to load from memory...\n", globals[ins->op2->name]);
        reg2 = 26;
        int ld2 = (32 << 26);
        ld2 |= (26 << 21);
        ld2 |= (30 << 16);
        int offset = globals[ins->op2->name] & 0xffff;
        ld2 |= offset;
        code.push_back(ld2);
    }
    int sub = (1 << 26);
    if(regdest != -1) {
        sub |= (regdest << 21);
    }
    else {
        regdest = 27;
        sub |= (27 << 21);
    }
    sub |= (reg1 << 16);
    sub |= reg2;
    // printf("reg for sub after: %d %d %d\n", reg1, reg2, regdest);
    code.push_back(sub);

    if(regdest == 27) {
        // printf("adding store at sub...\n");
        int store = (36 << 26);
        store |= (27 << 21);
        store |= (30 << 16);
        int offset = globals[ins->dest->name] & 0xffff;
        store |= offset;
        code.push_back(store);
        // printf("store at sub: %d\n", store);
    }
}

void DLXGenerator::genMul(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    int regdest = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : -1;
    // printf("reg for mul: %d %d %d\n", reg1, reg2, regdest);

    if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
            // printf("add ld1: %d\n", add1);
            reg1 = 27;
        }
    }
    else if(reg1 == -1) {
        // printf("loading from memory...%d\n", globals[ins->op1->name]);
        reg1 = 27;
        int ld1 = (32 << 26);
        ld1 |= (27 << 21);
        ld1 |= (30 << 16);
        int offset = globals[ins->op1->name] & 0xffff;
        ld1 |= offset;
        code.push_back(ld1);
    }
    if(reg2 == 0) {
        if(ins->op2->type == Type::constVal) {
            int add2 = ins->op2->value>=0 ? (16 << 26) : (17 << 26);
            add2 |= (26 << 21);
            add2 |= ins->op2->value>=0 ? ins->op2->value : -(ins->op2->value);
            // printf("add ld2: %d\n", add2);
            code.push_back(add2);
            reg2 = 26;
        }
    }
    else if(reg2 == -1) {
        // printf("need to load from memory...\n", globals[ins->op2->name]);
        reg2 = 26;
        int ld2 = (32 << 26);
        ld2 |= (26 << 21);
        ld2 |= (30 << 16);
        int offset = globals[ins->op2->name] & 0xffff;
        ld2 |= offset;
        code.push_back(ld2);
    }
    int mul = (2 << 26);
    if(regdest != -1) {
        mul |= (regdest << 21);
    }
    else {
        regdest = 27;
        mul |= (27 << 21);
    }
    mul |= (reg1 << 16);
    mul |= reg2;
    code.push_back(mul);

    if(regdest == 27) {
        // printf("adding store at mul...\n");
        int store = (36 << 26);
        store |= (27 << 21);
        store |= (30 << 16);
        int offset = globals[ins->dest->name] & 0xffff;
        store |= offset;
        code.push_back(store);
        // printf("store at mul: %d\n", store);
    }
}

void DLXGenerator::genDiv(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    int regdest = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : -1;

    if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            reg1 = 27;
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
        }
    }
    else if(reg1 == -1) {
        // printf("loading from memory...%d\n", globals[ins->op1->name]);
        reg1 = 27;
        int ld1 = (32 << 26);
        ld1 |= (27 << 21);
        ld1 |= (30 << 16);
        int offset = globals[ins->op1->name] & 0xffff;
        ld1 |= offset;
        code.push_back(ld1);
    }
    if(reg2 == 0) {
        if(ins->op2->type == Type::constVal) {
            reg2 = 26;
            int add2 = ins->op2->value>=0 ? (16 << 26) : (17 << 26);
            add2 |= (26 << 21);
            add2 |= ins->op2->value>=0 ? ins->op2->value : -(ins->op2->value);
            code.push_back(add2);
        }
    }
    else if(reg2 == -1) {
        // printf("need to load from memory...\n", globals[ins->op2->name]);
        reg2 = 26;
        int ld2 = (32 << 26);
        ld2 |= (26 << 21);
        ld2 |= (30 << 16);
        int offset = globals[ins->op2->name] & 0xffff;
        ld2 |= offset;
        code.push_back(ld2);
    }
    int div = (3 << 26);
    if(regdest != -1) {
        div |= (regdest << 21);
    }
    else {
        regdest = 27;
        div |= (27 << 21);
    }
    div |= (reg1 << 16);
    div |= reg2;
    code.push_back(div);

    if(regdest == 27) {
        // printf("adding store at div...\n");
        int store = (36 << 26);
        store |= (27 << 21);
        store |= (30 << 16);
        int offset = globals[ins->dest->name] & 0xffff;
        store |= offset;
        code.push_back(store);
        // printf("store at div: %d\n", store);
    }
}

void DLXGenerator::genCmp(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    int regdest = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : -1;
    // printf("reg for mul: %d %d %d\n", reg1, reg2, regdest);

    if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
            // printf("add ld1: %d\n", add1);
            reg1 = 27;
        }
    }
    else if(reg1 == -1) {
        // printf("loading from memory...%d\n", globals[ins->op1->name]);
        reg1 = 27;
        int ld1 = (32 << 26);
        ld1 |= (27 << 21);
        ld1 |= (30 << 16);
        int offset = globals[ins->op1->name] & 0xffff;
        ld1 |= offset;
        code.push_back(ld1);
    }
    if(reg2 == 0) {
        if(ins->op2->type == Type::constVal) {
            int add2 = ins->op2->value>=0 ? (16 << 26) : (17 << 26);
            add2 |= (26 << 21);
            add2 |= ins->op2->value>=0 ? ins->op2->value : -(ins->op2->value);
            // printf("add ld2: %d\n", add2);
            code.push_back(add2);
            reg2 = 26;
        }
    }
    else if(reg2 == -1) {
        // printf("need to load from memory...\n", globals[ins->op2->name]);
        reg2 = 26;
        int ld2 = (32 << 26);
        ld2 |= (26 << 21);
        ld2 |= (30 << 16);
        int offset = globals[ins->op2->name] & 0xffff;
        ld2 |= offset;
        code.push_back(ld2);
    }
    int cmp = (5 << 26);
    if(regdest != -1) {
        cmp |= (regdest << 21);
    }
    else {
        regdest = 27;
        cmp |= (27 << 21);
    }
    cmp |= (reg1 << 16);
    cmp |= reg2;
    code.push_back(cmp);

    if(regdest == 27) {
        // printf("adding store at cmp...\n");
        int store = (36 << 26);
        store |= (27 << 21);
        store |= (30 << 16);
        int offset = globals[ins->dest->name] & 0xffff;
        store |= offset;
        code.push_back(store);
        // printf("store at cmp: %d\n", store);
    }
}

void DLXGenerator::genJmp(Instruction* ins, string funcName, int kind) {
    switch(kind) {
        case 1: {
            int jmp = (40 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        case 2: {
            int jmp = (41 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        case 3: {
            int jmp = (42 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        case 4: {
            int jmp = (43 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        case 5: {
            int jmp = (44 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        case 6: {
            int jmp = (45 << 26);
            jmp |= (27 << 21);
            jmp |= (ins->op2->value << 16);
            code.push_back(jmp);
            break;
        }
        default: {
            break;
        }
    }
}

void DLXGenerator::genRet(Instruction* ins, string funcName) {
    if(ins->op1->type != Type::empty) {

    }
    else {
        if(funcName == "main")
            code.push_back((49 << 26));
    }
}

void DLXGenerator::genMove(Instruction* ins, string funcName) {
    int reg1 = regallocs[funcName][ins->op1->name];
    int reg2 = regallocs[funcName][ins->op2->name];
    // printf("reg1: %d, reg2: %d\n", reg1, reg2);
    if(reg1 == -1) {
        reg1 = 27;
        int ld = (32 << 26);
        ld |= (27 << 21);
        ld |= (30 << 16);
        ld |= (globals[ins->op1->name] & 0xffff);
        code.push_back(ld);
    }
    else if(reg1 == 0) {
        if(ins->op1->type == Type::constVal) {
            int add1 = ins->op1->value>=0 ? (16 << 26) : (17 << 26);
            add1 |= (27 << 21);
            add1 |= ins->op1->value>=0 ? ins->op1->value : -(ins->op1->value);
            code.push_back(add1);
            // printf("add ld1: %d\n", add1);
            reg1 = 27;
        }
    }

    if(reg2 == -1) {
        reg2 = 26;
    }

    int add = (1 << 26);
    add |= (reg2 << 21);
    add |= (reg1 << 16);
    add &= 0xffff0000;
    // printf("move res: %d\n", add);
    code.push_back(add);

    if(reg2 == 26) {
        int st = (36 << 26);
        st |= (26 << 21);
        st |= (30 << 16);
        st |= (globals[ins->op2->name] & 0xffff);
        code.push_back(st);
    }
    return;
}

void DLXGenerator::genRead(Instruction* ins, string funcName) {
    int reg = regallocs[funcName].count(ins->dest->name) ? regallocs[funcName][ins->dest->name] : 27;
    // printf("read into reg: %d\n", reg);
    int rd = 50 << 26;
    rd |= (reg << 21);
    code.push_back(rd);
}

void DLXGenerator::genWrite(Instruction* ins, string funcName) {
    if(ins->op1->type == Type::constVal) {
        int ld = 16 << 26;
        ld |= (27 << 21);
        ld |= ins->op1->value;
        code.push_back(ld);
        // printf("write load: %d\n", ld);
        int wr = 51 << 26;
        wr |= (27 << 16);
        // printf("write wr: %d\n", wr);
        code.push_back(wr);
    }
    else {
        int reg = regallocs[funcName][ins->op1->name] != -1 ? regallocs[funcName][ins->op1->name] : 27;
        if(reg == 27) {
            int ld = (32 << 26);
            ld |= (27 << 21);
            ld |= (30 << 16);
            ld |= globals[ins->op1->name] & 0xffff;
            code.push_back(ld);
        }
        // printf("op1 reg: %d\n", reg);
        int wr = 51 << 26;
        wr |= (reg << 16);
        // printf("write wr: %d\n", wr);
        code.push_back(wr);
    }
}

void DLXGenerator::genWriteNL() {
    code.push_back((53 << 26));
}

void DLXGenerator::dlxgenBlk(SSABuilder builder, BasicBlock* block, string funcName, int index) {
    blkstart[funcName][index] = code.size();
    // printf("index for blk %d is %d\n", index, blkstart[funcName][index]);
    for(auto ins: block->instructions) {
        switch(ins->opcode) {
            case OpCode::ADD: {
                // printf("gen add\n");
                genAdd(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::SUB: {
                // printf("gen sub\n");
                genSub(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::MUL: {
                // printf("gen mul\n");
                genMul(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::DIV: {
                // printf("gen div\n");
                genDiv(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::READ: {
                // printf("gen read\n");
                genRead(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::WRITE: {
                // printf("gen write\n");
                genWrite(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::WRITENL: {
                // printf("gen writenl\n");
                genWriteNL();
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::MOVE: {
                // printf("gen move\n");
                genMove(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::CMP: {
                // printf("gen cmp\n");
                genCmp(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BEQ: {
                // printf("gen beq\n");
                genJmp(ins, funcName, 1);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BNE: {
                // printf("gen bne\n");
                genJmp(ins, funcName, 2);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BLT: {
                // printf("gen blt\n");
                genJmp(ins, funcName, 3);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BGE: {
                // printf("gen bge\n");
                genJmp(ins, funcName, 4);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BLE: {
                // printf("gen ble\n");
                genJmp(ins, funcName, 5);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::BGT: {
                // printf("gen bgt\n");
                genJmp(ins, funcName, 6);
                // printf("gen res: %d\n", code.back());
                break;
            }
            case OpCode::RET: {
                // printf("gen ret\n");
                genRet(ins, funcName);
                // printf("gen res: %d\n", code.back());
                break;
            }
            default: {
                break;
            }
        }
    }
    // printf("blk %d generated %d ins before\n", index, code.size());
    if(block->successors.size() == 0) {
        // if(block->instructions.size() == 0) {
        code.push_back((49 << 26));
        // }
    }
    else {
        // printf("blkstart: %s, %d\n", funcName.c_str(), block->successors.size());
        // int fall = blkstart[funcName][block->successors[0]];
        int jmp = (46 << 26);
        // jmp |= (fall * 4) & 0x03ffffff;
        code.push_back(jmp);
    }
    // printf("blk %d generated %d ins\n", index, code.size());
}

void DLXGenerator::dlxgenFunc(SSABuilder builder, vector<BasicBlock*>* blocks, string funcName) {
    int blkIndex = 0;
    for(auto blk: *blocks) {
        dlxgenBlk(builder, blk, funcName, blkIndex);
        blkend[funcName][blkIndex] = code.size()-1;
        blkIndex++;
    }
    blkIndex = 0;
    for(auto blk: *blocks) {
        int last_2 = code[blkend[funcName][blkIndex]-1];
        // printf("opcode: %d\n", (last_2 & (0xfc000000)) >> 26);
        if(((last_2 & (0xfc000000)) >> 26) >= 40 && ((last_2 & (0xfc000000)) >> 26) <= 45) {
            // printf("opcode: %d\n", (last_2 & (0xfc000000)) >> 26);
            // printf("dest blk: %d\n", (last_2 & 0x001f0000) >> 16);
            int dest = blkstart[funcName][(blk->successors[1])];
            // printf("dest ins index: %d, opcode: %d\n", dest, (code[dest] & 0xfc000000) >> 26);
            int diff = dest - (blkend[funcName][blkIndex]-1);
            // printf("cur code: %d, target code: %d, diff: %d\n",(last_2 & (0xfc000000)) >> 26, (code[dest] & 0xfc000000) >> 26, diff);
            // printf("dest code: ");
            // for(int i=blkstart[funcName][(last_2 & 0x001f0000) >> 16]; i<=blkend[funcName][(last_2 & 0x001f0000) >> 16]; i++){
            //     printf("%d ", (code[i] & 0xfc000000) >> 26);
            // }
            // printf("\n");
            code[blkend[funcName][blkIndex]-1] |= diff & (0xffff);
            // printf("opcode: %d\n", (code[blkend[funcName][blkIndex]-1] & 0xfc000000) >> 26);
        }
        // int last = code[blkend[funcName][blkIndex]];
        // int target = (last & 0x03ffffff);
        // printf("fallthrough target: %d\n", target);
        // code[blkend[funcName][blkIndex]] &= 0xfc000000;
        if(blk->successors.size() > 0) {
            // printf("fallthrough target: %d %d\n", (blk->successors[0]), blkstart[funcName][(blk->successors[0])]);
            // printf("fallthrough index: %d\n", blkstart[funcName][(blk->successors[0])] - blkend[funcName][blkIndex]);
            code[blkend[funcName][blkIndex]] |= ((blkstart[funcName][(blk->successors[0])] - blkend[funcName][blkIndex]) & 0x03ffffff);
        }
        blkIndex++;
    }
}

void DLXGenerator::dlxgen(SSABuilder builder) {
    // init global area
    // printf("%d\n", sizeof(short int));
    // for(short int i=0;i<globals.size();i++) {
    //     int stw = (36 << 26);
    //     stw |= (30 << 16);
    //     stw |= (-i & (65535));
    //     code.push_back(stw);
    // }

    // generate main
    auto main_blocks = builder.blocks["main"];
    dlxgenFunc(builder, main_blocks, "main");
    // add return 0 for main.
    // code.push_back((49 << 26));

    // for(auto blk: *main_blocks) {
    //     for(auto ins: blk->instructions) {
    //         switch(ins->opcode) {
    //             case OpCode::ADD: {
    //                 // printf("gen add\n");
    //                 genAdd(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::SUB: {
    //                 // printf("gen sub\n");
    //                 genSub(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::MUL: {
    //                 // printf("gen mul\n");
    //                 genMul(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::DIV: {
    //                 // printf("gen div\n");
    //                 genDiv(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::READ: {
    //                 // printf("gen read\n");
    //                 genRead(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::WRITE: {
    //                 // printf("gen write\n");
    //                 genWrite(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::WRITENL: {
    //                 // printf("gen writenl\n");
    //                 genWriteNL();
    //                 // printf("gen res: %d\n", code.back());
    //                 break;
    //             }
    //             case OpCode::MOVE: {
    //                 // printf("gen move\n");
    //                 genMove(ins, "main");
    //                 // printf("gen res: %d\n", code.back());
    //             }
    //             default: {
    //                 break;
    //             }
    //         }
    //     }
    // }

    printf("dumping code: ");
    for(auto i: code) {
        printf("%d ", (i & 0xfc000000) >> 26);
    }
    printf("\n");
}

void DLXGenerator::dump(string name) {
    ofstream outfile;
    outfile.open(name.c_str(), ios::out);
    for(auto i: code) {
        outfile << i << endl;
    }
}