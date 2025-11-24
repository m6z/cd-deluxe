#include "cdd_options.h"

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
           s == CddOptions::direction_common;      //
}

std::string get_valid_directions_as_string()
{
    return "\"" + std::string(CddOptions::direction_forwards) + "\" (forwards), \"" + //
           std::string(CddOptions::direction_backwards) + "\" (backwards), \"" +      //
           std::string(CddOptions::direction_common) + "\" (common)";                 //
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
        error_message = "Argument vector cannot be empty.";
        return false;
    }

    // The program name is the first argument
    std::string program_name = args[0];
    cxxopts::Options options(program_name, " - A C++ Deluxe CD-ROM cataloger");

    // do special preprocessing since cxxopts does not handle '--' or longer as a standalone argument
    std::vector<std::string> preprocessed_args;
    std::vector<std::string> special_dashed_args;
    for (const auto& arg : args)
    {
        if (is_special_dash_parameter(arg))
        {
            special_dashed_args.push_back(arg);
        }
        else
        {
            preprocessed_args.push_back(arg);
        }
    }

    // 1. Define all the command-line options we accept
    // Allow arguments that don't match any defined option
    // options.allow_unrecognised_options();

    options.add_options()                                                                          //
        ("h,help", "Show this help message", cxxopts::value(show_help)->implicit_value("true"))    //
        ("l,list", "List history", cxxopts::value(list_history)->implicit_value("true"))           //
        ("default", "Default action (intended for CDD_OPTIONS)", cxxopts::value(default_action))   //
        ("d,direction", "Default direction (intended for CDD_OPTIONS)", cxxopts::value(direction)) //
        ("i,ignore-case", "Ignore case when comparing paths", cxxopts::value(ignore_case))         //

        ("m,max", "Max history", cxxopts::value(max_history))                              //
        ("max-backwards", "Max backwards history", cxxopts::value(max_backwards))          //
        ("max-forwards", "Max forwards history", cxxopts::value(max_forwards))             //
        ("max-common", "Max common history", cxxopts::value(max_common))                   //
        ("max-upwards", "Max upwards history", cxxopts::value(max_upwards))                //
        ("a,all", "Show all history", cxxopts::value(all_history)->implicit_value("true")) //
        ;

    // 2. Combine environment options and command-line arguments
    auto combined_args = combine_arguments(preprocessed_args, env_options);

    // 3. Convert vector<string> back to (argc, argv) for cxxopts
    std::vector<const char*> argv_c;
    for (const auto& arg : combined_args)
    {
        argv_c.push_back(arg.c_str());
    }
    int argc_c = static_cast<int>(argv_c.size());
    const char** argv_ptr = argv_c.data();

    // 4. Parse the options, catching any errors
    try
    {
        auto result = options.parse(argc_c, argv_ptr);
        if (show_help)
        {
            std::cerr << "Usage: " << program_name << " [options]" << std::endl;
            std::cerr << options.help() << std::endl;
            // Help was requested; no further processing needed
            return true;
        }

        if (!direction.empty() && !is_valid_direction(direction))
        {
            error_message = "Invalid direction: \"" + direction + "\". Valid directions are: " + get_valid_directions_as_string();
            return false;
        }

        unmatched_args = result.unmatched();
        if (!special_dashed_args.empty())
        {
            // insert at front of unmatched_args due to special meaning
            unmatched_args.insert(unmatched_args.begin(), special_dashed_args.begin(), special_dashed_args.end());
        }
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        error_message = e.what();
        return false;
    }

    return true;
}

std::vector<std::string> CddOptions::combine_arguments(const std::vector<std::string>& args, const std::string& env_options)
{
    std::vector<std::string> combined;
    // First, add the program name
    combined.push_back(args[0]);

    // Second, add the parsed environment options
    if (!env_options.empty())
    {
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
            combined.push_back(field);
        }
    }

    // Finally, add the original command-line arguments (skipping program name)
    combined.insert(combined.end(), args.begin() + 1, args.end());

    return combined;
}

void CddOptions::output(std::ostream& os) const
{
    os << "CddOptions:" << std::endl;
    os << "  show_help: " << std::boolalpha << show_help << std::endl;
    os << "  list_history: " << std::boolalpha << list_history << std::endl;
    os << "  default_action: " << default_action << std::endl;
    os << "  direction: " << direction << std::endl;
    os << "  max_history: " << max_history << std::endl;
    os << "  max_backwards: " << max_backwards << std::endl;
    os << "  max_forwards: " << max_forwards << std::endl;
    os << "  max_common: " << max_common << std::endl;
    os << "  max_upwards: " << max_upwards << std::endl;
    os << "  all_history: " << std::boolalpha << all_history << std::endl;
    os << "  ignore_case: " << std::boolalpha << ignore_case << std::endl;

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

    os << "  has_error: " << std::boolalpha << has_error << std::endl;
    os << "  error_message: " << error_message << std::endl;
}
