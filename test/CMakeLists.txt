cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME testmain)

# ---- Dependencies ----

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.6.0 # adjust to latest stable tag
)
FetchContent_MakeAvailable(Catch2)

# ---- Test executable ----
# file(GLOB test_sources CONFIGURE_DEPENDS "*.cpp")
set(test_sources
    cdd2_test.cpp
    cdd_options_test.cpp
    util_test.cpp
    cleanup_test.cpp
    test_helpers.cpp
)

add_executable(${TARGET_NAME} ${test_sources})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        cdd
        Catch2::Catch2WithMain
)

# precompiled header
target_precompile_headers(${TARGET_NAME} PRIVATE pch.h)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

# ---- Enable automatic test discovery ----
include(CTest)
include(Catch)
catch_discover_tests(${TARGET_NAME})
