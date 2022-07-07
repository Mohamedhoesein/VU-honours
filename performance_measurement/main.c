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
        .times = times,
        .file_size = BYTES_IN_GB,
        .message_size = MESSAGE_SIZE
    };
    test_info.write_test = calloc(sizeof(struct WriteTest), 3);
    test_info.write_test[0].name = "fopen";
    test_info.write_test[0].write_type = fopen_write;
    test_info.write_test[1].name = "open";
    test_info.write_test[1].write_type = open_write;
    test_info.write_test[2].name = "O_DIRECT_open";
    test_info.write_test[2].write_type = O_DIRECT_write;
    test_info.write_test_size = 3;
    write_file(test_info);
    remove("fopen");
    remove("open");
    remove("O_DIRECT_open");
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

