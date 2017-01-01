cmake_minimum_required(VERSION 3.0)
add_library(cdd cdd.cpp)
target_include_directories (cdd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
