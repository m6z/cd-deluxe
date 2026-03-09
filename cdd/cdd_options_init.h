/*

Copyright 2010-2026 Michael Graz
https://github.com/m6z/cd-deluxe

This file is part of Cd Deluxe.

Cd Deluxe is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cd Deluxe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cd Deluxe.  If not, see <http://www.gnu.org/licenses/>.

*/

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
