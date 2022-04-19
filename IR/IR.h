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
    constVal,
    eliminated
};

enum OpCode {
    NEG,ADD,SUB,
    MUL,DIV,CMP,
    MOVE,PHI,END,
    BRA,BNE,BEQ,
    BLE,BLT,BGE,
    BGT,MOVE,READ,
    WRITE,WRITENL,CALL
};

class Value {
public:
    int index;
    int value;
    enum Type type;
    string name;

    // Value();
    // ~Value();
    Value(int value): value(value), type(Type::constVal) {};
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
    class Function* func;
    std::vector<Instruction> instructions;
    std::vector<int> predecessors;
    std::vector<int> successors;

    // BasicBlock();
    BasicBlock(Function* func, int index):func(func), index(index) {};
    struct Value* addInstruction(struct BasicBlock* block, struct Value* op1, struct Value* op2, enum Token op);
};

class Function {
public:
    string name;
    class Module* module;
    std::vector<BasicBlock> blocks;

    // Function();
    // ~Function();
    Function(Module* module, string name):module(module), name(name) {};
};

class Module {
public:
    int funcLen = 0;
    int varLen = 0;
    symbolTable symbolTable;
    std::vector<Value> values;
    std::vector<Function> funcs;

    // Module();
    // ~Module();
};

Value newConstValue(int value);
// struct Value {
//     int index;
//     enum Type type;
//     char* data;
//     // struct Value* uses;
// };

// struct Instruction {
//     enum OpCode opcode;
//     int dest;
//     int op1;
//     int op2;
// };

// struct BasicBlock {
//     int index;
//     int insLen=0;
//     int valLen=0;
//     struct Instruction* instructions;
//     struct Value* values;
// };

// struct Function {
//     char* name;
//     int blockLen = 0;
//     struct BasicBlock* blocks;
// };

// struct Module {
//     int funcLen = 0;
//     int varLen = 0;
//     struct Value* vars;
//     struct Function* funcs;
// };

#endif