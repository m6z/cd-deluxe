cmake_minimum_required(VERSION 3.20)

# ---- Dependencies ----
include(FetchContent)
FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.3.1
)
FetchContent_MakeAvailable(cxxopts)

#----------------------------------------------------------------------

add_library(cdd STATIC
    cdd2.cpp
    cdd_util.cpp
    cdd_options.cpp
    cdd_options_init.cpp
    fzf_wrapper.cpp
)

# This command automatically adds the include directories and other
# usage requirements from cxxopts to the cdd target.
target_link_libraries(cdd PRIVATE cxxopts::cxxopts)

# Optional static lib flags (for GNU only)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(cdd PRIVATE -static-libgcc -static-libstdc++)
endif()
