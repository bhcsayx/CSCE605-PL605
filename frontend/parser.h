#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <stdio.h>
#include "scanner.h"

using namespace std;

struct typeDeclAST {
    char type; // 0 for var, 1 for array

    // reserved for arrays.
    // int dimNum; // number of dimensions;
    // int* dims; // dimensions
};

struct varDeclAST {
    struct typeDeclAST type;
    char* name;
    struct varDeclAST* next;
};

struct desiAST {
    char type; // 0 for normal var, 1 for array element access
    char* id;
    struct exprAST* expr;
};

struct funcCallAST {
    char* funcName;
    struct exprListAST* args;
};

struct factorAST {
    char type; // 0 for designator, 1 for number, 2 for parenthesized expr, 3 for funcCall
    void* data;
};

struct termAST {
    struct factorAST* factor;
    struct termTailAST* next;
};

struct termTailAST {
    enum Token op;
    struct factorAST* factor;
    struct termTailAST* next;
};

struct exprAST {
    struct termAST* term;
    struct exprTailAST* next;
};

struct relAST {
    struct exprAST* lhs;
    struct exprAST* rhs;
};

struct exprTailAST {
    enum Token op;
    struct termAST* term;
    struct exprTailAST* next;
};

struct exprListAST {
    struct exprAST* head;
    struct exprListAST* next;
};

struct assignAST {
    struct desiAST* lhs;
    enum Token op;
    struct exprAST* rhs;
};

struct stmtAST {
    char type; // 0 for assignment, 1 for funcCall, 2 for branch, 3 for while, 4 for repeat, 5 for return
    void* data;
};

struct stmtSeqAST {
    struct stmtAST* stat;
    struct stmtSeqAST* next;
};

struct computationAST {
    struct varDeclAST* vars;
    // struct funcAST* funcs;
    struct stmtSeqAST* stats;
};

enum Token get(struct tokenStream stream, int *cursor);
void expect(struct tokenStream stream, int *cursor, enum Token expected);
struct computationAST* parse(struct tokenStream stream);
struct typeDeclAST parseTypeDecl(struct tokenStream stream);
char* parseIdent(struct tokenStream stream);
char* parseNumber(struct tokenStream stream);
struct varDeclAST* parseVarDecls(struct tokenStream stream);
struct factorAST* parseFactor(struct tokenStream stream);
struct termAST* parseTerm(struct tokenStream stream);
struct exprAST* parseExpr(struct tokenStream stream);
struct relAST* parseRelation(struct tokenStream stream);
struct desiAST* parseDesi(struct tokenStream stream);
struct stmtAST* parseAssign(struct tokenStream stream);
struct stmtAST* parseCall(struct tokenStream stream);
struct stmtAST* parseStmt(struct tokenStream stream);
struct stmtSeqAST* parseStmtSequence(struct tokenStream stream);

#endif