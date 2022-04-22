#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../frontend/scanner.h"
#include "IR.h"

Value* constValue(int value) {
    Value* res = new Value(value);
    return res;
}

Value* emptyValue() {
    Value* res = new Value();
    res->type = empty;
    return res;
}

// Value::Value() {

// }

Value::~Value() {

}

BasicBlock::BasicBlock() {

}

BasicBlock::~BasicBlock() {

}

Value* BasicBlock::addInstruction(Value* op1, Value* op2, Token op, Global& glob, Instruction* place) {
    // struct Instruction place;
    place->op1 = op1;
    place->op2 = op2;
    switch(op) {
        case plusToken: {
            place->opcode = OpCode::ADD;
            break;
        }
        case minusToken: {
            place->opcode = OpCode::SUB;
            break;
        }
        case timesToken: {
            place->opcode = OpCode::MUL;
            break;
        }
        case divToken: {
            place->opcode = OpCode::DIV;
            break;
        }
        case assignToken: {
            place->opcode = OpCode::MOVE;
            // printf("val type: %d\n", place->op1.type);
            break;
        }
        case readToken: {
            place->opcode = OpCode::READ;
            break;
        }
        case writeToken: {
            place->opcode = OpCode::WRITE;
            break;
        }
        case writeNLToken: {
            place->opcode = OpCode::WRITENL;
            break;
        }
        case returnToken: {
            place->opcode = OpCode::RET;
            break;
        }
        case cmpToken: {
            place->opcode = OpCode::CMP;
            break;
        }
        case eqlToken: {
            place->opcode = OpCode::BEQ;
            break;
        }
        case neqToken: {
            place->opcode = OpCode::BNE;
            break;
        }
        case lssToken: {
            place->opcode = OpCode::BLT;
            break;
        }
        case gtrToken: {
            place->opcode = OpCode::BGT;
            break;
        }
        case leqToken: {
            place->opcode = OpCode::BLE;
            break;
        }
        case geqToken: {
            place->opcode = OpCode::BGE;
            break;
        }
        default:
            break;
    };
    // place->dest = ((Module*)(((Function*)(this->func))->module))->addValue();
    place->dest = new Value();
    glob.addValue(place->dest);
    instructions.push_back(place);
    return place->dest;
}

Value* BasicBlock::addCallInstruction(string name, vector<Value*> args, Value* func, Global& glob, Instruction* place) {
    // struct Instruction place;
    func->name = name;
    func->type = Type::def;
    place->op1 = func;
    place->op2 = emptyValue();
    place->opcode = OpCode::CALL;
    for(int i=0;i<args.size();i++)
        place->callArgs.push_back(args[i]);
    // place->dest = ((Module*)(((Function*)(this->func))->module))->addValue();
    place->dest = new Value();
    glob.addValue(place->dest);
    instructions.push_back(place);
    return place->dest;
}

void Function::addBasicBlock(BasicBlock* block) {
    blocks.push_back(block);
}

Value* Global::addValue(Value* res) {
    res->index = values.size();
    res->type = Type::insRes;
    values.push_back(res);
    return res;
}

Function& Module::addFunction(string name) {
    Function func = Function(name);
    this->funcs.push_back(func);
    return this->funcs.back();
}