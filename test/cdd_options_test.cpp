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
            // check if error starts with expected message:
            REQUIRE(options.error_message.find("** Invalid direction: \"%\"") == 0);
        }

        // Not implementing default action for now
        // // command line, long opts
        // {
        //     CddOptions options({"./_cdd", "--default", "cdd ,0", "--direction=,"});
        //     REQUIRE(options.default_action == "cdd ,0");
        //     REQUIRE(options.direction == ",");
        // }

        // command line, short opts
        {
            CddOptions options({"./_cdd", "-d", "-"});
            REQUIRE(options.direction == "-");
        }

        // env opts, long opts
        {
            std::string env_opts = "--direction=+";
            CddOptions options({"./_cdd"}, env_opts);
            REQUIRE(options.direction == "+");
        }
    }

    SECTION("multiple path spec")
    {
        CddOptions options({"./_cdd", "-", "abc"});
        REQUIRE(options.direction == "-");
        REQUIRE(options.unmatched_args.size() == 1);
        REQUIRE(options.unmatched_args[0] == "abc");
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

    SECTION("cleanup_options")
    {
        {
            // missing spec
            CddOptions options({"./_cdd", "--del"});
            REQUIRE(options.error_message == "** Delete option requires an argument (the entry to delete).");
        }

        {
            // short form
            CddOptions options({"./_cdd", "--del", "abc"});
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "abc");
        }
    }

    SECTION("warnings")
    {
        {
            CddOptions options({"./_cdd", "abc", "def"});
            REQUIRE(options.error_message == "** Ignoring extra arguments: \"def\"");
        }
    }

    SECTION("shorthand_backwards")
    {
        {
            CddOptions options({"./_cdd", "-", "-l"});
            REQUIRE(options.direction == "-");
            REQUIRE(options.list_history);
        }

        {
            CddOptions options({"./_cdd", "-f", "-"});
            REQUIRE(options.direction == "-");
            REQUIRE(options.use_fzf);
        }
    }

    SECTION("shorthand_forwards")
    {
        {
            CddOptions options({"./_cdd", "+", "--list"});
            REQUIRE(options.direction == "+");
            REQUIRE(options.list_history);
        }

        {
            CddOptions options({"./_cdd", "--fzf", "+"});
            REQUIRE(options.direction == "+");
            REQUIRE(options.use_fzf);
        }
    }

    SECTION("shorthand_common_upwards")
    {
        {
            CddOptions options({"./_cdd", ",", "--list"});
            REQUIRE(options.direction == ",");
            REQUIRE(options.list_history);
        }

        {
            CddOptions options({"./_cdd", "-f", ","});
            REQUIRE(options.direction == ",");
            REQUIRE(options.use_fzf);
        }
    }

    SECTION("shorthand_upwards")
    {
        {
            CddOptions options({"./_cdd", "..", "-l"});
            REQUIRE(options.direction == "..");
            REQUIRE(options.list_history);
        }

        {
            CddOptions options({"./_cdd", "-f", ".."});
            REQUIRE(options.direction == "..");
            REQUIRE(options.use_fzf);
        }
    }

    SECTION("direction_letter_aliases")
    {
        // Test --direction with letter aliases (PowerShell-friendly)
        SECTION("direction=f for forwards")
        {
            CddOptions options({"./_cdd", "--direction=f"});
            REQUIRE(options.direction == "+");
        }

        SECTION("direction=b for backwards")
        {
            CddOptions options({"./_cdd", "--direction=b"});
            REQUIRE(options.direction == "-");
        }

        SECTION("direction=c for common")
        {
            CddOptions options({"./_cdd", "--direction=c"});
            REQUIRE(options.direction == ",");
        }

        SECTION("direction=u for upwards")
        {
            CddOptions options({"./_cdd", "--direction=u"});
            REQUIRE(options.direction == "..");
        }

        SECTION("-d f for forwards")
        {
            CddOptions options({"./_cdd", "-d", "f"});
            REQUIRE(options.direction == "+");
        }

        SECTION("-d b for backwards")
        {
            CddOptions options({"./_cdd", "-d", "b"});
            REQUIRE(options.direction == "-");
        }

        SECTION("-d c for common")
        {
            CddOptions options({"./_cdd", "-d", "c"});
            REQUIRE(options.direction == ",");
        }

        SECTION("-d u for upwards")
        {
            CddOptions options({"./_cdd", "-d", "u"});
            REQUIRE(options.direction == "..");
        }
    }

    SECTION("direction_shorthand_flags")
    {
        // Test --db, --df, --dc, --du shorthand flags
        SECTION("--db for backwards")
        {
            CddOptions options({"./_cdd", "--db"});
            REQUIRE(options.direction == "-");
        }

        SECTION("--df for forwards")
        {
            CddOptions options({"./_cdd", "--df"});
            REQUIRE(options.direction == "+");
        }

        SECTION("--dc for common")
        {
            CddOptions options({"./_cdd", "--dc"});
            REQUIRE(options.direction == ",");
        }

        SECTION("--du for upwards")
        {
            CddOptions options({"./_cdd", "--du"});
            REQUIRE(options.direction == "..");
        }

        SECTION("--db with --list")
        {
            CddOptions options({"./_cdd", "--db", "--list"});
            REQUIRE(options.direction == "-");
            REQUIRE(options.list_history);
        }

        SECTION("--df with --list")
        {
            CddOptions options({"./_cdd", "--df", "--list"});
            REQUIRE(options.direction == "+");
            REQUIRE(options.list_history);
        }

        SECTION("--dc with --list")
        {
            CddOptions options({"./_cdd", "--dc", "--list"});
            REQUIRE(options.direction == ",");
            REQUIRE(options.list_history);
        }

        SECTION("--du with --list")
        {
            CddOptions options({"./_cdd", "--du", "--list"});
            REQUIRE(options.direction == "..");
            REQUIRE(options.list_history);
        }
    }

    SECTION("common_option")
    {
        // Test -c / --common option
        SECTION("-c sets direction to common")
        {
            CddOptions options({"./_cdd", "-c"});
            REQUIRE(options.direction == ",");
        }

        SECTION("--common sets direction to common")
        {
            CddOptions options({"./_cdd", "--common"});
            REQUIRE(options.direction == ",");
        }

        SECTION("-c with --list")
        {
            CddOptions options({"./_cdd", "-c", "--list"});
            REQUIRE(options.direction == ",");
            REQUIRE(options.list_history);
        }

        SECTION("--common with -l")
        {
            CddOptions options({"./_cdd", "--common", "-l"});
            REQUIRE(options.direction == ",");
            REQUIRE(options.list_history);
        }
    }

    SECTION("letter_aliases_not_positional")
    {
        // Letter aliases (b, f, c, u) should NOT work as positional shortcuts
        // They should be treated as search patterns, not directions
        SECTION("b as pattern not direction")
        {
            CddOptions options({"./_cdd", "--list", "b"});
            REQUIRE(options.direction == "-"); // default direction
            REQUIRE(options.list_history);
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "b");
        }

        SECTION("f as pattern not direction")
        {
            CddOptions options({"./_cdd", "--list", "f"});
            REQUIRE(options.direction == "-"); // default direction
            REQUIRE(options.list_history);
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "f");
        }

        SECTION("c as pattern not direction")
        {
            CddOptions options({"./_cdd", "--list", "c"});
            REQUIRE(options.direction == "-"); // default direction
            REQUIRE(options.list_history);
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "c");
        }

        SECTION("u as pattern not direction")
        {
            CddOptions options({"./_cdd", "--list", "u"});
            REQUIRE(options.direction == "-"); // default direction
            REQUIRE(options.list_history);
            REQUIRE(options.unmatched_args.size() == 1);
            REQUIRE(options.unmatched_args[0] == "u");
        }
    }
}
