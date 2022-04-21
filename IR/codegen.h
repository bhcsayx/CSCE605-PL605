#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IR.h"

Module codegen(struct computationAST* comp);
void codegen(struct varDeclAST* vars, Module mod);
void codegen(struct stmtSeqAST* stmts, Function& func, bool is_func=false);
Value codegen(struct desiAST* desi, BasicBlock& block, bool is_left=false);
Value codegen(struct factorAST* factor, BasicBlock& block);
Value codegen(struct termAST* term, BasicBlock& block);
Value codegen(struct exprAST* expr, BasicBlock& block);
Value codegen(struct assignAST* assign, BasicBlock& block);
Value codegen(struct funcCallAST* call, BasicBlock& block);
std::vector<Value> codegen(struct exprListAST* list, BasicBlock& block);

#endif