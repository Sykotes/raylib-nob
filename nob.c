#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#include "./nob.h"

#define INCLUDE_FOLDER "./include/"
#define LIB_FOLDER "./lib/"
#define BUILD_FOLDER "./build/"
#define SRC_FOLDER "./src/"
#define NINJA_EXE BUILD_FOLDER"ninja"

Nob_Cmd cmd = {0};

bool run = false;

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    
    const char *program_name = shift(argv, argc);

    while (argc > 0) {
        const char *flag = shift(argv, argc);
        if (strcmp(flag, "-r") == 0) { 
            run = true; 
            continue; 
        }
        if (strcmp(flag, "--") == 0) break;
        fprintf(stderr, "%s:%d: ERROR: unknown flag `%s`\n", __FILE__, __LINE__, flag);
        if (run) {
            fprintf(stderr, "NOTE: use -- to separate %s and %s command line arguments\n", program_name, NINJA_EXE);
        }
        return 1;
    }

    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-o"NINJA_EXE);
    cmd_append(&cmd, SRC_FOLDER"main.c");
    cmd_append(&cmd, "-L"LIB_FOLDER);
    cmd_append(&cmd, "-l:libraylib.a");
    cmd_append(&cmd, "-lm");
    if (!cmd_run(&cmd)) return 1;

    if (run) {
        cmd_append(&cmd, NINJA_EXE);
        da_append_many(&cmd, argv, argc);
        if (!cmd_run(&cmd)) return 1;
    }

    return 0;
}
