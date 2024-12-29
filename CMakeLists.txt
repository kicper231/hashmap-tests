cmake_minimum_required(VERSION 3.30)
project(untitled C)

set(CMAKE_C_STANDARD 17)

add_executable(untitled main.c hashmap_old.c hashmap_old.h error.h
        farmhash-c.h
        hashmap_new.c
        hashmap_new.h
        hashfarm-c.c
)
