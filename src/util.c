#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <grp.h>
#include "../include/util.h"

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
    *entries = calloc(*count, sizeof(struct dirent));

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
    return strcoll(((struct dirent*)left)->d_name, ((struct dirent*)right)->d_name);
}

void get_full_path(char *directory, char *file, char **full_path) {
    *full_path = calloc(strlen(directory) + strlen(file) + 2, sizeof(char));
    strcpy(*full_path, directory);
    char backslash[] = "/";
    strcat(*full_path, backslash);
    strcat(*full_path, file);
}

char *int_to_string(unsigned int value) {
    int character_count = (int)(floor(log10(value)) + 2);
    char *result = calloc(character_count, sizeof(char));
    sprintf(result, "%d", value);

    return result;
}

int retrieve_information(char *full_path, char *name, struct file_information *information){
    struct stat file_stat;
    stat(full_path, &file_stat);

    information->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(information->name, name);
    information->size = file_stat.st_size;
    information->number_of_links = file_stat.st_nlink;
    struct timespec last_modified = file_stat.st_mtim;
    struct timespec last_status_change = file_stat.st_ctim;

    if (
        last_modified.tv_sec < last_status_change.tv_sec ||
        (
            last_modified.tv_sec == last_status_change.tv_sec &&
            last_modified.tv_nsec < last_status_change.tv_nsec
        )
    ) {
        information->last_modified = last_modified.tv_sec;
    }
    else {
        information->last_modified = last_status_change.tv_sec;
    }

    struct passwd *pwd = getpwuid(file_stat.st_uid);
    if (pwd != NULL)
        information->owner = strdup(pwd->pw_name);
    else
        information->owner = int_to_string(file_stat.st_uid);

    struct group *group = getgrgid(file_stat.st_gid);
    if (group != NULL)
        information->group_owner = strdup(group->gr_name);
    else
        information->group_owner = int_to_string(file_stat.st_gid);

    information->permissions.directory = S_ISDIR(file_stat.st_mode);
    information->permissions.set_user_id = file_stat.st_mode & S_ISUID;
    information->permissions.set_group_id = file_stat.st_mode & S_ISGID;
    information->permissions.sticky_bit = file_stat.st_mode & S_ISVTX;
    information->permissions.user.read = file_stat.st_mode & S_IRUSR;
    information->permissions.user.write = file_stat.st_mode & S_IWUSR;
    information->permissions.user.execute = file_stat.st_mode & S_IXUSR;
    information->permissions.group.read = file_stat.st_mode & S_IRGRP;
    information->permissions.group.write = file_stat.st_mode & S_IWGRP;
    information->permissions.group.execute = file_stat.st_mode & S_IXGRP;
    information->permissions.other.read = file_stat.st_mode & S_IROTH;
    information->permissions.other.read = file_stat.st_mode & S_IWOTH;
    information->permissions.other.execute = file_stat.st_mode & S_IXOTH;

    return 0;
}

void print_information_short(struct dirent *entries, int count) {
    for (int i = 0; i < count; ++i) {
        if (count == i - 1)
            printf("%s", entries[i].d_name);
        else
            printf("%s ", entries[i].d_name);
    }
}

void print_permission_character(bool condition, char match) {
    printf("%c", condition ? match : '-');
}

void print_date(time_t seconds_since_epoch) {
    struct tm *ts;
    ts = localtime(&seconds_since_epoch);
    switch (ts->tm_mon) {
        case 0:
            printf(" Jan");
            break;
        case 1:
            printf(" Feb");
            break;
        case 3:
            printf(" Mar");
            break;
        case 4:
            printf(" Apr");
            break;
        case 5:
            printf(" Jun");
            break;
        case 6:
            printf(" Jul");
            break;
        case 7:
            printf(" Aug");
            break;
        case 8:
            printf(" Sep");
            break;
        case 9:
            printf(" Oct");
            break;
        case 10:
            printf(" Nov");
            break;
        case 11:
            printf(" Dec");
            break;
    }
    printf(" %2d %02d:%02d", ts->tm_mday, ts->tm_hour, ts->tm_min);
}

void print_individual_file_long(struct file_information *information, struct width_information width) {
    print_permission_character(information->permissions.directory, 'd');
    print_permission_character(information->permissions.user.read, 'r');
    print_permission_character(information->permissions.user.write, 'w');
    if (information->permissions.set_user_id && information->permissions.user.execute)
        printf("s");
    else if (information->permissions.set_user_id)
        printf("S");
    else
        print_permission_character(information->permissions.user.execute, 'x');
    print_permission_character(information->permissions.group.read, 'r');
    print_permission_character(information->permissions.group.write, 'w');
    if (information->permissions.set_group_id && information->permissions.group.execute)
        printf("s");
    else if (information->permissions.set_group_id)
        printf("S");
    else
        print_permission_character(information->permissions.group.execute, 'x');
    print_permission_character(information->permissions.other.read, 'r');
    print_permission_character(information->permissions.other.write, 'w');
    if (information->permissions.sticky_bit && information->permissions.other.execute)
        printf("t");
    else if (information->permissions.sticky_bit)
        printf("T");
    else
        print_permission_character(information->permissions.other.execute, 'x');

    printf(" %*lu", width.links, information->number_of_links);
    printf(" %*s", width.owner, information->owner);
    printf(" %*s", width.group_owner, information->group_owner);
    printf(" %*lu", width.size, information->size);

    print_date(information->last_modified);

    printf(" %s\n", information->name);
}

void print_information_long(struct arguments arguments, struct dirent *entries, int count) {
    struct file_information *all_information = calloc(count, sizeof(struct file_information));
    unsigned long largest_link_count = 0;
    long largest_size = LONG_MIN;
    unsigned int owner_length = 0;
    unsigned int group_length = 0;

    for (int i = 0; i < count; ++i) {
        char *full_path;
        get_full_path(arguments.target_directory, entries[i].d_name, &full_path);

        retrieve_information(full_path, entries[i].d_name, &(all_information[i]));

        if (all_information[i].number_of_links > largest_link_count)
            largest_link_count = all_information[i].number_of_links;
        if (all_information[i].size > largest_size)
            largest_size = all_information[i].size;

        unsigned int current_owner_length = strlen(all_information[i].owner);
        if (current_owner_length > owner_length)
            owner_length = current_owner_length;
        unsigned int current_group_length = strlen(all_information[i].group_owner);
        if (current_group_length > group_length)
            group_length = current_group_length;

        free(full_path);
    }

    unsigned int number_of_digits_link = (unsigned int)floorl(log10l(largest_link_count)) + 1;
    unsigned int number_of_digits_size = (unsigned int)floorl(log10l(largest_size)) + 1;

    struct width_information width = {
        .links = number_of_digits_link,
        .size = number_of_digits_size,
        .owner = owner_length,
        .group_owner = group_length
    };

    for (int i = 0; i < count; ++i) {
        print_individual_file_long(&all_information[i], width);
    }

    for (int i = 0; i < count; ++i) {
        free(all_information[i].group_owner);
        free(all_information[i].owner);
        free(all_information[i].name);
    }
    free(all_information);
}

void print_files(struct arguments arguments, struct dirent *entries, int count) {
    qsort(entries, count, sizeof(struct dirent), compare);
    if (!arguments.long_form) {
        print_information_short(entries, count);
    }
    else {
        print_information_long(arguments, entries, count);
    }
}