#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#include "./nob.h"

#define LIB_FOLDER "./lib/"
#define LIB_WIN_FOLDER "./lib-win/"
#define LIB_WEB_FOLDER "./lib-web/"
#define BUILD_FOLDER "./build/"
#define BUILD_FOLDER_WEB "./build/web/"
#define SRC_FOLDER "./src/"
#define NINJA_EXE BUILD_FOLDER "ninja"
#define NINJA_WEB BUILD_FOLDER_WEB "ninja.html"

Nob_Cmd cmd = {0};

bool run = false;
bool windows = false;
bool web = false;

void set_output(void) {
    if (web) {
        nob_cc_output(&cmd, NINJA_WEB);
    } else {
        nob_cc_output(&cmd, NINJA_EXE);
    }
}

void set_compiler(void) {
    if (windows) {
        cmd_append(&cmd, "x86_64-w64-mingw32-gcc");
    } else if (web){
        cmd_append(&cmd, "emcc");
    } else {
        nob_cc(&cmd);
    }
}

void set_lib_folder(void) {
    if (windows) {
        cmd_append(&cmd, "-L" LIB_WIN_FOLDER);
    } else if (web) {
        cmd_append(&cmd, "-L" LIB_WEB_FOLDER);
    }
    else {
        cmd_append(&cmd, "-L" LIB_FOLDER);
    }
}

void windows_stuff(void) {
    if (windows) {
        cmd_append(&cmd, "-lopengl32", "-lgdi32", "-lwinmm");
    }
}

bool web_stuff(void) {
    if (web) {
        cmd_append(&cmd, "-s", "USE_GLFW=3");
        cmd_append(&cmd, "-s", "ASYNCIFY");
        cmd_append(&cmd, "--shell-file", LIB_WEB_FOLDER"shell.html");
        cmd_append(&cmd, "--preload-file", "assets");
        cmd_append(&cmd, "-DPLATFORM_WEB");
    }
    return true;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program_name = shift(argv, argc);

    while (argc > 0) {
        const char *flag = shift(argv, argc);
        if (strcmp(flag, "-r") == 0) {
            run = true;
            continue;
        }
        if (strcmp(flag, "-win") == 0) {
            windows = true;
            continue;
        }
        if (strcmp(flag, "-web") == 0) {
            web = true;
            continue;
        }
        if (strcmp(flag, "--") == 0)
            break;
        fprintf(stderr, "%s:%d: ERROR: unknown flag `%s`\n", __FILE__, __LINE__,
                flag);
        if (run) {
            fprintf(
                stderr,
                "NOTE: use -- to separate %s and %s command line arguments\n",
                program_name, NINJA_EXE);
        }
        return 1;
    }

    if (windows && web) {
        fprintf(stderr, "NOTE: cannot compile for multiple platforms\n");
        return 1;
    }

    if (!mkdir_if_not_exists(BUILD_FOLDER))
        return 1;

    if (web) {
        if (!mkdir_if_not_exists(BUILD_FOLDER_WEB))
            return 1;
    }

    set_compiler();
    nob_cc_flags(&cmd);
    set_output();
    nob_cc_inputs(&cmd, SRC_FOLDER "main.c");
    set_lib_folder();
    cmd_append(&cmd, "-l:libraylib.a");
    cmd_append(&cmd, "-lm");
    windows_stuff();
    if (!web_stuff()) {
        return 1;
    }

    if (!cmd_run(&cmd))
        return 1;

    if (run) {
        if (web) {
            cmd_append(&cmd, "emrun");
            cmd_append(&cmd, NINJA_WEB);
        }
        else { 
            if (windows) {
                cmd_append(&cmd, "wine");
            }
            cmd_append(&cmd, NINJA_EXE);
            da_append_many(&cmd, argv, argc);
        }
        if (!cmd_run(&cmd))
            return 1;
    }

    return 0;
}
