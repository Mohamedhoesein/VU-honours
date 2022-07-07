#ifndef ROOT_PROJECT_WRITE_H
#define ROOT_PROJECT_WRITE_H

enum WriteType {
    fopen_write,
    open_write,
    O_DIRECT_write
};

struct WriteTest {
    char *name;
    enum WriteType write_type;
};

struct WriteTestInfo {
    int *time_size;
    struct TimeInfo **times;
    int file_size;
    int message_size;
    int write_test_size;
    struct WriteTest *write_test;
};

struct WriteInfo {
    char *name;
    struct timespec *start_time;
    struct timespec *end_time;
    int file_size;
    int message_size;
};

void place_name(struct WriteTestInfo *test_info, char *name);
void write_file(struct WriteTestInfo test_info);
void fopen_write_file(struct WriteInfo write_info);
void open_write_file(struct WriteInfo write_info);
void O_DIRECT_open_write_file(struct WriteInfo write_info);

#endif //ROOT_PROJECT_WRITE_H
