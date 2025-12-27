#pragma once

#include <iostream>
#include <string>
#include <vector>

class CddOptionsInit
{
public:
    CddOptionsInit() = default;

    /**
     * Parses arguments looking specifically for --help or --init.
     * Returns true if one of these options was found and handled.
     * Returns false if neither was found (program should proceed to main logic).
     */
    int parse(int argc, char* argv[]);

    bool has_error() const { return !error_message.empty(); }
    const std::string& get_error() const { return error_message; }

private:
    std::string error_message;

    // Helper to get the full path of the current running executable
    std::string get_self_executable_path(const char* argv0) const;

    // Helper to detect current shell from environment variables
    std::string detect_shell() const;

    // Generates the shell setup help message
    void print_shell_setup_help(const std::string& shell_type, const std::string& exe_path) const;

    // Generates the shell script content
    void print_init_script(const std::string& shell_type, const std::string& exe_path) const;
};
