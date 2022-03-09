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
    eofToken=255
};

struct tokenStream {
    enum Token* tokens;
    const char** ids;
    int *numbers;
    int token_len;
    int int_len;
    int id_len;
};


struct tokenStream scan(char *input);
int scan_for_op(char *input);
int scan_for_keyword(char *input);
int scan_for_number(char *input);
int scan_for_id(char *input);