#include <stdio.h>
#include "scanner.h"

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
};

struct funcCallAST {
    char* funcName;
    struct exprAST* args;
};

struct factorAST {
    char type; // 0 for designator, 1 for number, 2 for parenthesized expr, 3 for funcCall
    char* data;
};

struct termAST {
    struct factorAST factor;
    enum Token op;
    struct factorAST* next;
};

struct exprAST {
    struct termAST term;
    enum Token op;
    struct exprAST* next;
};

struct assignAST {
    struct desiAST lhs;
    enum Token op;
    struct exprAST rhs;
};

struct statAST {
    char type; // 0 for assignment, 1 for funcCall, 2 for branch, 3 for while, 4 for repeat, 5 for return
    char* data;
};

struct statSeqAST {
    struct statAST stat;
    struct statSeqAST* next;
};

struct computationAST {
    struct varDeclAST* vars;
    // struct funcAST* funcs;
    struct statAST* stats;
};

enum Token get(struct tokenStream stream, int *cursor) {
    return stream.tokens[*cursor];
}

void expect(struct tokenStream stream, int *cursor, enum Token expected) {
    if(stream.tokens[*cursor] != expected) {
        printf("parser error: expected %d", expected);
        exit(-1);
    }

    *(cursor) += 1;

    return;
}

struct parseTree parse(struct tokenStream stream);