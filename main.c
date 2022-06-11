#include <stdlib.h>
#include <string.h>
#include "include/struct.h"
#include "include/util.h"

struct arguments parse_arguments(int argc, char *argv[]) {
    struct arguments arguments = {
        .target_directory = NULL,
        .long_form = false
    };

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            arguments.target_directory = argv[++i];
        }
        else if (strcmp(argv[i], "-l") == 0) {
            arguments.long_form = true;
        }
    }

    return arguments;
}

int main(int argc, char *argv[]) {
    struct arguments arguments = parse_arguments(argc, argv);
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
