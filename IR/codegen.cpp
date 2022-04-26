
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../frontend/scanner.h"
#include "IR.h"
#include "codegen.h"

using namespace std;

int valIndex = 0;
Global glob;

Value* codegen(struct desiAST* desi, BasicBlock* block, bool is_left=false) {
    if(desi->type == 0) {
        string name(desi->id);
        // int index = ((Module*)(((Function*)(block->func))->module))->symbolTable.lookupSymbol(name);
        // if(block->glob == NULL) {
        //     printf("error.");
        // }
        // printf("skipped\n");
        if(is_left) {
            Value* res = new Value();
            res->value = -1;
            res->type = Type::def;
            res->name = name;
            return res;
        }
        int index = glob.symbolTable.lookupSymbol(name);
        printf("index of var %s is %d\n", name.c_str(), index);
        if (index == -2) {
            printf("error looking up symbol in desi\n");
            exit(-1);
        }
        if (index == -1) {
            Value* res = new Value();
            res->type = Type::def;
            res->name = name;
            return res;
        }
        glob.values[index]->name = name;
        return glob.values[index];
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
            return codegen((struct funcCallAST*)(factor->data), block);
        }
        default: {
            printf("unreachable in factor\n");
            exit(-1);
        }
    }
}

Value* codegen(struct termAST* term, BasicBlock* block) {
    Value* cur = codegen(term->factor, block);
    termTailAST* curAST = term->next;
    while(curAST) {
        Value* extra = codegen(curAST->factor, block);
        Instruction* ins = new Instruction();
        if(curAST->op == Token::timesToken)
            cur = block->addInstruction(cur, extra, timesToken, glob, ins);
        else if(curAST->op == Token::divToken)
            cur = block->addInstruction(cur, extra, divToken, glob, ins);
        curAST = curAST->next;
    }
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
            cur = block->addInstruction(cur, extra, plusToken, glob, ins);
        else if(curAST->op == Token::minusToken)
            cur = block->addInstruction(cur, extra, minusToken, glob, ins);
        curAST = curAST->next;
    }
    return cur;
}

Value* codegen(struct assignAST* assign, BasicBlock* block) {
    Value* lhs = codegen(assign->lhs, block, true);
    switch(assign->op) {
        case increaseToken: {
            Value* one = constValue(1);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, one, Token::plusToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case decreaseToken: {
            Value* one = constValue(1);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, one, Token::minusToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selfplusToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, rhs, Token::plusToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selfminusToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, rhs, Token::minusToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, tmp->index);
            return res;
            break;
        }
        case selftimesToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, rhs, Token::timesToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            return res;
            break;
        }
        case selfdivToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* tmp_ins = new Instruction();
            Instruction* ins = new Instruction();
            Value* tmp = block->addInstruction(lhs, rhs, Token::divToken, glob, tmp_ins);
            Value* res = block->addInstruction(tmp, lhs, Token::assignToken, glob, ins);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            return res;
            break;
        }
        case assignToken: {
            Value* rhs = codegen(assign->rhs, block);
            Instruction* ins = new Instruction();
            Value* res = block->addInstruction(rhs, lhs, Token::assignToken, glob, ins);
            // printf("%s %d\n", lhs.name.c_str(), rhs.index);
            glob.symbolTable.insertSymbol(lhs->name, rhs->index);
            int index = glob.symbolTable.lookupSymbol(lhs->name);
            // printf("after assign index of var %s is %d\n", lhs->name.c_str(), index);
            // printf("blk len after assign: %d\n", block->instructions.size());
            // for(auto k: glob.symbolTable.table.back()) {
            //     printf("%s, %d\n", k.first.c_str(), k.second);
            // }
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
    // printf("rel lhs finished\n");
    Value* rhs = codegen(rel->rhs, block);
    // printf("rel rhs finished\n");
    Instruction* cmp = new Instruction();
    Value* tmp = block->addInstruction(lhs, rhs, cmpToken, glob, cmp);
    // printf("cmp finished%d\n", rel->op);
    Value* empty = constValue(jmpIndex);
    switch(rel->op) {
        case eqlToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, eqlToken, glob, res);
        }
        case neqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, neqToken, glob, res);
        }
        case lssToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, lssToken, glob, res);
        }
        case gtrToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, gtrToken, glob, res);
        }
        case leqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, leqToken, glob, res);
        }
        case geqToken: {
            Instruction* res = new Instruction();
            return block->addInstruction(tmp, empty, geqToken, glob, res);
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
        Value* res = block->addInstruction(empty, empty, readToken, glob, ins);
        return res;
    }
    else if(name == "OutputNum") {
        Value* num = codegen(call->args, block)[0];
        Value* res = block->addInstruction(num, empty, writeToken, glob, ins);
        return res;
    }
    else if(name == "OutputNewLine") {
        Value* res = block->addInstruction(empty, empty, writeNLToken, glob, ins);
        return res;
    }
    else {
        if(call->args == NULL) {
            std::vector<Value*> tmp;
            Value* func = new Value();
            Value* res = block->addCallInstruction(name, tmp, func, glob, ins);
            return res;
        }
        else {
            std::vector<Value*> args = codegen(call->args, block);
            Value* func = new Value();
            Value* res = block->addCallInstruction(name, args, func, glob, ins);
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
    left_end->successors.push_back(join->index);
    join->predecessors.push_back(left_end->index);
    if(right) {
        right_end->successors.push_back(join->index);
        join->predecessors.push_back(right->index);
    }
    else {
        block->successors.push_back(join->index);
        join->predecessors.push_back(block->index);
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
        bodyEndBlock->addInstruction(v1, v2, jmpToken, glob, ins);
        
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
    block->addInstruction(v1, v2, returnToken, glob, ins);
}

void codegen(struct varDeclAST* vars, Module& mod) {
    // printf("processing vardecl\n");
    struct varDeclAST* cur = vars;
    while(cur) {
        string name(cur->name);
        if(find(mod.varNames.begin(), mod.varNames.end(), name) == mod.varNames.end())
            mod.varNames.push_back(name);
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
    
    BasicBlock* initBlock = curBlock;
    // printf("block added\n");
    while(cur) {
        switch(cur->stat->type) {
            case 0: { // assign'
                printf("handling assign...\n");
                codegen((struct assignAST*)(cur->stat->data), curBlock);
                break;
            }
            case 1: { // funccall
                printf("handling funccall...\n");
                codegen((struct funcCallAST*)(cur->stat->data), curBlock);
                printf("call finished\n");
                break;
            }
            case 2: { // branch
                printf("handling brh...\n");
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
                printf("handling while\n");
                curBlock = codegen((struct loopAST*)(cur->stat->data), func, curBlock);
                break;
            }
            case 4: { // repeat
                printf("handling repeat\n");
                curBlock = codegen((struct loopAST*)(cur->stat->data), func, curBlock);
                break;
            }
            case 5: {
                printf("hendling return\n");
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
    //     curBlock->addInstruction(empty, empty, returnToken, glob, ins);
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
    if(func->stmts)
        codegen(func->stmts, res);
}

Module codegen(struct computationAST* comp) {
    Module res;

    // add global var decls
    glob.symbolTable.newScope();
    codegen(comp->vars, res);
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

    printf("codegen success, dumping...\n");
    
    return res;
}