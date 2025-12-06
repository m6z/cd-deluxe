cmake_minimum_required(VERSION 4.0)

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

add_executable(testmain ${test_sources})

target_link_libraries(testmain
    PRIVATE
        cdd
        Catch2::Catch2WithMain
)

target_include_directories(testmain PRIVATE ${CMAKE_SOURCE_DIR})

# ---- Enable automatic test discovery ----
include(CTest)
include(Catch)
catch_discover_tests(testmain)
