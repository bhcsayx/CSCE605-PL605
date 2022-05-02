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

// void DLXGenerator::genAdd(Instruction* ins) {
//     if(ins->op1->type == Type::constVal) {
//         // handle unoptimized case
//         int ld1 = 16 << 26;
//         ld1 |= (27 << 21);
//         ld1 |= ins->op1->val;
//         code.push_back(ld1);
//         // int tmp1 = regavail++;

//         int dest = regallocs[curFunc][ins->dest->name] == -1 ? 26 : regallocs[curFunc][ins->dest->name];
//         if(ins->op2->type == Type::constVal) {
//             int ld2 = 16 << 26;
//             ld2 |= (26 << 21);
//             ld2 |= ins->op2->val;
//             code.push_back(ld2);
//         }

//     }

// }

DLXGenerator::DLXGenerator(regAlloc alloc) {
    for(auto res_iter: alloc.res) {
        auto funcName = res_iter.first;
        for(auto set_iter: res_iter.second) {
            auto valName = set_iter.first;
            regallocs[funcName][valName] = set_iter.second;
        }
    }
}

DLXGenerator::~DLXGenerator() {

}

void DLXGenerator::genWrite(Instruction* ins) {
    if(ins->op1->type == Type::constVal) {
        int ld = 16 << 26;
        ld |= (27 << 21);
        ld |= ins->op1->value;
        code.push_back(ld);

        int wr = 51 << 26;
        wr |= (27 << 16);
        code.push_back(wr);
    }
}

void DLXGenerator::dlxgen(SSABuilder builder) {
    // generate main
    auto main_blocks = builder.blocks["main"];
    for(auto blk: *main_blocks) {
        for(auto ins: blk->instructions) {
            switch(ins->opcode) {
                // case OpCode::ADD: {
                //     genAdd(ins);
                //     break;
                // }
                // case OpCode::SUB: {
                //     genSub(ins);
                //     break;
                // }
                // case OpCode::MUL: {
                //     genMul(ins);
                //     break;
                // }
                // case OpCode::DIV: {
                //     genDiv(ins);
                //     break;
                // }
                case OpCode::WRITE: {
                    genWrite(ins);
                }
                default: {
                    break;
                }
            }
        }
    }

    int ret = (49 << 26);
    code.push_back(ret);

    printf("dumping code: ");
    for(auto i: code) {
        printf("%d ", i);
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