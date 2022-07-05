# performance_measurement
This program writes a 1 GB file in 512 byte parts.
Each 512 byte part is randomly generated from `random`.
There are 3 cases being tested:
1. Using a file opened and created via `fopen`.
2. Using a file opened and created via `open` using the flags O_CREAT and O_WRONLY.
3. Using a file opened and created via `open` using the flags O_CREAT, O_WRONLY and O_DIRECT.

## General results
From some single digit runs and manually checking the results the following is the case:
* Case 1 is roughly half a second to a second faster than case 2. When using `fopen` and `fwrite` to write to a file some additional buffering is done which can explain the difference.
* Case 2 is roughly 11 times faster than case 3. The O_DIRECT flag ignores the caching that is done in the kernel when interacting with files, and this causes the difference.