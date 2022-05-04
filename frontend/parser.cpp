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
        printf("parser error: expected %d, found %d\n", expected, stream.tokens[*cursor]);
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
    char* dest = (char*) malloc(stream.id_strlen[idCursor]+1);
    strncpy(dest, stream.ids[idCursor], stream.id_strlen[idCursor]);
    dest[stream.id_strlen[idCursor]] = '\0';
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
            res->data = parseFacCall(stream);
            // printf("faccall: %d\n", res->data);
            break;
        }

        default: {
            printf("error parsing designator: %d\n", next);
            exit(-1);
            break;
        }
    }

    return res;
}

struct funcCallAST* parseFacCall(struct tokenStream stream) {
    expect(stream, &parseCursor, callToken);
    struct funcCallAST* data = (struct funcCallAST*) malloc(sizeof(struct funcCallAST));

    data->funcName = parseIdent(stream);
    expect(stream, &parseCursor, openparenToken);
    enum Token next = get(stream, &parseCursor);
    // printf("cur next:%d\n", next);
    // printf("call ident success\n");
    if(next != closeparenToken) {
        // printf("non empty args...%d\n", next);
        data->args = (struct exprListAST*) malloc(sizeof(struct exprListAST));
        struct exprListAST* cur = data->args;
        cur->head = parseExpr(stream);
        // printf("term: %d\n", cur->head->term);
        // printf("expr success\n");
        enum Token expr_next = get(stream, &parseCursor);
        // printf("first next token: %d\n", expr_next);
        while(expr_next == commaToken) {
            expect(stream, &parseCursor, commaToken);
            cur->next = (struct exprListAST*) malloc(sizeof(struct exprListAST));
            cur->next->head = parseExpr(stream);
            cur = cur->next;
            expr_next = get(stream, &parseCursor);
            printf("next token: %d\n", expr_next);
        }
        cur->next = NULL;
    }
    else
        data->args = NULL;
    expect(stream, &parseCursor, closeparenToken);
    return data;
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

struct relAST* parseRelation(struct tokenStream stream) {
    struct relAST* res = (struct relAST*) malloc(sizeof(struct relAST));
    res->lhs = parseExpr(stream);
    enum Token op = get(stream, &parseCursor);
    if((op != eqlToken) && (op != neqToken) && (op != lssToken) && (op != geqToken) && (op != leqToken) && (op != gtrToken)) {
        printf("parse error: expected one of the following tokens: == != < > <= >=\n");
        exit(-1);
    }
    res->op = get(stream, &parseCursor);
    parseCursor++;
    res->rhs = parseExpr(stream);
    return res;
}

struct stmtAST* parseBranch(struct tokenStream stream) {
    expect(stream, &parseCursor, ifToken);
    struct brhAST* data = (struct brhAST*) malloc(sizeof(struct brhAST));
    data->cond = parseRelation(stream);
    expect(stream, &parseCursor, thenToken);
    data->br1 = parseStmtSequence(stream);
    enum Token next = get(stream, &parseCursor);
    if(next == elseToken) {
        expect(stream, &parseCursor, elseToken);
        data->br2 = parseStmtSequence(stream);
    }
    else if(next == fiToken)
        data->br2 = NULL;
    else {
        printf("error processing ifStatement.\n");
        exit(-1);
    }
    expect(stream, &parseCursor, fiToken);
    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 2;
    res->data = (char*)data;
    return res;
}

struct stmtAST* parseWhile(struct tokenStream stream) {
    expect(stream, &parseCursor, whileToken);
    struct loopAST* data = (struct loopAST*) malloc(sizeof(struct loopAST));
    data->type = 0;
    data->cond = parseRelation(stream);
    expect(stream, &parseCursor, doToken);
    data->body = parseStmtSequence(stream);
    expect(stream, &parseCursor, odToken);
    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 3;
    res->data = (char*)data;
    return res;
}

struct stmtAST* parseRepeat(struct tokenStream stream) {
    expect(stream, &parseCursor, repeatToken);
    struct loopAST* data = (struct loopAST*) malloc(sizeof(struct loopAST));
    data->type = 1;
    data->body = parseStmtSequence(stream);
    expect(stream, &parseCursor, untilToken);
    data->cond = parseRelation(stream);
    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 4;
    res->data = (char*)data;
    return res;
}

struct stmtAST* parseReturn(struct tokenStream stream) {
    expect(stream, &parseCursor, returnToken);
    struct retAST* data = (struct retAST*) malloc(sizeof(struct retAST));
    if(get(stream, &parseCursor) == semiToken)
        data->expr = NULL;
    else
        data->expr = parseExpr(stream);
    struct stmtAST* res = (struct stmtAST*) malloc(sizeof(struct stmtAST));
    res->type = 5;
    res->data = (char*)data;
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
        // printf("head: %d\n", cur->head->term);
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
        cur->next = NULL;
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
            // printf("args: %d\n", ((struct funcCallAST*)(res->data))->args);
            // if(((struct funcCallAST*)(res->data))->args != NULL) {
            //     printf("1st arg term%d\n", ((struct funcCallAST*)(res->data))->args->head->term);
            // }
            break;
        }
        case ifToken: {
            // printf("handling branch\n");
            res = parseBranch(stream);
            // printf("next after if: %d\n", get(stream, &parseCursor));
            break;
        }
        case whileToken: {
            // printf("handling while\n");
            res = parseWhile(stream);
            break;
        }
        case repeatToken: {
            // printf("handling repeat");
            res = parseRepeat(stream);
            break;
        }
        case returnToken: {
            // printf("handling return\n");
            res = parseReturn(stream);
            // printf("return finished\n");
            break;
        }
        default: {
            if(start == endToken) {
                // printf("empty function\n");
                return NULL;
            }
            printf("stmt currently not supported...%d\n", start);
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
    if(!(stmts->stat))
        return NULL;
    enum Token next = get(stream, &parseCursor);

    struct stmtSeqAST* cur = stmts;
    // if(next == semiToken) {
    //     while(next == semiToken) {
    //         expect(stream, &parseCursor, semiToken);
    //         struct stmtSeqAST* _new = (struct stmtSeqAST*) malloc(sizeof(struct stmtSeqAST));
    //         _new->stat = parseStmt(stream);
    //         cur->next = _new;
    //         cur = _new;
    //         next = get(stream, &parseCursor);
    //         printf("next token: %d\n", next);
    //     }
    // }
    if(next == semiToken) {
        expect(stream, &parseCursor, semiToken);
        next = get(stream, &parseCursor);
        while(next == letToken || next == callToken || next == ifToken || next == whileToken || next == repeatToken || next == returnToken) {
            struct stmtSeqAST* _new = (struct stmtSeqAST*) malloc(sizeof(struct stmtSeqAST));
            _new->stat = parseStmt(stream);
            cur->next = _new;
            cur = _new;
            if(get(stream, &parseCursor) == semiToken)
                expect(stream, &parseCursor, semiToken);
            next = get(stream, &parseCursor);
            // printf("next token: %d\n", next);
        }
    }
    cur->next = NULL;
    next = get(stream, &parseCursor);
    if(next == semiToken)
        expect(stream, &parseCursor, semiToken);
    
    return stmts;
}

struct funcAST* parseFuncDecl(struct tokenStream stream) {
    enum Token start = get(stream, &parseCursor);
    struct funcAST* res = (struct funcAST*)malloc(sizeof(struct funcAST));

    if(start == voidToken)
        expect(stream, &parseCursor, voidToken);
    expect(stream, &parseCursor, funcToken);
    res->name = parseIdent(stream);
    // printf("func name: %s\n", res->name);

    // parse formal params
    expect(stream, &parseCursor, openparenToken);
    enum Token next = get(stream, &parseCursor);
    res->params = NULL;
    if(next != closeparenToken) {
        res->params = (struct varDeclAST*)malloc(sizeof(struct varDeclAST));
        struct varDeclAST* cur = res->params;
        while(next != closeparenToken) {
            cur->type.type = 0;
            cur->name = parseIdent(stream);
            next = get(stream, &parseCursor);
            if(next != closeparenToken) {
                expect(stream, &parseCursor, commaToken);
                // next = get(stream, &parseCursor);
                // if(next != closeparenToken)
                cur->next = (struct varDeclAST*)malloc(sizeof(struct varDeclAST));
                cur = cur->next;
            }
            else
                cur->next = NULL;
        }
    }
    // printf("next: %d\n", get(stream, &parseCursor));
    expect(stream, &parseCursor, closeparenToken);
    next = get(stream, &parseCursor);
    if(next == semiToken)
        expect(stream, &parseCursor, semiToken);
    // printf("next: %d\n", get(stream, &parseCursor));

    // parse func body
    expect(stream, &parseCursor, beginToken);
    next = get(stream, &parseCursor);
    if(next == varToken || next == arrayToken)
        res->decls = parseVarDecls(stream);
    else
        res->decls = NULL;
    res->stmts = parseStmtSequence(stream);
    expect(stream, &parseCursor, endToken);
    next = get(stream, &parseCursor);
    if(next == semiToken)
        expect(stream, &parseCursor, semiToken);

    return res;
}

struct computationAST* parse(struct tokenStream stream) {

    struct computationAST* root = (struct computationAST*) malloc(sizeof(struct computationAST));

    expect(stream, &parseCursor, mainToken);
    enum Token next = get(stream, &parseCursor);
    if(next == varToken || next == arrayToken) {
        struct varDeclAST* vars = parseVarDecls(stream);
        root->vars = vars;
    }

    next = get(stream, &parseCursor);
    root->funcs = NULL;
    if(next == voidToken || next == funcToken) {
        root->funcs = (struct funcListAST*)malloc(sizeof(struct funcListAST));
        struct funcListAST* cur = root->funcs;
        while(next == voidToken || next == funcToken) {
            cur->func = parseFuncDecl(stream);
            next = get(stream, &parseCursor);
            if(next == voidToken || next == funcToken) {
                cur->next = (struct funcListAST*)malloc(sizeof(struct funcListAST));
                cur = cur->next;
            }
        }
        cur->next = NULL;
    }
    // printf("funcdecl finished\n");

    expect(stream, &parseCursor, beginToken);
    root->stats = parseStmtSequence(stream);

    // printf("next token: %d\n", get(stream, &parseCursor));
    expect(stream, &parseCursor, endToken);
    expect(stream, &parseCursor, periodToken);
    expect(stream, &parseCursor, eofToken);
    printf("parse success.\n");

    return root;
}