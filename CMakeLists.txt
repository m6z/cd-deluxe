cmake_minimum_required(VERSION 4.0)
project(cd-deluxe VERSION 2.0 LANGUAGES CXX)

# Enforce modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable strict warnings for development
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add subdirectories
add_subdirectory(cdd)
add_subdirectory(test)
add_subdirectory(main)

#----------------------------------------------------------------------
include(CTest)
enable_testing()
