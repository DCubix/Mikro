cmake_minimum_required(VERSION 3.10)
project(gzip)
add_library(gzip INTERFACE)
target_include_directories(gzip INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)