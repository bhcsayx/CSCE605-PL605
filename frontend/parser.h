#include <stdio.h>
#include "scanner.h"

// enum parseTreeNodeType {
//     computation,
//     varDecls,
//     funcDecls,
//     typeDecl,
//     statSequence,
//     statement,
//     assignment,
//     funcCall,
//     expression,
//     term,
//     factor,
//     designator,
//     shortHandOp,
//     uniOp,
//     num,
//     id,
//     token
// };

// struct parseTreeNode {
//     enum parseTreeNodeType type;
//     int* numData;
//     char** nameData;
//     int numDataLen;
//     int nameDataLen;
//     int childNum;
//     struct parseTreeNode **children;
// };

struct computationAST {
    varDeclAST* vars;
    funcAST* funcs;
    statAST* stats;
    int varLen;
    int funcsLen;
    int statsLen;
};

enum Token get(struct tokenStream stream, int *cursor) {
    return stream.tokens[*cursor];
}

void expect(struct tokenStream stream, int *cursor, enum Token expected) {
    if(stream.tokens[cursor] != expected) {
        printf("parser error: expected %d", expected);
        exit(-1);
    }

    *(cursor) += 1;

    return;
}

struct parseTree parse(struct tokenStream stream);