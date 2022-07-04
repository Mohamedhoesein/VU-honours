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
#define BYTES_IN_HALF_GB BYTES_IN_GB / 2
#define BLOCKSIZE 512
#define MESSAGE_SIZE 512

struct WriteInfo {
    struct timespec *start_time;
    struct timespec *end_time;
    int file_size;
    int message_size;
};

void fopen_write_file(struct WriteInfo write_info);
void open_write_file(struct WriteInfo write_info);
void O_DIRECT_open_write_file(struct WriteInfo write_info);
void print_times(struct timespec *start_time, struct timespec *end_time);
struct timespec diff_timespec(struct timespec *end_time, struct timespec *start_time);

int main() {
    remove("./fopen");
    remove("./open");
    remove("./O_DIRECT_open");
    struct timespec fopen_start_time;
    struct timespec fopen_end_time;
    struct WriteInfo fopen_info = {
        .start_time = &fopen_start_time,
        .end_time = &fopen_end_time,
        .file_size = BYTES_IN_HALF_GB,
        .message_size = MESSAGE_SIZE
    };
    struct timespec open_start_time;
    struct timespec open_end_time;
    struct WriteInfo open_info = {
            .start_time = &open_start_time,
            .end_time = &open_end_time,
            .file_size = BYTES_IN_HALF_GB,
            .message_size = MESSAGE_SIZE
    };
    struct timespec O_DIRECT_open_start_time;
    struct timespec O_DIRECT_open_end_time;
    struct WriteInfo O_DIRECT_open_info = {
            .start_time = &O_DIRECT_open_start_time,
            .end_time = &O_DIRECT_open_end_time,
            .file_size = BYTES_IN_HALF_GB,
            .message_size = MESSAGE_SIZE
    };
    fopen_write_file(fopen_info);
    open_write_file(open_info);
    O_DIRECT_open_write_file(O_DIRECT_open_info);
    printf("fopen:\n");
    print_times(&fopen_end_time, &fopen_start_time);
    printf("open:\n");
    print_times(&open_end_time, &open_start_time);
    printf("O_DIRECT open:\n");
    print_times(&O_DIRECT_open_end_time, &O_DIRECT_open_start_time);
    return 0;
}

void fopen_write_file(struct WriteInfo write_info) {
    char *file_name = "./fopen";
    long long number_of_writes = write_info.file_size / write_info.message_size;
    FILE *fp = fopen(file_name, "w+");
    char value[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&value, write_info.message_size, 0);

        printf("fopen: %d/%lld\n", i, number_of_writes);

        if (fwrite(&value, write_info.message_size, 1, fp) != 1) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    fclose(fp);
}

void open_write_file(struct WriteInfo write_info) {
    char *file_name = "./open";
    long long number_of_writes = write_info.file_size / write_info.message_size;
    int fd = open(file_name, O_CREAT | O_WRONLY);
    char value[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&value, write_info.message_size, 0);

        printf("open: %d/%lld\n", i, number_of_writes);

        if (write(fd, &value, write_info.message_size) != write_info.message_size) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    close(fd);
}

void O_DIRECT_open_write_file(struct WriteInfo write_info) {
    char *file_name = "./O_DIRECT_open";
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

void print_times(struct timespec *start_time, struct timespec *end_time) {
    struct timespec diff = diff_timespec(end_time, start_time);
    printf("%ld s %ld ns\n", diff.tv_sec, diff.tv_nsec);
}

struct timespec diff_timespec(struct timespec *end_time, struct timespec *start_time) {
    struct timespec diff = {.tv_sec = start_time->tv_sec - end_time->tv_sec, .tv_nsec =
    start_time->tv_nsec - end_time->tv_nsec};
    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }
    return diff;
}

