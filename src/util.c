#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/util.h"

struct Arguments parse_arguments(int argc, char *argv[]) {
    struct Arguments arguments = {
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

int retrieve_files(char *target_directory, struct dirent **entries, int *count) {
    DIR *dir = opendir(target_directory);
    if (dir == NULL) {
        return -1;
    }

    *count = 0;
    struct dirent *entry = readdir(dir);
    while (entry) {
        ++(*count);
        entry = readdir(dir);
    }

    int new_count = 0;
    *entries = malloc(sizeof(struct dirent) * *count);

    rewinddir(dir);
    entry = readdir(dir);

    while (entry) {
        ++new_count;

        if (new_count > *count) {
            return -2;
        }
        (*entries)[new_count - 1] = *entry;

        entry = readdir(dir);
    }

    if (new_count < *count) {
        return -3;
    }

    closedir(dir);

    return 0;
}

int compare(const void *left, const void *right) {
    /*char order[] = {
            '.',
            'a', 'A', 'b', 'B',
            'c', 'C', 'd', 'D',
            'e', 'E', 'f', 'F',
            'g', 'G', 'h', 'H',
            'i', 'I', 'j', 'J',
            'k', 'K', 'l', 'L',
            'm', 'M', 'n', 'N',
            'o', 'O', 'p', 'P',
            'q', 'Q', 'r', 'R',
            's', 'S', 't', 'T',
            'v', 'V', 'w', 'W',
            'x', 'X', 'y', 'Y',
            'z', 'Z',
            '0', '1', '2', '3',
            '4', '5', '6', '7',
            '8', '9'
    };*/
    return strcoll(((struct dirent*)left)->d_name, ((struct dirent*)right)->d_name);
}

void print_files(struct Arguments arguments, struct dirent *entries, int count) {
    qsort(entries, count, sizeof(struct dirent), compare);
    if (!arguments.long_form) {
        for (int i = 0; i < count; ++i) {
            if (count == i - 1)
                printf("%s", entries[i].d_name);
            else
                printf("%s ", entries[i].d_name);
        }
    }
    else {
        //TODO: print long form
    }
}