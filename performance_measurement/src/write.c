#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/random.h>
#include "structs.h"
#include "write.h"

void write_file(struct WriteTestInfo test_info) {
    for (int i = 0; i < test_info.write_test_size; ++i) {
        struct WriteTest current_test = test_info.write_test[i];
        ++*test_info.time_size;
        *test_info.times = realloc(*test_info.times, sizeof(struct TimeInfo) * *test_info.time_size);
        struct TimeInfo *times = *test_info.times;
        struct TimeInfo *time = &times[*test_info.time_size - 1];

        time->name = calloc(strlen(current_test.name) + 1, sizeof(char));
        strcpy(time->name, current_test.name);

        struct WriteInfo write_info = {
            .start_time = &(time->start_time),
            .end_time = &(time->end_time),
            .file_size = test_info.file_size,
            .message_size = test_info.message_size
        };
        write_info.name = time->name;
        switch (current_test.write_type) {
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
}

void fopen_write_file(struct WriteInfo write_info) {
    remove(write_info.name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    FILE *fp = fopen(write_info.name, "w+");
    char buffer[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&buffer, write_info.message_size, 0);

        printf("%s: %d/%lld\n", write_info.name, i, number_of_writes);

        if (fwrite(&buffer, write_info.message_size, 1, fp) != 1) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    fclose(fp);
}

void open_write_file(struct WriteInfo write_info) {
    remove(write_info.name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    int fd = open(write_info.name, O_CREAT | O_WRONLY);
    char buffer[write_info.message_size];
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(&buffer, write_info.message_size, 0);

        printf("%s: %d/%lld\n", write_info.name, i, number_of_writes);

        if (write(fd, &buffer, write_info.message_size) != write_info.message_size) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    close(fd);
}

void O_DIRECT_open_write_file(struct WriteInfo write_info) {
    remove(write_info.name);
    long long number_of_writes = write_info.file_size / write_info.message_size;
    int fd = open(write_info.name, O_CREAT | O_WRONLY | O_DIRECT);
    void *buffer;
    posix_memalign(&buffer, BLOCKSIZE, write_info.message_size);
    srandom(time(NULL));

    timespec_get(write_info.start_time, TIME_UTC);
    for (int i = 0; i < number_of_writes; ++i) {
        getrandom(buffer, write_info.message_size, 0);

        printf("%s: %d/%lld\n", write_info.name, i, number_of_writes);

        if (write(fd, buffer, write_info.message_size) != write_info.message_size) {
            printf("error: %s\n", strerror(errno));
            return;
        }
    }
    timespec_get(write_info.end_time, TIME_UTC);

    close(fd);
}