#ifndef CUSTOM_LS_UTIL_H
#define CUSTOM_LS_UTIL_H

#include "struct.h"

int retrieve_files(char *target_directory, struct dirent **entries, int *count);

void print_files(struct Arguments arguments, struct dirent *entries, int count);

void get_full_path(char *directory, char *file, char **full_path);

#endif //CUSTOM_LS_UTIL_H
