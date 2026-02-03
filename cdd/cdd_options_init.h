#pragma once

#include <iostream>
#include <string>
#include <vector>

class CddOptionsInit
{
public:
    CddOptionsInit(std::ostream& output_stream = std::cout, std::istream& input_stream = std::cin)
        : output_stream_(output_stream), input_stream_(input_stream)
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
    std::istream& input_stream_;

    // Helper to get the full path of the current running executable
    std::string get_self_executable_path(const char* argv0) const;

    // Generates the shell setup help message
    void print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const;

    // Generates the shell script content
    void print_init_script(const std::string& shell_type, const std::string& exe_path) const;

#ifdef _WIN32
    // Windows-specific: check and optionally create cdd.cmd wrapper script
    void check_and_create_cmd_wrapper(const std::string& exe_path);
#endif
};
