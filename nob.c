#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#define NOB_EXPERIMENTAL_DELETE_OLD

#include "./nob.h"

#define LIB_LINUX_DIR "./lib/linux/"
#define LIB_WINDOWS_DIR "./lib/windows/"
#define LIB_WEB_DIR "./lib/web/"
#define BUILD_DIR "./build/"
#define BUILD_DIR_WEB "./build/web/"
#define SRC_DIR "./src/"
#define INCLUDE_GEN_DIR SRC_DIR"include/gen/"
#define NINJA_EXE BUILD_DIR "ninja"
#define NINJA_WEB BUILD_DIR_WEB "ninja.html"

Nob_Cmd cmd = {0};

bool run = false;
bool windows = false;
bool web = false;

bool compile_asset(const char* path) {
    Nob_Cmd comp_cmd = {0};
    const char* file_name = nob_path_name(path);
    const char* dot = strrchr(file_name, '.');
    size_t base_len = (dot ? (size_t)(dot - file_name) : strlen(file_name));
    static char full_path[512];
    int n = snprintf(full_path, sizeof(full_path), "%s%.*s%s", INCLUDE_GEN_DIR, (int)base_len, file_name, ".h");
    if (n >= sizeof(full_path)) return false;

    const char* header_name = full_path;

    cmd_append(&comp_cmd, "xxd", "-i");
    cmd_append(&comp_cmd, path);
    cmd_append(&comp_cmd, header_name);

    if (!cmd_run(&comp_cmd)) return false;
    return true;
}

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

void set_lib_dir(void) {
    if (windows) {
        cmd_append(&cmd, "-L" LIB_WINDOWS_DIR);
    } else if (web) {
        cmd_append(&cmd, "-L" LIB_WEB_DIR);
    }
    else {
        cmd_append(&cmd, "-L" LIB_LINUX_DIR);
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
        cmd_append(&cmd, "--shell-file", LIB_WEB_DIR"shell.html");
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

    if (!mkdir_if_not_exists(BUILD_DIR))
        return 1;

    if (!mkdir_if_not_exists(INCLUDE_GEN_DIR))
        return 1;

    if (web) {
        if (!mkdir_if_not_exists(BUILD_DIR_WEB))
            return 1;
    }

    if (!compile_asset("./assets/idle.png")) return 1;
    if (!compile_asset("./assets/jump.png")) return 1;
    if (!compile_asset("./assets/run.png")) return 1;
    if (!compile_asset("./assets/slide.png")) return 1;
    if (!compile_asset("./assets/wall_slide.png")) return 1;

    set_compiler();
    nob_cc_flags(&cmd);
    if (!web) cmd_append(&cmd, "-ggdb");
    set_output();
    nob_cc_inputs(&cmd, SRC_DIR "main.c");
    set_lib_dir();
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
