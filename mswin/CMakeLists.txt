cmake_minimum_required(VERSION 3.20)

#----------------------------------------------------------------------
# Find the makensis executable

find_program(NSIS_EXECUTABLE
    NAMES
        makensis.exe
    PATHS
        "C:/Program Files (x86)/NSIS"
        "C:/Program Files (x86)/NSIS/Bin"
        "C:/Program Files/NSIS"
        "C:/Program Files/NSIS/Bin"
        ENV PATH
)

# If not found, provide a warning but don't fail (installer is optional)
if(NOT NSIS_EXECUTABLE)
    message(WARNING "NSIS (makensis) not found. The 'installer' target will not be available. Install NSIS and ensure it's in your PATH.")
    return()
endif()

message(STATUS "Found NSIS executable: ${NSIS_EXECUTABLE}")

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
    COMMAND cmd /c "del /f /q \"${INSTALLER_OUTPUT}\" 2>nul || exit /b 0"
    # Build the installer
    COMMAND ${NSIS_EXECUTABLE}
        /V2
        /NOCD
        "/DCDD_EXE=${INSTALLER_FILENAME}"
        "/DCDD_VER=${PROJECT_VERSION}"
        "/DCDD_EXE_DIR=$<TARGET_FILE_DIR:cd-deluxe>"
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

message(STATUS "Installer target configured. Build with: cmake --build <dir> --target installer")
