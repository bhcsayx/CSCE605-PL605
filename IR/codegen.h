#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IR.h"

struct Module* codegen(struct computationAST comp);
void codegen(struct varDeclAST* vars, struct Module* _module);
void codegen(struct stmtSeqAST* stmts, struct Function* func);
void codegen(struct assignAST* assign, struct BasicBlock* block);
void codegen(struct funcCallAST* call, struct BasicBlock* block);

#endif