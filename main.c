#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/random.h>

#define BYTES_IN_GB 1000000000
#define BLOCKSIZE 512
#define MESSAGE_SIZE 512

struct TimeInfo {
    char *name;
    struct timespec start_time;
    struct timespec end_time;
};

struct WriteInfo {
    struct timespec *start_time;
    struct timespec *end_time;
    int file_size;
    int message_size;
};

enum WriteType {
    fopen_write,
    open_write,
    O_DIRECT_write
};

struct TestInfo {
    int *time_size;
    struct TimeInfo **times;
    char *name;
    int file_size;
    int message_size;
    enum WriteType write_type;
};

void place_name(struct TestInfo *test_info, char *name);
void execute(struct TestInfo test_info);
void fopen_write_file(struct WriteInfo write_info);
void open_write_file(struct WriteInfo write_info);
void O_DIRECT_open_write_file(struct WriteInfo write_info);
void print_time(struct TimeInfo time_info);
struct timespec diff_timespec(struct timespec start_time, struct timespec end_time);

int main() {
    struct TimeInfo *times = calloc(1, sizeof(struct TimeInfo));
    int size = 0;
    struct TestInfo test_info = {
        .time_size = &size,
        .name = NULL,
        .times = &times,
        .file_size = BYTES_IN_GB,
        .message_size = MESSAGE_SIZE,
        .write_type = fopen_write
    };
    place_name(&test_info, "fopen");
    execute(test_info);
    place_name(&test_info, "open");
    test_info.write_type = open_write;
    execute(test_info);
    place_name(&test_info, "O_DIRECT_open");
    test_info.write_type = O_DIRECT_write;
    execute(test_info);

    printf("\nDuration:\n");
    for (int i = 0; i < size; ++i)
        print_time(times[i]);
}

void place_name(struct TestInfo *test_info, char *name) {
    if (test_info->name != NULL)
        free(test_info->name);
    test_info->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(test_info->name, name);
}

void execute(struct TestInfo test_info) {
    ++*test_info.time_size;
    *test_info.times = realloc(*test_info.times, sizeof(struct TimeInfo) * *test_info.time_size);
    struct TimeInfo *times = *test_info.times;
    struct TimeInfo *time = &times[*test_info.time_size - 1];

    time->name = calloc(strlen(test_info.name) + 1, sizeof(char));
    strcpy(time->name, test_info.name);

    struct WriteInfo write_info = {
        .start_time = &(time->start_time),
        .end_time = &(time->end_time),
        .file_size = test_info.file_size,
        .message_size = test_info.message_size
    };
    switch (test_info.write_type) {
        case (fopen_write):
            fopen_write_file(write_info);
            break;
        case (open_write):
            open_write_file(write_info);
            break;
        case (O_DIRECT_write):
            O_DIRECT_open_write_file(write_info);
            break;
        default:
            break;
    }
}

void fopen_write_file(struct WriteInfo write_info) {
    char *file_name = "./fopen";
    remove(file_name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    FILE *fp = fopen(file_name, "w+");
    char buffer[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&buffer, write_info.message_size, 0);

        printf("fopen: %d/%lld\n", i, number_of_writes);

        if (fwrite(&buffer, write_info.message_size, 1, fp) != 1) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    fclose(fp);
}

void open_write_file(struct WriteInfo write_info) {
    char *file_name = "./open";
    remove(file_name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    int fd = open(file_name, O_CREAT | O_WRONLY);
    char buffer[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&buffer, write_info.message_size, 0);

        printf("open: %d/%lld\n", i, number_of_writes);

        if (write(fd, &buffer, write_info.message_size) != write_info.message_size) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    close(fd);
}

void O_DIRECT_open_write_file(struct WriteInfo write_info) {
    char *file_name = "./O_DIRECT_open";
    remove(file_name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    int fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY | O_DIRECT, S_IRWXU);
    void *buffer;
    posix_memalign(&buffer, BLOCKSIZE, write_info.message_size);
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(buffer, write_info.message_size, 0);

        printf("open: %d/%lld\n", i, number_of_writes);
        if (write(fd, buffer, write_info.message_size) != write_info.message_size) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    close(fd);
}

void print_time(struct TimeInfo time_info) {
    struct timespec diff = diff_timespec(time_info.start_time, time_info.end_time);
    printf("%s: %ld s %ld ns\n", time_info.name, diff.tv_sec, diff.tv_nsec);
}

struct timespec diff_timespec(struct timespec start_time, struct timespec end_time) {
    struct timespec diff = {
        .tv_sec = end_time.tv_sec - start_time.tv_sec,
        .tv_nsec = end_time.tv_nsec - start_time.tv_nsec
    };
    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }
    return diff;
}

