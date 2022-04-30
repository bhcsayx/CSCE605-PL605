
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../frontend/scanner.h"
#include "IR.h"
#include "codegen.h"
#include "../graphviz/graphviz.h"

using namespace std;

int valIndex = 0;
Global glob;

Value* codegen(struct desiAST* desi, BasicBlock* block, bool is_left=false) {
    if(desi->type == 0) {
        string name(desi->id);
        // printf("looking up for name: %s\n", name.c_str());
        // int index = ((Module*)(((Function*)(block->func))->module))->symbolTable.lookupSymbol(name);
        // if(block->glob == NULL) {
        //     printf("error.");
        // }
        // printf("skipped\n");
        // if(is_left) {
        //     Value* res = new Value();
        //     res->value = -1;
        //     res->type = Type::def;
        //     res->name = name;
        //     return res;
        // }
        int index = glob.symbolTable.lookupSymbol(name);
        // printf("index of var %s is %d\n", name.c_str(), index);
        if (index == -2) {
            printf("using undefined var: %s\n", name.c_str());
            exit(-1);
        }
        // if (index == -1) {
            Value* res = new Value();
            res->type = Type::def;
            res->name = name;
            return res;
        // }
        // glob.values[index]->name = name;
        // return glob.values[index];
    }
}

Value* codegen(struct factorAST* factor, BasicBlock* block) {
    switch(factor->type) {
        case 0: {
            return codegen((struct desiAST*)(factor->data), block);
            break;
        }
        case 1: {
            int value = *(int*)(factor->data);
            return constValue(value);
            break;
        }
        case 2: {
            return codegen((struct exprAST*)(factor->data), block);
            break;
        }
        case 3: {
            Value* res = codegen((struct funcCallAST*)(factor->data), block);
            // printf("faccall res: %s\n", res->name.c_str());
            return res;
        }
        default: {
            printf("unreachable in factor\n");
            exit(-1);
        }
    }
}

Value* codegen(struct termAST* term, BasicBlock* block) {
    Value* cur = codegen(term->factor, block);
    // printf("term res: %s\n", cur->name.c_str());
    termTailAST* curAST = term->next;
    while(curAST) {
        Value* extra = codegen(curAST->factor, block);
        Instruction* ins = new Instruction();
        if(curAST->op == Token::timesToken)
            cur = block->addInstruction(cur, extra, timesToken, ins, valIndex);
        else if(curAST->op == Token::divToken)
            cur = block->addInstruction(cur, extra, divToken, ins, valIndex);
        curAST = curAST->next;
    }
    // printf("term res: %s\n", cur->name.c_str());
    return cur;
}

Value* codegen(struct exprAST* expr, BasicBlock* block) {
    // printf("term: %d\n", expr->term);
    Value* cur = codegen(expr->term, block);
    exprTailAST* curAST = expr->next;
    // printf("%d\n", curAST);
    while(curAST) {
        // printf("op: %d\n", curAST->op);
        Value* extra = codegen(curAST->term, block);
        Instruction* ins = new Instruction();
        if(curAST->op == Token::plusToken)
            cur = block->addInstruction(cur, extra, plusToken, ins, valIndex);
        else if(curAST->op == Token::minusToken)
            cur = block->addInstruction(cur, extra, minusToken, ins, valIndex);
        curAST = curAST->next;
    }
    // printf("ret expr: %s\n", cur->name.c_str());
    return cur;
}

Value* codegen(struct assignAST* assign, BasicBlock* block) {
    Value* lhs = codegen(assign->lhs, block, true);
    switch(assign->op) {
        case increaseToken: {
            Value* one = constValue(1);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, one, Token::plusToken, tmp_ins, valIndex);
            // Value* tmp2 = new Value(); tmp2->index = tmp->index; tmp2->name = tmp->name; tmp2->type = tmp->type;
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case decreaseToken: {
            Value* one = constValue(1);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, one, Token::minusToken, tmp_ins, valIndex);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selfplusToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, rhs, Token::plusToken, tmp_ins, valIndex);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selfminusToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, rhs, Token::minusToken, tmp_ins, valIndex);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selftimesToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, rhs, Token::timesToken, tmp_ins, valIndex);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            return res;
            break;
        }
        case selfdivToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp_lhs = new Value(); tmp_lhs->index = lhs->index; tmp_lhs->name = lhs->name; tmp_lhs->type = lhs->type;
            Value* tmp = block->addInstruction(tmp_lhs, rhs, Token::divToken, tmp_ins, valIndex);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, ins, valIndex);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            return res;
            break;
        }
        case assignToken: {
            Value* rhs = codegen(assign->rhs, block);
            // printf("rhs name:%s\n", rhs->name.c_str());
            Instruction* ins = new Instruction();
            Value* res = block->addInstruction(rhs, lhs, Token::assignToken, ins, valIndex);
            // printf("%s %d\n", lhs.name.c_str(), rhs.index);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            // int index = glob.symbolTable.lookupSymbol(lhs->name);
            // printf("after assign index of var %s is %d\n", lhs->name.c_str(), index);
            // printf("blk len after assign: %d\n", block->instructions.size());
            // for(auto k: glob.symbolTable.table.back()) {
            //     printf("%s, %d\n", k.first.c_str(), k.second);
            // }
            // printf("rhs name 2:%s\n", res->name.c_str());
            return res;
            break;
        }
        default: {
            printf("error in assign code generation: %d\n", assign->op);
            exit(-1);
        }
    }
}

std::vector<Value*> codegen(struct exprListAST* list, BasicBlock* block) {
    std::vector<Value*> res;
    struct exprListAST* cur = list;
    while(cur) {
        Value* v = codegen(cur->head, block);
        res.push_back(v);
        cur = cur->next;
    }
    return res;
}

Value* codegen(struct relAST* rel, BasicBlock* block, int jmpIndex) {
    // printf("generating rel at block: %d\n", block->index);
    Value* lhs = codegen(rel->lhs, block);
    // printf("rel lhs finished %s\n", lhs->name.c_str());
    Value* rhs = codegen(rel->rhs, block);
    // printf("rel rhs finished%s\n", rhs->name.c_str());
    Instruction* cmp = new Instruction();
    Value* tmp = block->addInstruction(lhs, rhs, cmpToken, cmp, valIndex);
    // printf("cmp finished%d\n", rel->op);
    Value* empty = constValue(jmpIndex);
    switch(rel->op) {
        case eqlToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, eqlToken, res, valIndex);
        }
        case neqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, neqToken, res, valIndex);
        }
        case lssToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, lssToken, res, valIndex);
        }
        case gtrToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, gtrToken, res, valIndex);
        }
        case leqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, leqToken, res, valIndex);
        }
        case geqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, geqToken, res, valIndex);
        }
        default: {
            printf("error in relation code generation: %d\n", rel->op);
            exit(-1);
        }
    }
}

Value* codegen(struct funcCallAST* call, BasicBlock* block) {
    // printf("handling call %s\n", call->funcName);
    string name(call->funcName);
    Instruction* ins = new Instruction();
    Value* empty = emptyValue();
    if(name == "InputNum") {
        Value* res = block->addInstruction(empty, empty, readToken, ins, valIndex);
        return res;
    }
    else if(name == "OutputNum") {
        Value* num = codegen(call->args, block)[0];
        // printf("output num name: %s\n", num->name.c_str());
        Value* res = block->addInstruction(num, empty, writeToken, ins, valIndex);
        // printf("output res name: %s\n", res->name.c_str());
        return res;
    }
    else if(name == "OutputNewLine") {
        Value* res = block->addInstruction(empty, empty, writeNLToken, ins, valIndex);
        return res;
    }
    else {
        if(call->args == NULL) {
            std::vector<Value*> tmp;
            Value* func = new Value();
            Value* res = block->addCallInstruction(name, tmp, func, ins, valIndex);
            return res;
        }
        else {
            std::vector<Value*> args = codegen(call->args, block);
            Value* func = new Value();
            // printf("call name: %s\n", name.c_str());
            Value* res = block->addCallInstruction(name, args, func, ins, valIndex);
            // printf("call res name: %s\n", res->name.c_str());
            return res;
        }
    }
}

BasicBlock* codegen(struct brhAST* branch, Function& func, BasicBlock* block) {
    codegen(branch->cond, block, func.blocks.size());
    // printf("rel finished\n");
    BasicBlock* left = new BasicBlock();
    func.addBasicBlock(left);
    BasicBlock* left_end =  codegen(branch->br1, func, left);
    BasicBlock* right = NULL;
    BasicBlock* right_end = NULL;
    // printf("br1 finished\n");
    // BasicBlock* left = func.blocks[func.blocks.size()-1];
    block->successors.push_back(left->index);
    left->predecessors.push_back(block->index);
    if(branch->br2) {
        // printf("generating right...\n");
        right = new BasicBlock();
        func.addBasicBlock(right);
        block->successors.push_back(right->index);
        right->predecessors.push_back(block->index);
        right_end = codegen(branch->br2, func, right);
        // BasicBlock* right = func.blocks[func.blocks.size()-1];
    }
    BasicBlock* join = new BasicBlock();
    func.addBasicBlock(join);
    if(right) {
        auto last_left = (left_end->instructions[left_end->instructions.size()-1])->opcode;
        if(last_left != OpCode::RET) {
            left_end->successors.push_back(join->index);
            join->predecessors.push_back(left_end->index);
        }

        auto last_right = (left_end->instructions[left_end->instructions.size()-1])->opcode;
        if(last_right != OpCode::RET) {
            right_end->successors.push_back(join->index);
            join->predecessors.push_back(right->index);
        }
    }
    else {
        block->successors.push_back(join->index);
        join->predecessors.push_back(block->index);

        auto last_left = (left_end->instructions[left_end->instructions.size()-1])->opcode;
        if(last_left != OpCode::RET) {
            left_end->successors.push_back(join->index);
            join->predecessors.push_back(left_end->index);
        }
    }
    return join;
}

BasicBlock* codegen(struct loopAST* loop, Function& func, BasicBlock* block) {
    if(loop->type == 0) {
        BasicBlock* condBlock = new BasicBlock();
        func.addBasicBlock(condBlock);
        block->successors.push_back(condBlock->index);
        condBlock->predecessors.push_back(block->index);
        codegen(loop->cond, condBlock, func.blocks.size());

        BasicBlock* bodyStartBlock = new BasicBlock();
        func.addBasicBlock(bodyStartBlock);
        condBlock->successors.push_back(bodyStartBlock->index);
        bodyStartBlock->predecessors.push_back(condBlock->index);
        BasicBlock* bodyEndBlock = codegen(loop->body, func, bodyStartBlock);
        bodyEndBlock->successors.push_back(condBlock->index);
        condBlock->predecessors.push_back(bodyEndBlock->index);

        Value* v1 = constValue(condBlock->index);
        Value* v2 = emptyValue();
        Instruction* ins = new Instruction();
        bodyEndBlock->addInstruction(v1, v2, jmpToken, ins, valIndex);
        
        BasicBlock* join = new BasicBlock();
        func.addBasicBlock(join);
        condBlock->successors.push_back(join->index);
        join->predecessors.push_back(condBlock->index);
        return join;
    }
    else {
        BasicBlock* bodyStartBlock = new BasicBlock();
        func.addBasicBlock(bodyStartBlock);
        block->successors.push_back(bodyStartBlock->index);
        bodyStartBlock->predecessors.push_back(block->index);
        BasicBlock* bodyEndBlock = codegen(loop->body, func, bodyStartBlock);

        BasicBlock* condBlock = new BasicBlock();
        func.addBasicBlock(condBlock);
        condBlock->predecessors.push_back(bodyEndBlock->index);
        bodyEndBlock->successors.push_back(condBlock->index);
        condBlock->successors.push_back(bodyStartBlock->index);
        bodyStartBlock->predecessors.push_back(condBlock->index);
        codegen(loop->cond, condBlock, func.blocks.size());

        BasicBlock* join = new BasicBlock();
        func.addBasicBlock(join);
        condBlock->successors.push_back(join->index);
        join->predecessors.push_back(condBlock->index);
        return join;
    }
}

Value* codegen(struct retAST* _return, BasicBlock* block) {
    Value* v1 = emptyValue();
    Value* v2 = emptyValue();
    if(_return->expr)
        v1 = codegen(_return->expr, block);
    Instruction* ins = new Instruction();
    block->addInstruction(v1, v2, returnToken, ins, valIndex);
}

void codegen(struct varDeclAST* vars, Module& mod, bool is_global=false) {
    // printf("processing vardecl\n");
    struct varDeclAST* cur = vars;
    while(cur) {
        string name(cur->name);
        if(find(mod.varNames.begin(), mod.varNames.end(), name) == mod.varNames.end())
            mod.varNames.push_back(name);
        if(is_global) {
            if(find(mod.globalNames.begin(), mod.globalNames.end(), name) == mod.globalNames.end())
                mod.globalNames.push_back(name);
        }
        printf("adding var: %s\n", name.c_str());
        int value = -1;
        glob.symbolTable.insertSymbol(name, value);
        cur = cur->next;
    }
}

BasicBlock* codegen(struct stmtSeqAST* stmts, Function& func, BasicBlock* block) {
    struct stmtSeqAST* cur = stmts;
    BasicBlock* curBlock = NULL;
    if(!block){
        curBlock = new BasicBlock();
        func.addBasicBlock(curBlock);
    }
    else
        curBlock = block;
    
    // printf("block added\n");
    while(cur) {
        switch(cur->stat->type) {
            case 0: { // assign'
                // printf("handling assign...\n");
                Value* res = codegen((struct assignAST*)(cur->stat->data), curBlock);
                // printf("assign finished: %s\n", res->name.c_str());
                break;
            }
            case 1: { // funccall
                // printf("handling funccall...\n");
                Value* res = codegen((struct funcCallAST*)(cur->stat->data), curBlock);
                // printf("call finished, %s\n", curBlock->instructions.back()->dest->name.c_str());
                break;
            }
            case 2: { // branch
                // printf("handling brh...\n");
                curBlock = codegen((struct brhAST*)(cur->stat->data), func, curBlock);
                // BasicBlock* joinBlock = new BasicBlock();
                // func.addBasicBlock(joinBlock);
                // if(((struct brhAST*)(cur->stat->data))->br2) {
                //     BasicBlock* left = func.blocks[func.blocks.size()-3];
                //     BasicBlock* right = func.blocks[func.blocks.size()-2];
                //     left->successors.push_back(joinBlock->index);
                //     right->successors.push_back(joinBlock->index);
                //     joinBlock->predecessors.push_back(left->index);
                //     joinBlock->predecessors.push_back(right->index);
                // }
                // else {
                //     BasicBlock* left = func.blocks[func.blocks.size()-3];
                //     left->successors.push_back(joinBlock->index);
                //     joinBlock->predecessors.push_back(left->index);
                // }
                // curBlock = joinBlock;
                break;
            }
            case 3: { // while
                // printf("handling while\n");
                curBlock = codegen((struct loopAST*)(cur->stat->data), func, curBlock);
                break;
            }
            case 4: { // repeat
                // printf("handling repeat\n");
                curBlock = codegen((struct loopAST*)(cur->stat->data), func, curBlock);
                break;
            }
            case 5: {
                // printf("hendling return\n");
                codegen((struct retAST*)(cur->stat->data), curBlock);
                break;
            }
            default: {
                break;
            }
        }
        cur = cur->next;
        // if(cur == NULL) {
        //     printf("finished\n");
        // }
    }
    // printf("block len: %d\n", func.blocks.size());
    // if(is_func) {
    //     Value* empty = emptyValue();
    //     Instruction* ins = new Instruction();
    //     curBlock->addInstruction(empty, empty, returnToken, ins);
    // }
    return curBlock;
}

void codegen(struct funcAST* func, Module& mod) {
    printf("codegen %s\n", func->name);
    Function& res = mod.addFunction(func->name);
    glob.symbolTable.newScope();
    if(func->params)
        codegen(func->params, mod);
    if(func->decls)
        codegen(func->decls, mod);
    // for(auto s: glob.symbolTable.table.back()) {
    //     printf("sym: %s %d\n", s.first.c_str(), s.second);//glob.values[s.second]->name.c_str());
    // }
    if(func->stmts)
        codegen(func->stmts, res);
    glob.symbolTable.exitScope();
    // dump2txt(mod);
}

Module codegen(struct computationAST* comp) {
    Module res;

    // add global var decls
    glob.symbolTable.newScope();
    codegen(comp->vars, res, true);
    // printf("type decl success.\n");
    // add func decl handling
    auto funcPtr = comp->funcs;
    while(funcPtr != NULL) {
        codegen(funcPtr->func, res);
        funcPtr = funcPtr->next;
    }

    // handling main function
    // string main = "main";
    // Function mainFunc = Function(&res, main);
    Function& mainFunc = res.addFunction("main");
    codegen(comp->stats, mainFunc);

    // for(auto f: res.funcs)
    //     printf("func: %s\n", f.name.c_str());
    printf("codegen success, dumping...\n");
    
    return res;
}