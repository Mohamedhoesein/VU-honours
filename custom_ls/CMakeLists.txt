cmake_minimum_required(VERSION 3.22)
project(custom_ls C)

set(CMAKE_C_STANDARD 11)

add_executable(custom_ls main.c src/util.c)

target_link_libraries(custom_ls m)
