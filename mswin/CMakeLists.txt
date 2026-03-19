cmake_minimum_required(VERSION 3.20)

#----------------------------------------------------------------------
# Find the makensis executable

find_program(NSIS_EXECUTABLE
    NAMES
        makensis.exe
    PATHS
        "$ENV{ProgramW6432}/NSIS"       # 64-bit Program Files
        "$ENV{ProgramFiles}/NSIS"       # Current arch Program Files
        "C:/Program Files (x86)/NSIS"   # Hardcoded fallback
        "C:/Program Files/NSIS"         # Hardcoded fallback
    PATH_SUFFIXES Bin
)

if(NOT NSIS_EXECUTABLE)
    message(FATAL_ERROR "NSIS not found. Installer target will not be available.")
endif()

message(STATUS "Found NSIS: ${NSIS_EXECUTABLE}")

#----------------------------------------------------------------------
# Define the installer output filename and paths

set(INSTALLER_FILENAME "CddInstaller_${PROJECT_VERSION}.${CDD_BUILD_NUMBER}.exe")
set(INSTALLER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${INSTALLER_FILENAME}")

#----------------------------------------------------------------------
# Custom command to build the installer
# NSIS creates output in source directory (standard NSIS workflow)

add_custom_command(
    OUTPUT ${INSTALLER_OUTPUT}
    # Clean up old installer file (ignore errors if locked)
    COMMAND ${CMAKE_COMMAND} -E remove -f ${INSTALLER_OUTPUT}
    # Build the installer
    COMMAND ${NSIS_EXECUTABLE}
        /V2
        /NOCD
        "/DCDD_EXE=${INSTALLER_FILENAME}"
        "/DCDD_VER=${PROJECT_VERSION}"
        "/DCDD_EXE_DIR=$<TARGET_FILE_DIR:cd-deluxe>"
        "/DOUTPUT_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
        ${CMAKE_CURRENT_SOURCE_DIR}/cdd_installer.nsi
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS cd-deluxe
            ${CMAKE_CURRENT_SOURCE_DIR}/cdd_installer.nsi
            ${CMAKE_CURRENT_SOURCE_DIR}/cdd_shell.cmd
            ${CMAKE_CURRENT_SOURCE_DIR}/cdd_shell.ps1
            ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt
    COMMENT "Building Windows installer: ${INSTALLER_FILENAME}"
    VERBATIM
)

#----------------------------------------------------------------------
# Custom target 'installer' that depends on the installer executable

add_custom_target(installer
    DEPENDS ${INSTALLER_OUTPUT}
)

# Ensure cd-deluxe is built before the installer
add_dependencies(installer cd-deluxe)
