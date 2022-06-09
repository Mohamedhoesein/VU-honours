#ifndef CUSTOM_LS_UTIL_H
#define CUSTOM_LS_UTIL_H

#include "struct.h"

struct Arguments parse_arguments(int argc, char *argv[]);

int retrieve_files(char *target_directory, struct dirent **entries, int *count);

void print_files(struct Arguments arguments, struct dirent *entries, int count);

#endif //CUSTOM_LS_UTIL_H
