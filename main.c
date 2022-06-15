#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "include/struct.h"
#include "include/util.h"

static char doc[] = "Custom ls implementation, use -i to specify a is_directory.\n-l is only supported.\n";
static char args_doc[] = "";
static struct argp_option options[] = {
        {"directory", 'i', "is_directory", 0, "Directory to list" },
        {"long", 'l', 0, 0, "Produce long output" },
        { 0 }
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct Arguments *arguments = state->input;
    switch (key) {
        case 'i':
            arguments->target_directory = arg;
            break;
        case 'l':
            arguments->long_form = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct Arguments parse_arguments(int argc, char *argv[]) {
    struct Arguments arguments = {
        .target_directory = NULL,
        .long_form = false
    };

    struct argp argp = { options, parse_opt, args_doc, doc };
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return arguments;
}

int main(int argc, char *argv[]) {
    struct Arguments arguments = parse_arguments(argc, argv);
    bool default_path = false;
    if (arguments.target_directory == NULL) {
        arguments.target_directory = calloc(3, sizeof(char));
        strcpy(arguments.target_directory, "./");
        default_path = true;
    }

    struct dirent *entries;
    int count;
    if (retrieve_files(arguments.target_directory, &entries, &count) != 0)
        return -1;

    print_files(arguments, entries, count);

    if (default_path)
        free(arguments.target_directory);

    free(entries);

    return 0;
}
