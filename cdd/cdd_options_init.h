#pragma once

#include <iostream>
#include <string>
#include <vector>

class CddOptionsInit
{
public:
    CddOptionsInit(std::ostream& output_stream = std::cout
#ifdef _WIN32
                   ,
                   std::istream& input_stream = std::cin
#endif
                   )
        : output_stream_(output_stream)
#ifdef _WIN32
          ,
          input_stream_(input_stream)
#endif
    {
    }

    /**
     * Parses arguments looking specifically for --help or --init.
     * Returns true if one of these options was found and handled.
     * Returns false if neither was found (program should proceed to main logic).
     */
    int parse(int argc, char* argv[], bool force_default_setup = false);

private:
    std::ostream& output_stream_;
#ifdef _WIN32
    std::istream& input_stream_;
#endif

    // Helper to get the full path of the current running executable
    std::string get_self_executable_path(const char* argv0) const;

    // Generates the shell setup help message
    void print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const;

    // Generates the shell script content
    void print_init_script(const std::string& shell_type, const std::string& exe_path) const;

#ifdef _WIN32
    // Windows-specific: check and optionally create cdd.cmd wrapper script
    // If force is true, overwrite existing file without prompting
    void check_and_create_cmd_wrapper(const std::string& exe_path, bool force = false);
    // Windows-specific: check and optionally create cdd.ps1 wrapper script for PowerShell
    // If force is true, overwrite existing file without prompting
    void check_and_create_ps1_wrapper(const std::string& exe_path, bool force = false);
#endif
};
