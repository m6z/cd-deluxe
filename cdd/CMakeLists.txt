cmake_minimum_required(VERSION 2.8)
add_library(cdd cdd.cpp)
target_link_libraries(cdd -static-libgcc -static-libstdc++)
target_include_directories (cdd PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
