#include "cdd_options.h"

#include "cxxopts.hpp"

#include <iomanip> // For std::quoted
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

CddOptions::CddOptions(const std::vector<std::string>& args, const std::string& env_options)
{
#ifdef _WIN32
    ignore_case = true;
#endif

    if (args.empty())
    {
        parse_error = true;
        error_message = "Argument vector cannot be empty.";
        return;
    }

    // The program name is the first argument
    std::string program_name = args[0];
    cxxopts::Options options(program_name, " - A C++ Deluxe CD-ROM cataloger");

    // do special preprocessing since cxxopts does not handle '--' as a standalone argument
    bool double_dash_present = false;
    std::vector<std::string> preprocessed_args;
    for (const auto& arg : args)
    {
        if (arg == "--")
        {
            double_dash_present = true;
        }
        else
        {
            preprocessed_args.push_back(arg);
        }
    }

    // 1. Define all the command-line options we accept
    // Allow arguments that don't match any defined option
    options.allow_unrecognised_options();

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

    // Store the generated help text for later use
    help_text = options.help();

    // 2. Combine environment options and command-line arguments
    auto combined_args = combine_arguments(args, env_options);

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
        unmatched_args = result.unmatched();
        if (double_dash_present)
        {
            // insert at front of unmatched_args due to special meaning
            unmatched_args.insert(unmatched_args.begin(), "--");
        }
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        parse_error = true;
        error_message = e.what();
    }
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
