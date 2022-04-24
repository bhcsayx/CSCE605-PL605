#ifndef SSA_H
#define SSA_H

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "IR.h"

using namespace std;

class SSABuilder {
public:

    vector<string> varNames;
    map<string, vector<BasicBlock*>*> blocks;
    map<string, vector<BasicBlock*>*> dfs;
    map<string, map<BasicBlock*, vector<BasicBlock*>*>*> DomTrees;
    map<string, map<BasicBlock*, vector<BasicBlock*>*>*> DFTrees;

    map<BasicBlock*, bool> visited;

    void SSABuilder::recurDFS(vector<BasicBlock*>* blocks, BasicBlock* cur, vector<BasicBlock*>* dest, int& mask);

    SSABuilder(Module mod);
    void computeDomTree();
    void computeDFTree();
    void renameVar(string name);
    void transform();
};

#endif