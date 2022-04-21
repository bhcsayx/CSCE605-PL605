#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int parseCursor = 0;
int idCursor = 0;
int numCursor = 0;

enum Token get(struct tokenStream stream, int *cursor) {
    return stream.tokens[*cursor];
}

void expect(struct tokenStream stream, int *cursor, enum Token expected) {
    if(stream.tokens[*cursor] != expected) {
        printf("parser error: expected %d\n", expected);
        exit(-1);
    }

    *(cursor) += 1;

    return;
}

struct typeDeclAST parseTypeDecl(struct tokenStream stream) {

    struct typeDeclAST type;
    if(get(stream, &parseCursor) == varToken) {
        expect(stream, &parseCursor, varToken);
        type.type = 0;
    }
    else if(get(stream, &parseCursor) == arrayToken) {
        expect(stream, &parseCursor, arrayToken);
        type.type = 1;
    }
    return type;

}

char* parseIdent(struct tokenStream stream) {
    expect(stream, &parseCursor, ident);
    char* dest = (char*) malloc(strlen(stream.ids[idCursor]));
    strncpy(dest, stream.ids[idCursor], strlen(stream.ids[idCursor]));
    idCursor++;
    // printf("ident: %s\n", dest);
    return dest;
}

char* parseNumber(struct tokenStream stream) {
    expect(stream, &parseCursor, number);
    const char* dest = (const char*) malloc(sizeof(int));
    strncpy(dest, (const char*)&(stream.numbers[numCursor]), sizeof(int));
    numCursor++;
    // printf("num: %d\n", *(int*)dest);
    return dest;
}

struct varDeclAST* parseVarDecls(struct tokenStream stream) {

    struct varDeclAST* vars = (struct varDeclAST*) malloc(sizeof(struct varDeclAST));
    vars->type = parseTypeDecl(stream);
    vars->name = parseIdent(stream);
    vars->next = NULL;
    struct varDeclAST* cur = vars;
    while(get(stream, &parseCursor) == commaToken) {
        expect(stream, &parseCursor, commaToken);
        cur->next = (struct varDeclAST*) malloc(sizeof(struct varDeclAST));
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

struct factorAST* parseFactor(struct tokenStream stream) {
    struct factorAST* res = (struct factorAST*) malloc(sizeof(struct factorAST));
    // printf("parsing factor...\n");
    enum Token next = get(stream, &parseCursor);
    // printf("next: %d\n", next);
    switch(next) {
        case ident: {
            res->type = 0;
            res->data = parseDesi(stream);
            break;
        }

        case number: {
            res->type = 1;
            res->data = parseNumber(stream);
            break;
        }

        case openparenToken: {
            res->type = 2;
            expect(stream, &parseCursor, openparenToken);
            struct exprAST* tmp = parseExpr(stream);
            res->data = tmp;
            expect(stream, &parseCursor, closeparenToken);
            break;
        }

        case callToken: {
            res->type = 3;
            res->data = parseCall(stream);
            break;
        }

        default: {
            printf("error parsing designator\n");
            exit(-1);
            break;
        }
    }

    return res;
}

struct termAST* parseTerm(struct tokenStream stream) {
    struct termAST* res = (struct termAST*) malloc(sizeof(struct termAST));
    // printf("parsing term...\n");
    res->factor = parseFactor(stream);
    enum Token op = get(stream, &parseCursor);
    if((op != timesToken) && (op != divToken)) {
        res->next = NULL;
    }
    else {
        res->next = (struct termTailAST*) malloc(sizeof(struct termTailAST));
        struct termTailAST* cur = res->next;
        enum Token next = get(stream, &parseCursor);
        while(next == timesToken || next == divToken) {
            if(next == timesToken)
                expect(stream, &parseCursor, timesToken);
            else
                expect(stream, &parseCursor, divToken);
            cur->op = next;
            cur->factor = parseFactor(stream);
            cur = cur->next;
            cur = (struct termTailAST*) malloc(sizeof(struct termTailAST));
            next = get(stream, &parseCursor);
        }
        cur = NULL;
    }

    return res;
}

struct exprAST* parseExpr(struct tokenStream stream) {
    struct exprAST* res = (struct exprAST*) malloc(sizeof(struct exprAST));
    // printf("parsing expr...\n");
    res->term = parseTerm(stream);
    // printf("1st term correct\n");
    enum Token op = get(stream, &parseCursor);
    if((op != plusToken) && (op != minusToken)) {
        res->next = NULL;
    }
    else {
        res->next = (struct exprTailAST*) malloc(sizeof(struct exprTailAST));
        struct exprTailAST* cur = res->next;
        enum Token next = get(stream, &parseCursor);
        while(next == plusToken || next == minusToken) {
            if(next == plusToken)
                expect(stream, &parseCursor, plusToken);
            else
                expect(stream, &parseCursor, minusToken);
            cur->op = next;
            // printf("expr token: %d\n", cur->op);
            cur->term = parseTerm(stream);
            next = get(stream, &parseCursor);
            if (next == plusToken || next == minusToken) {
                cur->next = (struct exprTailAST*) malloc(sizeof(struct exprTailAST));
                cur = cur->next;
            }
        }
        // cur = NULL;
    }
    // printf("parsing expr correct...\n");
    return res;
}

struct desiAST* parseDesi(struct tokenStream stream) {
    struct desiAST* res = (struct desiAST*) malloc(sizeof(struct desiAST));
    res->id = parseIdent(stream);
    res->expr = NULL;
    res->type = 0;
    if(get(stream, &parseCursor) == openbracketToken) {
        res->type = 1;
        expect(stream, &parseCursor, openbracketToken);
        res->expr = parseExpr(stream);
        expect(stream, &parseCursor, closebracketToken);
    }
    return res;
}
struct stmtAST* parseAssign(struct tokenStream stream) {
    expect(stream, &parseCursor, letToken);
    struct assignAST* data = (struct assignAST*) malloc(sizeof(struct assignAST));

    data->lhs = parseDesi(stream);
    enum Token op = get(stream, &parseCursor);
    if((op != assignToken) && (op != selfplusToken) && (op != selfminusToken) && (op != selftimesToken) && (op != selfdivToken) && (op != increaseToken) && (op != decreaseToken)) {
        printf("parse error: expected one of the following tokens: := += -= *= /=\n");
        exit(-1);
    }
    data->op = op;
    parseCursor++;
    
    if(op == increaseToken || op == decreaseToken)
        data->rhs = NULL;
    else
        data->rhs = parseExpr(stream);

    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 0;
    res->data = (char*)data;
    return res;
}

struct stmtAST* parseCall(struct tokenStream stream) {
    expect(stream, &parseCursor, callToken);
    struct funcCallAST* data = (struct funcCallAST*) malloc(sizeof(struct funcCallAST));

    data->funcName = parseIdent(stream);
    expect(stream, &parseCursor, openparenToken);
    enum Token next = get(stream, &parseCursor);
    // printf("call ident success\n");
    if(next != closeparenToken) {
        // printf("non empty args...\n");
        data->args = (struct exprListAST*) malloc(sizeof(struct exprListAST));
        struct exprListAST* cur = data->args;
        cur->head = parseExpr(stream);
        // printf("expr success\n");
        enum Token expr_next = get(stream, &parseCursor);
        // printf("first next token: %d\n", expr_next);
        while(expr_next == commaToken) {
            expect(stream, &parseCursor, commaToken);
            cur->next = (struct exprListAST*) malloc(sizeof(struct exprListAST));
            cur->next->head = parseExpr(stream);
            cur = cur->next;
            expr_next = get(stream, &parseCursor);
            // printf("next token: %d\n", expr_next);
        }
    }
    else
        data->args = NULL;
    
    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 1;
    res->data = data;
    expect(stream, &parseCursor, closeparenToken);
    return res;
}

struct stmtAST* parseStmt(struct tokenStream stream) {
    enum Token start = get(stream, &parseCursor);
    struct stmtAST* res = NULL;

    switch(start) {
        case letToken: {
            // printf("handling assign\n");
            res = parseAssign(stream);
            break;
        }
        case callToken: {
            // printf("handling call\n");
            res = parseCall(stream);
            break;
        }
        default: {
            printf("stmt currently not supported...\n");
            exit(-1);
            break;
        }
    };
    // printf("parse stmt success...\n");
    return res;
}

struct stmtSeqAST* parseStmtSequence(struct tokenStream stream) {
    struct stmtSeqAST* stmts = (struct stmtSeqAST*) malloc(sizeof(struct stmtSeqAST));
    stmts->stat = parseStmt(stream);
    enum Token next = get(stream, &parseCursor);

    struct stmtSeqAST* cur = stmts;
    if(next == semiToken) {
        while(next == semiToken) {
            expect(stream, &parseCursor, semiToken);
            struct stmtSeqAST* _new = (struct stmtSeqAST*) malloc(sizeof(struct stmtSeqAST));
            _new->stat = parseStmt(stream);
            cur->next = _new;
            cur = _new;
            next = get(stream, &parseCursor);
        }
    }
    cur->next = NULL;
    next = get(stream, &parseCursor);
    if(next == semiToken)
        expect(stream, &parseCursor, semiToken);
    
    return stmts;
}

struct computationAST* parse(struct tokenStream stream) {

    struct computationAST* root = (struct computationAST*) malloc(sizeof(struct computationAST));

    expect(stream, &parseCursor, mainToken);
    enum Token next = get(stream, &parseCursor);
    if(next == varToken || next == arrayToken) {
        struct varDeclAST* vars = parseVarDecls(stream);
        root->vars = vars;
    }

    // next = get(stream, &parseCursor);
    // if(next == voidToken || next == funcToken) {
    //     struct funcAST* funcs = parseFuncDecls(stream);
    //     root.funcs = funcs;
    // }

    expect(stream, &parseCursor, beginToken);
    root->stats = parseStmtSequence(stream);

    // printf("next token: %d\n", get(stream, &parseCursor));
    expect(stream, &parseCursor, endToken);
    expect(stream, &parseCursor, periodToken);
    expect(stream, &parseCursor, eofToken);
    printf("parse success.\n");

    return root;
}