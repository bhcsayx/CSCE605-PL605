#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../IR/IR.h"
#include "graphviz.h"

using namespace std;

string val2txt(Value val) {
    switch(val.type) {
        case Type::def: {
            return val.name;
            break;
        }
        case Type::empty: {
            string res("");
            return res;
        }
        case Type::insRes: {
            string res("%");
            auto cstr = std::to_string(val.index);
            res.append(cstr.c_str());
            return res;
        }
        case Type::constVal: {
            auto cstr = std::to_string(val.value);
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
            string res("add");
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
        int blkIdx = 0;
        for(auto blk: func.blocks) {
            res.push_back(tabs);
            string name("bb");
            name.append(std::to_string(blkIdx).c_str());
            blkIdx++;
            res.push_back(name);
            for (auto ins: blk.instructions) {
                string ins_str("");
                ins_str.append(op2txt(ins.opcode).c_str());
                ins_str.append(" ");
                if(ins.opcode == OpCode::CALL) {
                    ins_str.append(val2txt(ins.op1).c_str());
                    ins_str.append(" ");
                    for(auto arg: ins.callArgs) {
                        ins_str.append(val2txt(arg).c_str());
                        ins_str.append(" ");
                    }
                }
                else {
                    ins_str.append(val2txt(ins.op1).c_str());
                    ins_str.append(" ");
                    ins_str.append(val2txt(ins.op2).c_str());
                    ins_str.append(" ");
                }
                res.push_back(ins_str);
            }
        }
    }
    for(auto s:res) {
        std::cout << s << endl;
    }
    return res;
}