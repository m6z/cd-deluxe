#pragma once

#include <iostream>
#include <string>
#include <vector>

class CddOptions
{
public:
    // constants
    static constexpr const char* direction_forwards = "+";
    static constexpr const char* direction_backwards = "-";
    static constexpr const char* direction_common = ",";

    static constexpr const char* direction_default = direction_backwards;

    static constexpr const char* environment_variable_name = "CDD_OPTIONS";

    // --- Parsed Results ---
    bool show_help = false;
    bool list_history = false;
    std::string default_action;
    std::string direction = direction_default;
    std::size_t max_history = 10;
    std::size_t max_backwards = 10;
    std::size_t max_forwards = 10;
    std::size_t max_common = 10;
    std::size_t max_upwards = 10;
    bool all_history = false;
    bool ignore_case = false;

    std::vector<std::string> unmatched_args;

    // --- Status Information ---
    bool has_error = false;
    std::string error_message;
    // std::string help_text;

    CddOptions() {}
    CddOptions(const std::vector<std::string>& args, const std::string& env_options = "")
    {
        if (!initialize(args, env_options))
        {
            has_error = true;
        }
    }
    bool initialize(const std::vector<std::string>& args, const std::string& env_options);

    void output(std::ostream& os = std::cout) const;
};
