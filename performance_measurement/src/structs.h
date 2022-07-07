#ifndef ROOT_PROJECT_STRUCTS_H
#define ROOT_PROJECT_STRUCTS_H

#include <time.h>

struct TimeInfo {
    char *name;
    struct timespec start_time;
    struct timespec end_time;
};

#define BYTES_IN_GB 1000000000
#define BLOCKSIZE 512
#define MESSAGE_SIZE 512

#endif //ROOT_PROJECT_STRUCTS_H
