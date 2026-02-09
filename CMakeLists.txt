cmake_minimum_required(VERSION 3.20)

#----------------------------------------------------------------------
# Version information

project(cd-deluxe
    VERSION 2.0.6
    LANGUAGES CXX
)

# Get the total commit count across all branches
execute_process(
    COMMAND git rev-list --count --all
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE CDD_BUILD_NUMBER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_RESULT
)

# Check if the git command was successful
if(GIT_RESULT EQUAL 0)
    message(STATUS "Git total commit count: ${CDD_BUILD_NUMBER}")
else()
    set(CDD_BUILD_NUMBER 0)
    message(WARNING "Failed to get git commit count, defaulting to: ${CDD_BUILD_NUMBER}")
endif()

#----------------------------------------------------------------------

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

if (WIN32)
    add_subdirectory(mswin)
endif()

#----------------------------------------------------------------------
include(CTest)
enable_testing()
