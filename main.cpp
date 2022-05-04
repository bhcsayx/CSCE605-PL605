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
#include "IR/SSA.h"

#include "backend/regalloc.h"
#include "backend/dlxgen.h"

#include "optimization/arithsim.h"
#include "optimization/constfold.h"
#include "optimization/orphanfunc.h"

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
    // printf("get var length: %d\n", mod.varNames.size());
    // dump2txt(mod);
    SSABuilder builder = SSABuilder(mod);

    // string cfg_name = "../graphs/";
    string cfg_name = "";
    cfg_name.append((dir));
    cfg_name.append("-CFG.dot");
    printf("cfg name: %s\n", cfg_name.c_str());
    dump2dot(builder, cfg_name);

    builder.transform();
    // dump2txt(mod);

    // string ssa_name = "../graphs/";
    string ssa_name = "";
    ssa_name.append((dir));
    ssa_name.append("-SSA.dot");
    printf("ssa name: %s\n", ssa_name.c_str());
    dump2dot(builder, ssa_name);

    builder.detransform();
    // string dessa_name = "../graphs/";
    string dessa_name = "";
    dessa_name.append((dir));
    dessa_name.append("-deSSA.dot");
    printf("dessa name: %s\n", dessa_name.c_str());
    dump2dot(builder, dessa_name);

    // for(auto func: builder.funcNames) {
    //     printf("glob func: %s\n", func.c_str());
    //     for(auto s: *(builder.globalNames[func])) {
    //         printf("glob var: %s\n", s.c_str());
    //     }
    // }
    // arithSim sim;
    // sim.run(builder);
    // string sim_name = "";
    // sim_name.append((dir));
    // sim_name.append("-arithSim.dot");
    // sim.dump(builder, sim_name);

    // constFold fold;
    // fold.run(builder);
    // string fold_name = "";
    // fold_name.append((dir));
    // fold_name.append("-constFold.dot");
    // fold.dump(builder, fold_name);

    // orphanFunc orf;
    // orf.run(builder);
    // string orf_name = "";
    // orf_name.append((dir));
    // orf_name.append("-orphanFunc.dot");
    // orf.dump(builder, orf_name);

    liveVarAnalysis LVA(builder);
    // string reg_name = "../graphs/";
    string lva_name = "";
    lva_name.append((dir));
    lva_name.append("-LVA.dot");
    LVA.dump2dot(builder, lva_name);

    regAlloc alloc(builder, LVA, 5);
    string reg_name = "";
    reg_name.append((dir));
    reg_name.append("-regAlloc.dot");
    alloc.dump(builder, reg_name);

    DLXGenerator dlxgen(builder, alloc);
    dlxgen.dlxgen(builder);

    string dlx_name = "";
    dlx_name.append((dir));
    dlx_name.append(".dlx");
    dlxgen.dump(dlx_name);

    exit(0);
}