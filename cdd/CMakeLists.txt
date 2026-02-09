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

# Configure a header file to assign the version number
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
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
