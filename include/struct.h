#ifndef CUSTOM_LS_STRUCT_H
#define CUSTOM_LS_STRUCT_H

#include <dirent.h>
#include <stdbool.h>
#include <time.h>

struct Arguments {
    char *target_directory;
    bool long_form;
};

struct AccessClassPermissions {
    bool read;
    bool write;
    bool execute;
};

struct FilePermissions {
    bool set_user_id;
    bool set_group_id;
    bool sticky_bit;
    struct AccessClassPermissions user;
    struct AccessClassPermissions group;
    struct AccessClassPermissions other;
};

struct FileInformation {
    bool is_directory;
    bool is_link;
    char *full_path;
    char *name;
    long size;
    time_t last_modified;
    unsigned long number_of_links;
    char *owner;
    char *group_owner;
    struct FilePermissions permissions;
};

struct WidthInformation {
    unsigned int links;
    unsigned int size;
    unsigned int owner;
    unsigned int group_owner;
};

#endif //CUSTOM_LS_STRUCT_H

