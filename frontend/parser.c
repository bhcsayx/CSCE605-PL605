#include <stdio.h>
#include "parser.h"

int parseCursor = 0;
int idCursor = 0;

struct typeDeclAST parseTypeDecl(struct tokenStream stream) {

    struct typeDeclAST type;
    if(get(stream, &parseCursor) == varToken)
        type.type = 0;
    else if(get(stream, &parseCursor) == arrayToken) {
        type.type = 1;
    }
    return type;

}

char* parseIdent(struct tokenStream) {
    expect(tokenStream, &parseCursor, ident);
    char* dest = (char*) malloc(strlen(tokenStream.ids[idCursor]));
    strncpy(dest, tokenStream.ids[idCursor], strlen(tokenStream.ids[idCursor++]));
    return dest;
}

struct varDeclAST* parseVarDecls(struct tokenStream stream) {

    struct varDeclAST* vars = (struct varDeclAST*) malloc(sizeof(varDeclAST));
    vars->type = parseTypeDecl(stream);
    vars->name = parseIdent(stream);
    vars->next = NULL;
    struct varDeclAST* cur = vars;
    while(get(stream, &parseCursor) == commaToken) {
        expect(stream, &parseCursor, commaToken);
        cur->next = (struct varDeclAST*) malloc(sizeof(varDeclAST));
        cur->next->type = vars->type;
        cur->next->name = parseIdent(stream);
        cur->next->next= NULL;
        cur = cur->next;
    }
    expect(stream, &parseCursor, semiToken);

    if(get(stream, &parseCursor) == varToken || get(stream, &parseCursor) == arrayToken)
        cur->next = parseVarDecls(stream);

    return vars;
}

struct computationAST* parse(tokenStream stream) {

    struct computationAST* root = (struct computationAST*) malloc(sizeof(computationAST));

    expect(stream, &parseCursor, mainToken);
    enum Token next = get(stream, &parseCursor);
    if(next == varToken || next == arrayToken) {
        struct varDeclAST* vars = parseVarDecls(stream);
        root->vars = vars;
    }

    // next = get(stream, &parseCursor);
    // if(next == voidToken || next = funcToken) {
    //     struct funcAST* funcs = parseFuncDecls(stream);
    //     root.funcs = funcs;
    // }

    expect(stream, &parseCursor, beginToken);
    root->stats = parseStatSequence(stream);

    expect(stream, &parseCursor, endToken);
    expect(stream, &parseCursor, periodToken);
    expect(stream, &parseCursor, eofToken);
    printf("parse success.");

    return root;
}