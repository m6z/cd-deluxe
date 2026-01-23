cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME cdd)

# ---- Dependencies ----
include(FetchContent)
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.3.1
)
FetchContent_MakeAvailable(cxxopts)

#----------------------------------------------------------------------

# Get the total commit count across all branches
execute_process(
    COMMAND git rev-list --count --all
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BUILD_NUMBER
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_RESULT
)

# Check if the git command was successful
if(GIT_RESULT EQUAL 0)
    message(STATUS "Git total commit count: ${GIT_BUILD_NUMBER}")
else()
    message(WARNING "Failed to get git commit count, defaulting to 0.")
    set(GIT_BUILD_NUMBER 0)
endif()

# Configure a header file (e.g., "config.h.in") to write the build number
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
    @ONLY
)

#----------------------------------------------------------------------

add_library(${TARGET_NAME} STATIC
    cdd2.cpp
    cdd_util.cpp
    cdd_options.cpp
    cdd_options_init.cpp
    fzf_wrapper.cpp
    process_util.cpp
)

# Add the directory where the configured header is generated
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Add the header to the source list (optional, but good practice)
target_sources(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# This command automatically adds the include directories and other
# usage requirements from cxxopts to the cdd target.
target_link_libraries(cdd PRIVATE cxxopts::cxxopts)

# Optional static lib flags (for GNU only)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(cdd PRIVATE -static-libgcc -static-libstdc++)
endif()
