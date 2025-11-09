cmake_minimum_required(VERSION 4.0)

set(TARGET_NAME "cd-deluxe")

# ---- Main executable ----
file(GLOB main_src CONFIGURE_DEPENDS "*.cpp")

add_executable(${TARGET_NAME} ${main_src})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        cdd
        cxxopts
)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

