#include <stdio.h>
#include "parser.h"

int parseCursor = 0;

// struct parseTreeNode* parseVarDecls(tokenStream stream) {

//     struct parseTreeNode* vars = malloc(sizeof(struct parseTreeNode));
//     vars.type = varDecls;
//     vars.childNum = 0;
//     vars.children = 

//     return NULL;
// }

// struct parseTreeNode* parseFuncDecls(tokenStream stream) {
//     return NULL;
// }

// struct parseTreeNode* parse(tokenStream stream) {

//     struct parseTreeNode* root = malloc(sizeof(struct parseTreeNode));
//     root.type = computation;
//     root.childNum = 0;
//     root.children = malloc(sizeof(struct parseTreeNode*) * 3);
    
//     expect(stream, &parseCursor, mainToken);
//     enum Token next = get(stream, &parseCursor);
//     if(next == varToken || next == arrayToken) {
//         struct parseTreeNode* vars = parseVarDecls(stream);
//         root.children[root.childNum++] = vars;
//     }
    
//     // next = get(stream, &parseCursor);
//     // if(next == voidToken || next = funcToken) {
//     //     struct parseTreeNode* funcs = parseFuncDecls(stream);
//     //     root.children[root.childNum++] = funcs;
//     // }

//     expect(stream, &parseCursor, beginToken);
//     struct stats = parseStatSequence(stream);
//     root.children[root.childNum++] = stats;

//     expect(stream, &parseCursor, endToken);
//     expect(stream, &parseCursor, periodToken);
//     expect(stream, &parseCursor, eofToken);
//     printf("parse success.");

//     return root;
// }



struct computationAST* parse(tokenStream stream) {

    struct computationAST* root = (struct computationAST*) malloc(sizeof(computationAST));
    root.varLen = root.funcsLen = root.statsLen = 0;

    expect(stream, &parseCursor, mainToken);
    enum Token next = get(stream, &parseCursor);
    if(next == varToken || next == arrayToken) {
        struct varDeclAST* vars = parseVarDecls(stream);
        root.vars = vars;
    }

    // next = get(stream, &parseCursor);
    // if(next == voidToken || next = funcToken) {
    //     struct funcAST* funcs = parseFuncDecls(stream);
    //     root.funcs = funcs;
    // }

    expect(stream, &parseCursor, beginToken);
    struct statAST* stats = parseStatSequence(stream);

    expect(stream, &parseCursor, endToken);
    expect(stream, &parseCursor, periodToken);
    expect(stream, &parseCursor, eofToken);
    printf("parse success.");

    return root;
}