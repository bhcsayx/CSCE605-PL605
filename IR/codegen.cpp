
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../frontend/scanner.h"
#include "IR.h"
#include "codegen.h"

using namespace std;

int valIndex = 0;
Global glob;

Value codegen(struct desiAST* desi, BasicBlock& block, bool is_left=false) {
    if(desi->type == 0) {
        string name(desi->id);
        // int index = ((Module*)(((Function*)(block->func))->module))->symbolTable.lookupSymbol(name);
        // if(block->glob == NULL) {
        //     printf("error.");
        // }
        // printf("skipped\n");
        if(is_left) {
            Value res;
            res.value = -1;
            res.type = Type::def;
            res.name = name;
            return res;
        }
        int index = glob.symbolTable.lookupSymbol(name);
        // printf("index of var %s is %d\n", name.c_str(), index);
        if (index == -2) {
            printf("error looking up symbol in desi\n");
            exit(-1);
        }
        if (index == -1) {
            Value res;
            res.type = Type::def;
            res.name = name;
            return res;
        }
        return glob.values[index];
    }
}

Value codegen(struct factorAST* factor, BasicBlock& block) {
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

Value codegen(struct termAST* term, BasicBlock& block) {
    Value cur = codegen(term->factor, block);
    termTailAST* curAST = term->next;
    while(curAST) {
        Value extra = codegen(curAST->factor, block);
        if(curAST->op == Token::timesToken)
            cur = block.addInstruction(cur, extra, timesToken, glob);
        else if(curAST->op == Token::divToken)
            cur = block.addInstruction(cur, extra, divToken, glob);
        curAST = curAST->next;
    }
    return cur;
}

Value codegen(struct exprAST* expr, BasicBlock& block) {
    Value cur = codegen(expr->term, block);
    exprTailAST* curAST = expr->next;
    // printf("%d\n", curAST);
    while(curAST) {
        // printf("op: %d\n", curAST->op);
        Value extra = codegen(curAST->term, block);
        if(curAST->op == Token::plusToken)
            cur = block.addInstruction(cur, extra, plusToken, glob);
        else if(curAST->op == Token::minusToken)
            cur = block.addInstruction(cur, extra, minusToken, glob);
        curAST = curAST->next;
    }
    return cur;
}

Value codegen(struct assignAST* assign, BasicBlock& block) {
    Value lhs = codegen(assign->lhs, block, true);
    switch(assign->op) {
        case increaseToken: {
            Value one = constValue(1);
            Value tmp = block.addInstruction(lhs, one, Token::plusToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, tmp.index);
            return res;
            break;
        }
        case decreaseToken: {
            Value one = constValue(1);
            Value tmp = block.addInstruction(lhs, one, Token::minusToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, tmp.index);
            return res;
            break;
        }
        case selfplusToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block.addInstruction(lhs, rhs, Token::plusToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, tmp.index);
            return res;
            break;
        }
        case selfminusToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block.addInstruction(lhs, rhs, Token::minusToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, tmp.index);
            return res;
            break;
        }
        case selftimesToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block.addInstruction(lhs, rhs, Token::timesToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, rhs.index);
            return res;
            break;
        }
        case selfdivToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block.addInstruction(lhs, rhs, Token::divToken, glob);
            Value res = block.addInstruction(tmp, lhs, Token::assignToken, glob);
            glob.symbolTable.insertSymbol(lhs.name, rhs.index);
            return res;
            break;
        }
        case assignToken: {
            Value rhs = codegen(assign->rhs, block);
            Value res = block.addInstruction(rhs, lhs, Token::assignToken, glob);
            // printf("%s %d\n", lhs.name.c_str(), rhs.index);
            glob.symbolTable.insertSymbol(lhs.name, rhs.index);
            int index = glob.symbolTable.lookupSymbol(lhs.name);
            // printf("after assign index of var %s is %d\n", lhs.name.c_str(), index);
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

std::vector<Value> codegen(struct exprListAST* list, BasicBlock& block) {
    std::vector<Value> res;
    struct exprListAST* cur = list;
    while(cur) {
        Value v = codegen(cur->head, block);
        res.push_back(v);
        cur = cur->next;
    }
    return res;
}

Value codegen(struct funcCallAST* call, BasicBlock& block) {
    // printf("handling call\n");
    string name(call->funcName);
    Value empty = emptyValue();
    if(name == "InputNum") {
        Value res = block.addInstruction(empty, empty, readToken, glob);
        return res;
    }
    else if(name == "OutputNum") {
        Value num = codegen(call->args, block)[0];
        Value res = block.addInstruction(num, empty, writeToken, glob);
        return res;
    }
    else if(name == "OutputNL") {
        Value res = block.addInstruction(empty, empty, writeNLToken, glob);
        return res;
    }
    else {
        if(call->args == NULL) {
            std::vector<Value> tmp;
            Value res = block.addCallInstruction(name, tmp, glob);
            return res;
        }
        else {
            std::vector<Value> args = codegen(call->args, block);
            Value res = block.addCallInstruction(name, args, glob);
            return res;
        }
    }
}

void codegen(struct varDeclAST* vars, Module mod) {
    // printf("processing vardecl\n");
    glob.symbolTable.newScope();
    struct varDeclAST* cur = vars;
    while(cur) {
        string name(cur->name);
        // printf("adding var: %s\n", name.c_str());
        int value = -1;
        glob.symbolTable.insertSymbol(name, value);
        cur = cur->next;
    }
}

void codegen(struct stmtSeqAST* stmts, Function& func, bool is_func=false) {
    struct stmtSeqAST* cur = stmts;
    BasicBlock& curBlock = func.addBasicBlock();
    // printf("block added\n");
    while(cur) {
        switch(cur->stat->type) {
            case 0: { // assign'
                // printf("handling assign...%x\n", cur->stat->data);
                codegen((struct assignAST*)(cur->stat->data), curBlock);
                break;
            }
            case 1: { // funccall
                // printf("handling funccall...\n");
                codegen((struct funcCallAST*)(cur->stat->data), curBlock);
                break;
            }
            default: {
                break;
            }
        }
        cur = cur->next;
    }
    if(is_func) {
        Value empty = emptyValue();
        curBlock.addInstruction(empty, empty, returnToken, glob);
    }
}

Module codegen(struct computationAST* comp) {
    Module res;

    // add global var decls
    codegen(comp->vars, res);
    // printf("type decl success.\n");
    // TODO: add func decl handling

    // handling main function
    // string main = "main";
    // Function mainFunc = Function(&res, main);
    Function& mainFunc = res.addFunction("main");
    codegen(comp->stats, mainFunc, true);

    printf("codegen success, dumping...\n");
    
    return res;
}