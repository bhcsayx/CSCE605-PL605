#ifndef DLXGEN_H
#define DLXGEN_H

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

class DLXGenerator {
public:
    vector<int> code;
    map<string, map<string, int>> regallocs;
    map<string, short int> globals;
    map<int, int> blkstart;
    map<int, int> blkend;
    string curFunc;
    int regavail;

    DLXGenerator(SSABuilder builder, regAlloc alloc);
    ~DLXGenerator();
    void dlxgen(SSABuilder builder);
    void dlxgenFunc(SSABuilder builder, vector<BasicBlock*>* blocks, string funcName);
    void dlxgenBlk(SSABuilder builder, BasicBlock* block, string funcName, int index);

    void genAdd(Instruction* ins, string funcName);
    void genSub(Instruction* ins, string funcName);
    void genMul(Instruction* ins, string funcName);
    void genDiv(Instruction* ins, string funcName);
    void genCmp(Instruction* ins, string funcName);

    void genRet(Instruction* ins, string funcName);

    void genMove(Instruction* ins, string funcName);
    void genRead(Instruction* ins, string funcName);
    void genWrite(Instruction* ins, string funcName);
    void genWriteNL();

    void dump(string name);
};

#endif