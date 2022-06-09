#ifndef CUSTOM_LS_STRUCT_H
#define CUSTOM_LS_STRUCT_H

#include <dirent.h>
#include <stdbool.h>

struct Arguments {
    char *target_directory;
    bool long_form;
};

struct DIR_List_Item {
    DIR *dir;
    struct DIR_List_Item *next;
};

#endif //CUSTOM_LS_STRUCT_H
