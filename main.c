#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "scanner.h"

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


    int *tokens = scan(raw_input);
    // int i = 0;
    // while(1) {
    //     if(*(tokens+i) == 255)
    //         break;
    //     printf("%d ", *(tokens+i));
    //     i++;
    // }

    exit(0);
}