cmake_minimum_required(VERSION 2.8)

find_package (Boost COMPONENTS system regex program_options filesystem REQUIRED)

include_directories(.. ${Boost_INCLUDE_DIRS})

set(cdd_hdr ../cdd/cdd.h)

file(GLOB main_src "*.cpp")

add_executable(_cdd ${main_src} ${cdd_hdr})

target_link_libraries (_cdd LINK_PUBLIC cdd
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

