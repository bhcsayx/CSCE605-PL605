#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "frontend/scanner.h"
#include "frontend/parser.h"

#include "graphviz/graphviz.h"

#include "IR/codegen.h"

int main(int argc, char *argv[]) {

    if(argc < 2) {
        printf("Usage: ./pl605 <input file> .\n");
        exit(-1);
    }

    const char* dir = argv[1];
    int input_fd = open(dir, O_RDONLY);

    if(input_fd < 0) {
        printf("Input file does not exist.\n");
        exit(-1);
    }

    struct stat file_meta;
    fstat(input_fd, &file_meta);
    off_t size = file_meta.st_size;
    // printf("Input file size: %ld\n", size);
    
    char *raw_input = (char*) malloc(size);
    read(input_fd, raw_input, size);
    // printf("source: %s\n", raw_input);


    struct tokenStream stream = scan(raw_input);

    // for(int i=0; i<stream.token_len; i++){
    //     printf("%d ", stream.tokens[i]);
    // }
    // printf("\n");
    // for(int i=0; i<stream.int_len; i++){
    //     printf("%d ", stream.numbers[i]);
    // }
    // printf("\n");
    // for(int i=0; i<stream.id_len; i++){
    //     printf("%s ", stream.ids[i]);
    // }

    struct computationAST* root = parse(stream);
    
    Module mod = codegen(root);
    dump2txt(mod);

    exit(0);
}