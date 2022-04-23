#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"

int token_length(enum Token input) {
    int length[256] = {
        0,1,1,1,1,0,0,0, 0,0,0,2,2,2,2,0,
        0,0,0,0,2,2,1,2, 2,1,0,0,0,0,1,1,
        1,1,1,1,0,0,0,0, 2,4,2,5,0,0,0,0,
        0,0,0,2,2,0,0,0, 0,0,0,0,0,0,1,4,
        0,0,0,0,0,0,1,0, 0,0,0,0,0,3,0,0,
        1,2,2,0,0,0,0,0, 0,0,4,0,0,0,0,0,
        0,0,0,0,4,2,5,6, 6,0,0,0,0,0,3,5,
        8,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,1,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 4,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
    };
    return length[input];
}
int scan_for_op(char *input){

    char c = *input;
    char next = *(input+1);
    switch(c) {
        case '*':
            return next=='=' ? selftimesToken : timesToken;
        case '/': {
            if(next == '/')
                return errorToken;
            return next=='=' ? selfdivToken : divToken;
        }
        case '+': {
            if(next == '+')
                return increaseToken;
            else if(next == '=')
                return selfplusToken;
            else
                return plusToken;
        }
        case '-': {
            if(next == '-')
                return decreaseToken;
            else if(next == '=')
                return selfminusToken;
            else if(!(next >= '0' && next <= '9'))
                return minusToken;
            else
                return errorToken;
        }
        case '>':
            return next=='=' ? geqToken : gtrToken;
        case '<':
            return next=='=' ? leqToken : lssToken;
        case '!':
            return next=='=' ? neqToken : errorToken;
        case '=':
            return next=='=' ? eqlToken : errorToken;
        case ':':
            return next=='=' ? assignToken : errorToken;
        case '.':
            return periodToken;
        case ',':
            return commaToken;
        case '(':
            return openparenToken;
        case ')':
            return closeparenToken;
        case '[':
            return openbracketToken;
        case ']':
            return closebracketToken;
        case '{':
            return beginToken;
        case '}':
            return endToken;
        case '_':
            return underscoreToken;
        case ';':
            return semiToken;
        default:
            return errorToken;
    }
}

int scan_for_keyword(char *input) {

    if(strncmp((const char*)input, "then", 4) == 0)
        return thenToken;
    if(strncmp((const char*)input, "do", 2) == 0)
        return doToken;
    if(strncmp((const char*)input, "until", 5) == 0)
        return untilToken;
    if(strncmp((const char*)input, "void", 4) == 0)
        return voidToken;
    if(strncmp((const char*)input, "od", 2) == 0)
        return odToken;
    if(strncmp((const char*)input, "fi", 2) == 0)
        return fiToken;
    if(strncmp((const char*)input, "else", 4) == 0)
        return elseToken;
    if(strncmp((const char*)input, "let", 3) == 0)
        return letToken;
    if(strncmp((const char*)input, "call", 4) == 0)
        return callToken;
    if(strncmp((const char*)input, "if", 2) == 0)
        return ifToken;
    if(strncmp((const char*)input, "while", 5) == 0)
        return whileToken;
    if(strncmp((const char*)input, "repeat", 6) == 0)
        return repeatToken;
    if(strncmp((const char*)input, "return", 6) == 0)
        return returnToken;
    if(strncmp((const char*)input, "var", 3) == 0)
        return varToken;
    if(strncmp((const char*)input, "array", 5) == 0)
        return arrayToken;
    if(strncmp((const char*)input, "function", 8) == 0)
        return funcToken;
    if(strncmp((const char*)input, "main", 4) == 0)
        return mainToken;
    return errorToken;

}

int scan_for_number(char *input){
    // scan for number token, return with the length of number.
    char c = *input;
    if(c != '-' && !(c >= '0' && c <= '9'))
        return errorToken;
    
    int len = 1;
    while('0' <= *(input+len) && *(input+len) <= '9')
        len++;

    return len;
}

int is_id_char(char c) {
    // check if a char will be one in an ident.
    return c == '_' || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z');
}

int scan_for_id(char *input){
    // scan for ident token, return with the length of ident.
    char c = *input;
    if( c < 'A' || (c > 'Z' && c < 'a') || c > 'z')
        return errorToken;
    
    int len = 1;
    
    while(is_id_char(*(input+len)))
        len++;
    
    return len;
}

struct tokenStream scan(char *input){
    
    int i = 0, j = 0, i_int = 0, i_id = 0, line_number = 1; 
    int len = strlen(input);
    int* output = (int *) malloc(len * 4);
    int* numbers = (int *) malloc(len * 4);
    int* id_strlen = (int *)malloc(len * 4);
    char **ids = (char **) malloc(len);
    while(i < len) {

        if(*(input+i) == '/' && *(input+i+1) == '/') { // skipping comment
            while(*(input+i) != '\n')
                i++;
            line_number++;
        }
        while(*(input+i) == ' ' || *(input+i) == '\n' || *(input+i) == '\t'){
            if(*(input+i) == '\n')
                line_number++;
            i++;
        }
            
        
        int status = 0;
        if((status = scan_for_op(input+i)) != 0) {
            // printf("match an op: %d\n", status);
            output[j++] = status;
            i += token_length(status);
            // printf("current pos: %d\n", i);
        }
            
        else if((status = scan_for_keyword(input+i)) != 0) {
            // printf("match a keyword: %d\n", status);
            output[j++] = status;
            i += token_length(status);
            // printf("current pos: %d\n", i);
        }

        else if((status = scan_for_number(input+i)) != 0) {
            // printf("match a number\n");
            output[j++] = number;
            char tmp_int[status];
            strncpy(tmp_int, input+i, status);
            numbers[i_int++] = atoi((const char*)tmp_int);
            i += status;
            // printf("current pos: %d\n", i);
        }

        else if((status = scan_for_id(input+i)) != 0) {
            // printf("match an id: ");
            // for(int k=i; k<i+status; k++)
            //     printf("%c", input[k]);
            // printf("\n");
            output[j++] = ident;
            ids[i_id] = (char *) malloc(status);
            id_strlen[i_id] = status;
            strncpy(ids[i_id++], input+i, status);
            i += status;
            // printf("current pos: %d\n", i);
        }
        
        else {
            // printf("%d, %d\n", i, input[i]);
            if(*(input+i) == '/' && *(input+i+1) == '/' || *(input+i) == ' ' || *(input+i) == '\n' || *(input+i) == '\t' || *(input+i) == '\r')
                continue;
            printf("scanner error at line: %d, exiting...\n", line_number);
            exit(-1);
        }
        // printf("%d, ", line_number);
    }
    output[j++] = eofToken;

    output = realloc(output, sizeof(int) * (j+1));
    numbers = realloc(numbers, sizeof(int) * (i_int+1));
    ids = realloc(ids, sizeof(char*) * (i_id+1));
    
    struct tokenStream result;
    result.tokens = output;
    result.numbers = numbers;
    result.ids = ids;
    result.id_strlen = id_strlen;
    result.token_len = j;
    result.int_len = i_int;
    result.id_len = i_id;

    return result;

}