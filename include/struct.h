#ifndef CUSTOM_LS_STRUCT_H
#define CUSTOM_LS_STRUCT_H

#include <dirent.h>
#include <stdbool.h>
#include <time.h>

struct arguments {
    char *target_directory;
    bool long_form;
};

struct access_class_permissions {
    bool read;
    bool write;
    bool execute;
};

struct file_permissions {
    bool directory;
    struct access_class_permissions user;
    struct access_class_permissions group;
    struct access_class_permissions other;
};

struct file_information {
    char *name;
    long size;
    time_t last_modified;
    unsigned long number_of_links;
    char *owner;
    char *group_owner;
    struct file_permissions permissions;
};

struct width_information {
    unsigned int links;
    unsigned int size;
    unsigned int owner;
    unsigned int group_owner;
};

#endif //CUSTOM_LS_STRUCT_H

