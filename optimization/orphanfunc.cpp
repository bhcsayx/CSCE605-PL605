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

#include "orphanfunc.h"

using namespace std;

orphanFunc::orphanFunc() {}
orphanFunc::~orphanFunc() {}

void orphanFunc::run(SSABuilder builder) {
    for(auto funcName: builder.funcNames)
        orphan[funcName] = false;
    orphan["main"] = true;
    auto map_iter = builder.blocks.begin();
    while(map_iter != builder.blocks.end()) {
        auto funcName = map_iter->first;
        for(auto blk: *(builder.blocks[funcName])) {
            for(auto ins: blk->instructions) {
                if(ins->opcode == OpCode::CALL)
                    orphan[ins->op1->name] = true;
            }
        }
        map_iter++;
    }
}

void orphanFunc::dump(SSABuilder builder, string name) {
    Dot dot;
    for(auto funcName: builder.funcNames) {
        // printf("funcname: %s\n", funcName.c_str());
        // for(auto blk: *(blks_iter->second)) {
        for(auto blk: *(builder.blocks[funcName])) {
            string name = funcName;
            if(orphan[funcName] == false)
                continue;
            name.append("_BB"); name.append(to_string(blk->index));
            dot.addNode(name);
            for(auto ins: blk->instructions)
                dot.addInstruction(name, ins);
            if(blk->successors.size() > 0) {
                if(blk->successors.size() == 2) {
                    string br1 = funcName;
                    br1.append("_BB"); br1.append(to_string(blk->successors[0]));
                    dot.addCFGEdge(name, br1, "branch");
                    string br2 = funcName;
                    br2.append("_BB"); br2.append(to_string(blk->successors[1]));
                    dot.addCFGEdge(name, br2, "fall-through");
                }
                else {
                    string br1 = funcName;
                    br1.append("_BB"); br1.append(to_string(blk->successors[0]));
                    dot.addCFGEdge(name, br1, "fall-through");
                }
            }
        }

        for(auto dom: builder.DomTrees[funcName]) {
            string lname = funcName;
            lname.append("_BB"); lname.append(to_string(dom.first->index));
            if(dom.second == NULL)
                continue;
            for(auto r: *(dom.second)) {
                string rname = funcName;
                rname.append("_BB"); rname.append(to_string(r->index));
                dot.addDomEdge(lname, rname);
            }
        }

        for(auto df: builder.DFTrees[funcName]) {
            string lname = funcName;
            lname.append("_BB"); lname.append(to_string(df.first->index));
            if(df.second == NULL)
                continue;
            for(auto r: *(df.second)) {
                string rname = funcName;
                rname.append("_BB"); rname.append(to_string(r->index));
                dot.addDFEdge(lname, rname);
            }
        }
        // blks_iter++;
    }
    dot.dump(name);
}