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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cdd_options.h"
#include "cdd_options_init.h"
#include "cdd_util.h"
#include "cxxopts.hpp"

namespace
{

bool is_valid_direction(const std::string& s)
{
    return s == CddOptions::direction_forwards ||      //
           s == CddOptions::direction_forwards_alt ||  // "f" alias for forwards
           s == CddOptions::direction_backwards ||     //
           s == CddOptions::direction_backwards_alt || // "b" alias for backwards
           s == CddOptions::direction_common ||        //
           s == CddOptions::direction_common_alt ||    // "c" alias for common
           s == CddOptions::direction_upwards ||       //
           s == CddOptions::direction_upwards_alt;     // "u" alias for upwards
}

// Check if direction is a symbol (not a letter alias) - used for positional shorthand
// Letter aliases like 'b', 'f', 'c', 'u' should only work with -d/--direction, not as positional args
bool is_symbol_direction(const std::string& s)
{
    return s == CddOptions::direction_forwards ||  // "+"
           s == CddOptions::direction_backwards || // "-"
           s == CddOptions::direction_common ||    // ","
           s == CddOptions::direction_upwards;     // ".."
}

// Normalize direction aliases to their canonical form
std::string normalize_direction(const std::string& s)
{
    if (s == CddOptions::direction_forwards_alt)
        return CddOptions::direction_forwards;
    if (s == CddOptions::direction_backwards_alt)
        return CddOptions::direction_backwards;
    if (s == CddOptions::direction_common_alt)
        return CddOptions::direction_common;
    if (s == CddOptions::direction_upwards_alt)
        return CddOptions::direction_upwards;
    return s;
}

std::string get_valid_directions_as_string()
{
    return "\"" + std::string(CddOptions::direction_forwards) + "\" or \"" +         //
           std::string(CddOptions::direction_forwards_alt) + "\" (forwards), \"" +   //
           std::string(CddOptions::direction_backwards) + "\" or \"" +               //
           std::string(CddOptions::direction_backwards_alt) + "\" (backwards), \"" + //
           std::string(CddOptions::direction_common) + "\" or \"" +                  //
           std::string(CddOptions::direction_common_alt) + "\" (common), \"" +       //
           std::string(CddOptions::direction_upwards) + "\" or \"" +                 //
           std::string(CddOptions::direction_upwards_alt) + "\" (upwards)";          //
}

// Helper to split the environment string into a vector of arguments
std::vector<std::string> tokenize_environment_variable(const std::string& env_options)
{
    std::vector<std::string> tokens;
    if (env_options.empty())
    {
        return tokens;
    }

    // Add a dummy program name so cxxopts parsing aligns with argv[0] expectations
    tokens.push_back("cdd_env");

    std::istringstream iss(env_options);
    std::string field;

    while (iss >> std::ws) // Consume leading whitespace
    {
        int nextChar = iss.peek();
        if (nextChar == '"')
        {
            iss >> std::quoted(field);
        }
        else
        {
            iss >> field;
        }
        tokens.push_back(field);
    }
    return tokens;
}

// Flags for direction shorthand options (used to set direction after parsing)
struct DirectionFlags
{
    bool backwards = false;
    bool forwards = false;
    bool common = false;
    bool upwards = false;
};

static DirectionFlags direction_flags;

// Helper to define the subset of options allowed in the environment variable
void add_common_options(CddOptions& x, cxxopts::Options& options)
{
    options.add_options()                                                                                                     //
        ("l,list", "List history", cxxopts::value(x.list_history)->implicit_value("true"))                                    //
        ("f,fzf", "Filter history with fzf", cxxopts::value(x.use_fzf)->implicit_value("true"))                               //
        ("d,direction", get_valid_directions_as_string(), cxxopts::value(x.direction))                                        //
        ("c,common", "Set direction to common (same as -d,)", cxxopts::value(direction_flags.common)->implicit_value("true")) //
        ("db", "Set direction to backwards (same as -d-)", cxxopts::value(direction_flags.backwards)->implicit_value("true")) //
        ("df", "Set direction to forwards (same as -d+)", cxxopts::value(direction_flags.forwards)->implicit_value("true"))   //
        ("dc", "Set direction to common (same as -d,)", cxxopts::value(direction_flags.common)->implicit_value("true"))       //
        ("du", "Set direction to upwards (same as -d..)", cxxopts::value(direction_flags.upwards)->implicit_value("true"))    //
        ("m,max", "Max history", cxxopts::value(x.max_history))                                                               //
        ("max-backwards", "Max backwards history", cxxopts::value(x.max_backwards))                                           //
        ("max-forwards", "Max forwards history", cxxopts::value(x.max_forwards))                                              //
        ("max-common", "Max common history", cxxopts::value(x.max_common))                                                    //
        ("max-upwards", "Max upwards history", cxxopts::value(x.max_upwards))                                                 //
        ("a,all", "Show all history", cxxopts::value(x.all_history)->implicit_value("true"))                                  //
        ("i,ignore-case", "Ignore case when comparing paths", cxxopts::value(x.ignore_case)->implicit_value("true"))          //
        ("init", "Generate shell initialization code")                                                                        //
        ;
}

// Apply direction flags to options (call after parsing)
void apply_direction_flags(CddOptions& x)
{
    // Priority: explicit -d/--direction > shorthand flags
    // Only apply flags if direction wasn't explicitly set via -d
    if (direction_flags.common)
        x.direction = CddOptions::direction_common;
    else if (direction_flags.backwards)
        x.direction = CddOptions::direction_backwards;
    else if (direction_flags.forwards)
        x.direction = CddOptions::direction_forwards;
    else if (direction_flags.upwards)
        x.direction = CddOptions::direction_upwards;

    // Reset flags for next parse
    direction_flags = DirectionFlags{};
}

// Helper to define the full set of options (includes common options)
void add_full_options(CddOptions& x, cxxopts::Options& options)
{
    add_common_options(x, options);

    options.add_options()                                                                                            //
        ("h,help", "Show this help message", cxxopts::value(x.show_help)->implicit_value("true"))                    //
        ("v,version", "Show version", cxxopts::value(x.show_version)->implicit_value("true"))                        //
        ("del", "Delete an entry", cxxopts::value(x.delete_entry)->implicit_value("true"))                           //
        ("reset", "Reset history", cxxopts::value(x.reset_history)->implicit_value("true"))                          //
        ("gc,garbage-collect", "Garbage collect history", cxxopts::value(x.garbage_collect)->implicit_value("true")) //
        ;

    // options.custom_help("xx custom help");
    // options.positional_help("xx positional help");
}

void output_footer()
{
    std::cerr << "See https://github.com/m6z/cd-deluxe for more information." << std::endl;
    std::cerr << "Copyright (c) " << get_year() << " Michael Graz" << std::endl;
}

} // namespace

//----------------------------------------------------------------------

bool CddOptions::initialize(const std::vector<std::string>& args, const std::string& env_options)
{
#ifdef WIN32
    ignore_case = true;
#endif

    if (args.empty())
    {
        set_error("Argument vector cannot be empty.");
        return false;
    }

    //-------------------------------------------------------------------------
    // 1. Parse Environment Variable (Subset of options)
    //-------------------------------------------------------------------------
    if (!env_options.empty())
    {
        try
        {
            cxxopts::Options env_parser("env", "Environment options");
            add_common_options(*this, env_parser);

            auto env_args = tokenize_environment_variable(env_options);

            // Convert to C-style argv for cxxopts
            std::vector<const char*> argv_env;
            for (const auto& arg : env_args)
            {
                argv_env.push_back(arg.c_str());
            }
            int argc_env = static_cast<int>(argv_env.size());
            const char** argv_env_ptr = argv_env.data();

            env_parser.parse(argc_env, argv_env_ptr);

            // Apply direction shorthand flags from environment
            apply_direction_flags(*this);

            // Normalize direction aliases (e.g., "c" -> ",")
            direction = normalize_direction(direction);

            // Validate direction from environment immediately
            // If invalid, warn and reset logic (optional, or just keep invalid and let CLI fail later)
            // Here we warn and continue as requested.
            if (!direction.empty() && !is_valid_direction(direction))
            {
                std::cerr << "Warning: Invalid direction in environment variable " << environment_variable_name //
                          << ": \"" << direction << "\". Ignoring." << std::endl;
                direction = direction_default; // reset direction
            }
        }
        catch (const cxxopts::exceptions::exception& e)
        {
            // Requirement: "If there are any errors... print a warning to stderr and continue"
            std::cerr << "Warning: Failed to parse environment variable " << environment_variable_name << ": " << e.what() << std::endl;
        }
    }

    //-------------------------------------------------------------------------
    // 2. Parse Command Line Arguments (Full set of options)
    //-------------------------------------------------------------------------

    // The program name is the first argument
    std::string program_name = args[0];
    cxxopts::Options cmd_parser(program_name, " - A Change Directory Utility");
    add_full_options(*this, cmd_parser);

    // Special preprocessing for custom dash parameters (same as original logic)
    std::vector<std::string> preprocessed_args;
    std::vector<std::string> special_dashed_args;

    // Ensure program name is in preprocessed args
    preprocessed_args.push_back(args[0]);

    for (size_t i = 1; i < args.size(); ++i)
    {
        const auto& arg = args[i];
        if (is_special_dash_parameter(arg))
        {
            special_dashed_args.push_back(arg);
        }
        else
        {
            preprocessed_args.push_back(arg);
        }
    }

    // Convert vector<string> back to (argc, argv) for cxxopts
    std::vector<const char*> argv_c;
    for (const auto& arg : preprocessed_args)
    {
        argv_c.push_back(arg.c_str());
    }
    int argc_c = static_cast<int>(argv_c.size());
    const char** argv_ptr = argv_c.data();

    try
    {
        auto result = cmd_parser.parse(argc_c, argv_ptr);

        if (show_help)
        {
            std::cerr << "Usage: " << program_name << " [options]" << std::endl;
            std::cerr << cmd_parser.help() << std::endl;
            output_footer();
            return true;
        }

        if (show_version)
        {
            std::cerr << "cd-deluxe version " << get_cdd_version() << std::endl;
            return true;
        }

        if (result.count("init"))
        {
            // need to send output the stderr in case this is being run under the cdd shell function
            CddOptionsInit options_init(std::cerr);
            options_init.parse(argc_c, const_cast<char**>(argv_ptr), /* force_default_setup */ true);
            return false; // signal to caller that no further action is needed
        }

        // Apply direction shorthand flags (--dc, --common, etc.)
        apply_direction_flags(*this);

        // Normalize direction aliases (e.g., "c" -> ",")
        direction = normalize_direction(direction);

        // Validate direction (Fatal error if invalid here)
        if (!direction.empty() && !is_valid_direction(direction))
        {
            set_error("Invalid direction: \"" + direction + "\". Valid directions are: " + get_valid_directions_as_string());
            return false;
        }

        unmatched_args = result.unmatched();
        if (!special_dashed_args.empty())
        {
            // insert at front of unmatched_args due to special meaning
            unmatched_args.insert(unmatched_args.begin(), special_dashed_args.begin(), special_dashed_args.end());
        }

        if (delete_entry)
        {
            if (unmatched_args.empty())
            {
                set_error("Delete option requires an argument (the entry to delete).");
                return false;
            }
        }
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        // Requirement: "Any options parsing errors are considered to be errors."
        set_error(e.what());
        return false;
    }

    // allow for shorthand specification of the direction using symbols (+, -, ,, ..)
    // Note: letter aliases (b, f, c, u) are NOT allowed here as they conflict with search patterns
    if (!unmatched_args.empty() && is_symbol_direction(unmatched_args[0]))
    {
        if (unmatched_args.size() > 1 || list_history || use_fzf)
        {
            // shorthand format
            direction = unmatched_args[0];
            unmatched_args.erase(unmatched_args.begin());
        }
    }

    // check again
    if (unmatched_args.size() > 1)
    {
        // too many unmatched args
        std::stringstream strm;
        strm << "Ignoring extra arguments:";
        for (size_t i = 1; i < unmatched_args.size(); ++i)
        {
            strm << " \"" << unmatched_args[i] << "\"";
        }
        set_error(strm.str());
    }

    return true;
}

void CddOptions::output(std::ostream& os) const
{
    os << "CddOptions:" << std::endl;
    os << "  show_help: " << std::boolalpha << show_help << std::endl;
    os << "  show_version: " << std::boolalpha << show_version << std::endl;
    os << "  list_history: " << std::boolalpha << list_history << std::endl;
    os << "  direction: " << direction << std::endl;
    os << "  max_history: " << max_history << std::endl;
    os << "  max_backwards: " << max_backwards << std::endl;
    os << "  max_forwards: " << max_forwards << std::endl;
    os << "  max_common: " << max_common << std::endl;
    os << "  max_upwards: " << max_upwards << std::endl;
    os << "  all_history: " << std::boolalpha << all_history << std::endl;
    os << "  ignore_case: " << std::boolalpha << ignore_case << std::endl;
    os << "  delete_entry: " << std::boolalpha << delete_entry << std::endl;
    os << "  reset_history: " << std::boolalpha << reset_history << std::endl;
    os << "  garbage_collect: " << std::boolalpha << garbage_collect << std::endl;
    os << "  use_fzf: " << std::boolalpha << use_fzf << std::endl;

    os << "  unmatched_args: [";
    for (size_t i = 0; i < unmatched_args.size(); ++i)
    {
        os << "\"" << unmatched_args[i] << "\"";
        if (i < unmatched_args.size() - 1)
        {
            os << ", ";
        }
    }
    os << "]" << std::endl;

    os << "  has_error: " << std::boolalpha << has_error() << std::endl;
    os << "  error_message: " << error_message << std::endl;
}
