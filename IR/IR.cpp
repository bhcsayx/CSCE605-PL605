#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "IR.h"

Value newConstValue(int value) {
    return Value res(value);
}

Value addEmptyValue() {
    Value res;
    res.type = empty;
    return res;
}

Value Module::addValue() {
    Value res;
    res.index = values.len();
    res.type = Type::insRes;
    values.push(res);
    return res;
}

Value BasicBlock::addInstruction(Value op1, Value op2, Token op) {
    struct Instruction place;
    place.op1 = op1;
    place.op2 = op2;
    switch(op) {
        case plusToken: {
            place.opcode = OpCode::ADD;
            break;
        }
        case minusToken: {
            place.opcode = OpCode::SUB;
            break;
        }
        case timesToken: {
            place.opcode = OpCode::MUL;
            break;
        }
        case divToken: {
            place.opcode = OpCode::DIV;
            break;
        }
        case assignToken: {
            place.opcode = OpCode::MOVE;
            break;
        }
        default:
            break;
    };
    place.dest = this->func->module->addValue();
    return place.dest;
}

Value BasicBlock::addCallInstruction(string name, vector<Value> args) {
    struct Instruction place;
    Value function;
    function.name = name;
    function.type = Type::def;
    place.op1 = function;
    place.op2 = addEmptyValue();
    place.opcode = OpCode::CALL;
    for(int i=0;i<args.len();i++)
        place.callArgs.push(args[i]);
    place.dest = this->func->module->addValue();
    return place.dest;
}