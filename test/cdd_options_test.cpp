// This tells Catch2 to provide its own main() function
#define CATCH_CONFIG_MAIN

#include <catch2/catch_test_macros.hpp>
#include <initializer_list>
#include <string>
#include <vector>

#include "cdd/cdd_options.h"

TEST_CASE("cdd_options_test", "[options]")
{
    SECTION("Default state with no arguments")
    {
        CddOptions options({"./_cdd"});
        REQUIRE_FALSE(options.has_error);
        REQUIRE_FALSE(options.show_help);
        REQUIRE_FALSE(options.list_history);
        REQUIRE(options.unmatched_args.empty());
    }

    SECTION("Help flag is detected")
    {
        SECTION("Long form --help")
        {
            CddOptions options({"./_cdd", "--help"});
            REQUIRE(options.show_help);
            REQUIRE_FALSE(options.list_history);
        }
        SECTION("Short form -h")
        {
            CddOptions options({"./_cdd", "-h"});
            REQUIRE(options.show_help);
            REQUIRE_FALSE(options.list_history);
        }
    }

    SECTION("List flag is detected")
    {
        SECTION("Long form --list")
        {
            CddOptions options({"./_cdd", "--list"});
            REQUIRE(options.list_history);
            REQUIRE_FALSE(options.show_help);
        }
        SECTION("Short form -l")
        {
            CddOptions options({"./_cdd", "-l"});
            REQUIRE(options.list_history);
            REQUIRE_FALSE(options.show_help);
        }
    }

    SECTION("Unmatched arguments are collected")
    {
        CddOptions options({"./_cdd", "unmatched1", "--list", "unmatched2"});
        REQUIRE(options.list_history);
        REQUIRE(options.unmatched_args.size() == 2);
        REQUIRE(options.unmatched_args[0] == "unmatched1");
        REQUIRE(options.unmatched_args[1] == "unmatched2");
    }

    SECTION("Environment options are processed")
    {
        std::string env_opts = "--list"; // Simulate an environment variable
        CddOptions options({"./_cdd", "command_line_arg"}, env_opts);

        REQUIRE(options.list_history); // The --list from env_opts should be parsed
        REQUIRE_FALSE(options.show_help);
        REQUIRE(options.unmatched_args.size() == 1);
        REQUIRE(options.unmatched_args[0] == "command_line_arg");
    }

    // TODO is this important?
    // SECTION("Error handling for empty argument vector")
    // {
    //     // This is a rare case, but the constructor should handle it gracefully
    //     CddOptions options();
    //     REQUIRE(options.has_error);
    //     REQUIRE_FALSE(options.error_message.empty());
    // }

    SECTION("Max history length (temp)")
    {
        CddOptions options({"./_cdd", "--max-backwards=5", "--max-forwards", "7", "--max-common=3", "--max-upwards=4"});
        REQUIRE(options.max_backwards == 5);
        REQUIRE(options.max_forwards == 7);
        REQUIRE(options.max_common == 3);
        REQUIRE(options.max_upwards == 4);
    }

    SECTION("Max history length")
    {
        {
            CddOptions options({"./_cdd", "--max=11"});
            REQUIRE(options.max_history == 11);
        }

        {
            CddOptions options({"./_cdd", "--max", "12"});
            REQUIRE(options.max_history == 12);
        }

        // will not work
        // {
        //     CddOptions options({"./_cdd", "-m=13"});
        //     REQUIRE(options.max_history == 13);
        // }

        {
            CddOptions options({"./_cdd", "-m", "14"});
            REQUIRE(options.max_history == 14);
        }
    }

    SECTION("action_and_direction")
    {
        // Bad direction
        {
            CddOptions options({"./_cdd", "--list", "--direction=%"});
            REQUIRE(options.has_error);
            // check if error starts with expected message:
            REQUIRE(options.error_message.find("Invalid direction: \"%\"") == 0);
        }

        // command line, long opts
        {
            CddOptions options({"./_cdd", "--default", "cdd ,0", "--direction=,"});
            REQUIRE(options.default_action == "cdd ,0");
            REQUIRE(options.direction == ",");
        }

        // command line, short opts
        {
            CddOptions options({"./_cdd", "-d", "-"});
            REQUIRE(options.direction == "-");
        }

        // env opts, long opts
        {
            std::string env_opts = "--default \"cdd ,2\" --direction=+";
            CddOptions options({"./_cdd"}, env_opts);
            REQUIRE(options.default_action == "cdd ,2");
            REQUIRE(options.direction == "+");
        }
    }

    SECTION("multiple path spec")
    {
        CddOptions options({"./_cdd", "-", "abc"});
        REQUIRE(options.unmatched_args.size() == 2);
        REQUIRE(options.unmatched_args[0] == "-");
        REQUIRE(options.unmatched_args[1] == "abc");
    }

    SECTION("dashes")
    {
        {
            CddOptions options({"./_cdd", "-"});
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "-");
        }

        {
            CddOptions options({"./_cdd", "--"});
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "--");
        }

        {
            CddOptions options({"./_cdd", "---"});
            options.output();
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "---");
        }
    }
}
