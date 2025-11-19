#pragma once

#include <string>
#include <vector>

class CddOptions
{
public:
    // --- Parsed Results ---
    bool show_help = false;
    bool list_history = false;
    std::string default_action;
    std::string direction;
    int max_history = 10;
    int max_backwards = 10;
    int max_forwards = 10;
    int max_common = 10;
    int max_upwards = 10;
    bool all_history = false;
    bool ignore_case = false;

    std::vector<std::string> unmatched_args;

    // --- Status Information ---
    bool parse_error = false;
    std::string error_message;
    std::string help_text;

    CddOptions(const std::vector<std::string>& args, const std::string& env_options = "");

private:
    std::vector<std::string> combine_arguments(const std::vector<std::string>& args, const std::string& env_options);
};
