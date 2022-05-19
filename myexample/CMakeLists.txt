cmake_minimum_required(VERSION 3.0)

project (pblaster)

set (CMAKE_BUILD_TYPE Debug)

message("PROJECT_BINARY_DIR:" ${PROJECT_BINARY_DIR})

include_directories (include lib src)

file (GLOB SOURCES "src/*.cpp")

add_library(spinapi SHARED
    lib/spinapi.c
)