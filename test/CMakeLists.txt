cmake_minimum_required(VERSION 2.8)

# find_package (Boost COMPONENTS system regex program_options filesystem unit_test_framework REQUIRED)
# include_directories(.. ${Boost_INCLUDE_DIRS})

include_directories(..)

set(cdd_hdr ../cdd/cdd.h)

file(GLOB testmain_src "*.cpp")

add_executable(testmain ${testmain_src} ${cdd_hdr})

target_link_libraries (testmain LINK_PUBLIC cdd
#     ${Boost_SYSTEM_LIBRARY}
#     ${Boost_REGEX_LIBRARY}
#     ${Boost_PROGRAM_OPTIONS_LIBRARY}
#     ${Boost_FILESYSTEM_LIBRARY}
#     ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

