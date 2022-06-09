#include "include/struct.h"
#include "include/util.h"

int main(int argc, char *argv[]) {
    struct Arguments arguments = parse_arguments(argc, argv);
    struct dirent *entries;
    int count;
    if (retrieve_files(arguments.target_directory, &entries, &count) != 0)
        return -1;

    print_files(arguments, entries, count);
    return 0;
}
