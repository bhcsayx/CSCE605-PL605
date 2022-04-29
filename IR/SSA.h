#ifndef SSA_H
#define SSA_H

#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "IR.h"

using namespace std;

class SSABuilder {
public:

    vector<string> varNames;
    vector<string> funcNames;
    map<string, vector<BasicBlock*>*> blocks;
    map<string, vector<BasicBlock*>*> dfs;
    map<string, map<BasicBlock*, vector<BasicBlock*>*>> DomTrees;
    map<string, map<BasicBlock*, vector<BasicBlock*>*>> DFTrees;

    map<string, map<string, vector<BasicBlock*>*>>blocksOf;
    map<string, map<BasicBlock*, vector<string>*>> phiNodes;
    map<string, int> counter;
    map<string, vector<int>*> stack;

    void renameVarinBlk(string funcName, string name, BasicBlock* blk);
    void recurDFS(vector<BasicBlock*>* blocks, BasicBlock* cur, vector<BasicBlock*>* dest, int& mask); //, int& dfnid);

    SSABuilder(Module mod);
    void computeDomTree();
    void computeDFTree();
    void getBlocksofVar();
    void insertPhiNode();
    void renameVar(string name);
    void transform();
};

#endif