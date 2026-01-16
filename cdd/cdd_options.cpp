#include "cdd_options.h"
#include "cdd_options_init.h"

#include "cxxopts.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "cdd_util.h"

namespace
{

bool is_valid_direction(const std::string& s)
{
    return s == CddOptions::direction_forwards ||  //
           s == CddOptions::direction_backwards || //
           s == CddOptions::direction_common ||    //
           s == CddOptions::direction_upwards;     //
}

std::string get_valid_directions_as_string()
{
    return "\"" + std::string(CddOptions::direction_forwards) + "\" (forwards), \"" + //
           std::string(CddOptions::direction_backwards) + "\" (backwards), \"" +      //
           std::string(CddOptions::direction_common) + "\" (common), \"" +            //
           std::string(CddOptions::direction_upwards) + "\" (upwards)";               //
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
        char nextChar = iss.peek();
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

// Helper to define the subset of options allowed in the environment variable
void add_common_options(CddOptions& x, cxxopts::Options& options)
{
    options.add_options()                                                                                            //
        ("l,list", "List history", cxxopts::value(x.list_history)->implicit_value("true"))                           //
        ("f,fzf", "Filter history with fzf", cxxopts::value(x.use_fzf)->implicit_value("true"))                      //
        ("d,direction", get_valid_directions_as_string(), cxxopts::value(x.direction))                               //
        ("m,max", "Max history", cxxopts::value(x.max_history))                                                      //
        ("max-backwards", "Max backwards history", cxxopts::value(x.max_backwards))                                  //
        ("max-forwards", "Max forwards history", cxxopts::value(x.max_forwards))                                     //
        ("max-common", "Max common history", cxxopts::value(x.max_common))                                           //
        ("max-upwards", "Max upwards history", cxxopts::value(x.max_upwards))                                        //
        ("a,all", "Show all history", cxxopts::value(x.all_history)->implicit_value("true"))                         //
        ("i,ignore-case", "Ignore case when comparing paths", cxxopts::value(x.ignore_case)->implicit_value("true")) //
        ("init", "Generate shell initialization code")                                                               //
        ;
}

// Helper to define the full set of options (includes common options)
void add_full_options(CddOptions& x, cxxopts::Options& options)
{
    add_common_options(x, options);

    options.add_options()                                                                                            //
        ("h,help", "Show this help message", cxxopts::value(x.show_help)->implicit_value("true"))                    //
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
#ifdef _WIN32
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

        if (result.count("init"))
        {
            // need to send output the stderr in case this is being run under the cdd shell function
            CddOptionsInit options_init(std::cerr);
            options_init.parse(argc_c, const_cast<char**>(argv_ptr), /* force_default_setup */ true);
            return false; // signal to caller that no further action is needed
        }

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

    // allow for shorthand specification of the direction
    if (!unmatched_args.empty() && is_valid_direction(unmatched_args[0]))
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
