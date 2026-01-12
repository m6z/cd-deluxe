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
