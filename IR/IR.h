#ifndef IR_H
#define IR_H

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "symbolTable.h"

#include "../frontend/scanner.h"
#include "../frontend/parser.h"

using namespace std;

enum Type {
    def,
    empty,
    insRes,
    constVal
};

enum OpCode {
    NEG,ADD,SUB,
    MUL,DIV,CMP,
    MOVE,PHI,END,
    BRA,BNE,BEQ,
    BLE,BLT,BGE,
    BGT,READ,WRITE,
    WRITENL,CALL,RET
};

class Value {
public:
    int index;
    int value;
    bool eliminated = false;
    enum Type type;
    string name;

    Value();
    Value(int value): value(value), type(Type::constVal) {};
    ~Value();
};

class Global {
public:
    symbolTable symbolTable;
    std::vector<Value> values;
    Value addValue();
    Value addValue(int v);
};

struct Instruction {
    enum OpCode opcode;
    Value dest;
    Value op1;
    Value op2;
    std::vector<Value> callArgs;
};

class BasicBlock {
public:
    int index = 0;
    std::vector<Instruction> instructions;
    std::vector<int> predecessors;
    std::vector<int> successors;

    BasicBlock();
    ~BasicBlock();
    Value addInstruction(Value op1, Value op2, enum Token op, Global& glob);
    Value addCallInstruction(string name, vector<Value> args, Global& glob);
};

class Function {
public:
    string name;
    std::vector<BasicBlock> blocks;

    // Function();
    // ~Function();
    Function(string name):name(name) {};
    BasicBlock& addBasicBlock();
};

class Module {
public:
    int funcLen = 0;
    std::vector<Function> funcs;

    // Module();
    // ~Module();
    Function& addFunction(string name);
};

Value constValue(int value);
Value emptyValue();

#endif