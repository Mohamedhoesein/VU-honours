#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/write.h"
#include "src/structs.h"

void test_writes(struct TimeInfo **times, int *size);
void print_time(struct TimeInfo time_info);
struct timespec diff_timespec(struct timespec start_time, struct timespec end_time);

int main() {
    struct TimeInfo *times = calloc(1, sizeof(struct TimeInfo));
    int size = 0;

    test_writes(&times, &size);

    printf("\nDuration:\n");
    for (int i = 0; i < size; ++i)
        print_time(times[i]);
}

void test_writes(struct TimeInfo **times, int *size) {
    struct WriteTestInfo test_info = {
        .time_size = size,
        .name = NULL,
        .times = times,
        .file_size = BYTES_IN_GB,
        .message_size = MESSAGE_SIZE,
        .write_type = fopen_write
    };
    place_name(&test_info, "fopen");
    write_file(test_info);
    place_name(&test_info, "open");
    test_info.write_type = open_write;
    write_file(test_info);
    place_name(&test_info, "O_DIRECT_open");
    test_info.write_type = O_DIRECT_write;
    write_file(test_info);
}

void place_name(struct WriteTestInfo *test_info, char *name) {
    if (test_info->name != NULL)
        free(test_info->name);
    test_info->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(test_info->name, name);
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

