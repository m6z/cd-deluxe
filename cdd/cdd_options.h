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

class CddOptions
{
public:
    // constants
    static constexpr const char* direction_forwards = "+";
    static constexpr const char* direction_forwards_alt = "f"; // alias for forwards (PowerShell-friendly)
    static constexpr const char* direction_backwards = "-";
    static constexpr const char* direction_backwards_alt = "b"; // alias for backwards (PowerShell-friendly)
    static constexpr const char* direction_common = ",";
    static constexpr const char* direction_common_alt = "c"; // alias for common (PowerShell-friendly)
    static constexpr const char* direction_upwards = "..";
    static constexpr const char* direction_upwards_alt = "u"; // alias for upwards (PowerShell-friendly)

    static constexpr const char* direction_default = direction_backwards;

    static constexpr const char* environment_variable_name = "CDD_OPTIONS";

    // --- Parsed Results ---
    bool show_help = false;
    bool show_version = false;
    bool list_history = false;
    std::string direction = direction_default;
    std::size_t max_history = 10;
    std::size_t max_backwards = 10;
    std::size_t max_forwards = 10;
    std::size_t max_common = 10;
    std::size_t max_upwards = 10;
    bool all_history = false;
    bool ignore_case = false;
    bool delete_entry = false;
    bool reset_history = false;
    bool garbage_collect = false;
    bool use_fzf = false;
    std::string error_message;

    std::vector<std::string> unmatched_args;

    CddOptions() {}
    CddOptions(const std::vector<std::string>& args, const std::string& env_options = "") { initialize(args, env_options); }
    bool initialize(const std::vector<std::string>& args, const std::string& env_options);

    bool has_error() const { return !error_message.empty(); }
    void set_error(const std::string& msg) { error_message = "** " + msg; }

    void output(std::ostream& os = std::cout) const;
};
