#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

enum Token {
    errorToken=0,
    timesToken,
    divToken,
    plusToken,
    minusToken,
    selftimesToken=11,
    selfdivToken,
    selfplusToken,
    selfminusToken,
    eqlToken=20,
    neqToken,
    lssToken,
    geqToken,
    leqToken,
    gtrToken,
    periodToken=30,
    commaToken,
    openbracketToken,
    openparenToken,
    closebracketToken,
    closeparenToken,
    assignToken=40,
    thenToken,
    doToken,
    untilToken,
    increaseToken=51,
    decreaseToken,
    number=60,
    ident,
    underscoreToken,
    voidToken,
    semiToken=70,
    endToken=80,
    odToken,
    fiToken,
    elseToken=90,
    letToken=77,
    callToken=100,
    ifToken,
    whileToken,
    repeatToken,
    returnToken,
    varToken=110,
    arrayToken,
    funcToken,
    beginToken=150,
    mainToken=200,
    readToken,
    writeToken,
    writeNLToken,
    cmpToken,
    jmpToken,
    blankToken, // token for placeholder in some AST fields
    eofToken=255
};

struct tokenStream {
    int* tokens;
    const char** ids;
    int *id_strlen;
    int *numbers;
    int token_len;
    int int_len;
    int id_len;
};


struct tokenStream scan(char *input);
int is_id_char(char c);
int scan_for_op(char *input);
int scan_for_keyword(char *input);
int scan_for_number(char *input);
int scan_for_id(char *input);

#endif