
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "IR.h"
#include "codegen.h"

using namespace std;

int valIndex = 0;

Value codegen(struct desiAST* desi, BasicBlock* block) {
    if(desi->type == 0) {
        string name(desi->id);
        int index = block->func->module->symbolTable.lookupSymbol(name);
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
        return block->func->module->values[index];
    }
}

Value codegen(struct factorAST* factor, BasicBlock* block) {
    switch(factor->type) {
        case 0: {
            return codegen((struct desiAST*)(factor->data), block);
            break;
        }
        case 1: {
            int value = *(int*)(factor->data);
            return addValue(value);
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

Value codegen(struct termAST* term, BasicBlock* block) {
    Value cur = codegen(term->factor, block);
    termTailAST* curAST = term->next;
    while(curAST) {
        Value extra = codegen(curAST->factor, block);
        if(curAST->op == Token::timesToken)
            cur = block->addInstruction(cur, extra, timesToken);
        else if(curAST->op == Token::divToken)
            cur = block->addInstruction(cur, extra, divToken);
        curAST = curAST->next;
    }
    return cur;
}

Value codegen(struct exprAST* expr, BasicBlock* block) {
    Value cur = codegen(expr->term, block);
    exprTailAST* curAST = expr->next;
    while(curAST) {
        Value extra = codegen(curAST->term, block);
        if(curAST->op == Token::plusToken)
            cur = block->addInstruction(cur, extra, plusToken);
        else if(curAST->op == Token::minusToken)
            cur = block->addInstruction(cur, extra, minusToken);
        curAST = curAST->next;
    }
    return cur;
}

Value codegen(struct assignAST* assign, BasicBlock* block) {
    Value lhs = codegen(assign->lhs, block);
    switch(assign->op) {
        case increaseToken: {
            Value one = newConstValue(1);
            Value tmp = block->addInstruction(lhs, one, Token::plusToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case decreaseToken: {
            Value one = newConstValue(1);
            Value tmp = block->addInstruction(lhs, one, Token::minusToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case selfplusToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block->addInstruction(lhs, rhs, Token::plusToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case selfminusToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block->addInstruction(lhs, rhs, Token::minusToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case selftimesToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block->addInstruction(lhs, rhs, Token::timesToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case selfdivToken: {
            Value rhs = codegen(assign->rhs, block);
            Value tmp = block->addInstruction(lhs, rhs, Token::divToken);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        case assignToken: {
            Value rhs = codegen(assign->rhs, block);
            return block->addInstruction(lhs, tmp, Token::assignToken);
            break;
        }
        default: {
            printf("error in assign code generation: %d\n", assign->op);
            exit(-1);
        }
    }
}

std::vector<Value> codegen(struct exprListAST* list) {
    std::vector<Value> res;
    struct exprListAST* cur = list;
    while(cur) {
        Value v = codegen(cur->head, block);
        res.push_back(v);
        cur = cur->next;
    }
    return res;
}

Value codegen(struct funcCallAST* call, BasicBlock* block) {
    string name(call->name);
    Value empty = addEmptyValue();
    if(name == "InputNum") {
        Value res = block->addInstruction(empty, empty, readToken);
        return res;
    }
    else if(name == "OutputNum") {
        Value num = codegen(call->args, block)[0];
        Value res = block->addInstruction(num, empty, writeToken);
        return res;
    }
    else if(name == "OutputNL") {
        Value res = block->addInstruction(num, empty, writeNLToken);
        return res;
    }
    else {
        if(call->args == NULL) {
            Value res = block->addInstruction(name, empty, callToken);
            return res;
        }
        else {
            std::vector<Value> args = codegen(call->args, block);
            Value res = block->addCallInstruction(name, args);
            return res;
        }
    }
}

void codegen(struct varDeclAST* vars, Module* _module) {
    _module->symbolTable.newScope();
    struct varDeclAST* cur = vars;
    while(cur) {
        string name(cur->name);
        int value = -1;
        _module->symbolTable.insertSymbol(name, value);
        cur = cur->next;
    }
}

void codegen(struct stmtSeqAST* stmts, struct Function* func) {
    struct stmtSeqAST* cur = stmts;
    int blkIndex = 0; BasicBlock curBlock(func, blkIndex);
    
    while(cur) {
        switch(cur->stat->type) {
            case 0: { // assign
                codegen((struct assignAST*)(cur->stat->data), curBlock);
                break;
            }
            case 1: { // funccall
                codegen((struct funcCallAST*)(cur->stat->data), curBlock);
                break;
            }
            default: {
                break;
            }
        }
        cur = cur->next;
    }
}

Module codegen(struct computationAST* comp) {
    Module res;

    // add global var decls
    codegen(comp->vars, &res);

    // TODO: add func decl handling

    // handling main function
    string main = "main";
    Function mainFunc = Function(&res, main);
    codegen(comp->stats, &mainFunc);
    
    return res;
}