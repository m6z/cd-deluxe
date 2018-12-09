/*

Copyright 2010-2018 Michael Graz
http://www.plan10.com/cdd

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

#include "stdafx.h"

#include "catch.hpp"

#define countof(x) (sizeof(x)/sizeof(x[0]))

TEST_CASE("options_test")
{

SECTION("options_default")
{
    Cdd cdd;
    const char *av[] = {"_cdd"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE(false == cdd.direction.is_assigned());
    REQUIRE("-" == cdd.direction._direction);
}

//----------------------------------------------------------------------
// Explicit named options

SECTION("options_help")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--help"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_help);
}

SECTION("options_version")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--version"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_version);
}

SECTION("options_path")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("abc" == cdd.opt_path);
}

SECTION("options_direction_plus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--direction=+"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.direction._direction);
}

SECTION("options_direction_minus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--direction", "-"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-" == cdd.direction._direction);
}

SECTION("options_direction_error")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "z"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == rc);
    REQUIRE(
        "** Options error: Unknown direction 'z'\n"
        "Use --help to see possible options\n" ==
        cdd.strm_err.str());
}

SECTION("options_direction_minus_and_qmark")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "-", "?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("-" == cdd.direction._direction);
}

SECTION("options_direction_minus_and_double_qmark")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "-", "??"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
}

SECTION("options_history")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
}

SECTION("options_limit_backwards")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--limit-backwards=3"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(3 == cdd.opt_limit_backwards);
}

SECTION("options_limit_forwards")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--limit-forwards", "7", "--history"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(7 == cdd.opt_limit_forwards);
}

SECTION("options_limit_common")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--limit-common", "5", "--history"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(5 == cdd.opt_limit_common);
}

SECTION("options_limit_all")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--all"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_all);
}

SECTION("options_limit_all_override")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--all", ",?", "5"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE(5 == cdd.opt_limit_common);
    REQUIRE(false == cdd.opt_all);
}

SECTION("options_path_equals_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=1"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("1" == cdd.opt_path);
}

SECTION("options_path_equals_minus_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=-1"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-1" == cdd.opt_path);
}

SECTION("options_path_equals_plus_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=+1"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+1" == cdd.opt_path);
}

//----------------------------------------------------------------------
// Freeform (positional) options

SECTION("freeform_path_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("abc" == cdd.opt_path);
}

SECTION("freeform_zero")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "0"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("0" == cdd.opt_path);
    REQUIRE("+" == cdd.direction._direction);
}

SECTION("freeform_path_1_dash")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-" == cdd.opt_path);
}

SECTION("freeform_path_minus_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-1"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-1" == cdd.opt_path);
}

SECTION("freeform_path_1_plus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.opt_path);
}

SECTION("freeform_path_plus_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+1"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+1" == cdd.opt_path);
}

SECTION("freeform_path_2_dash")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("--" == cdd.opt_path);
}

SECTION("freeform_path_minus_2")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-2"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-2" == cdd.opt_path);
}

SECTION("freeform_path_2_plus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "++"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("++" == cdd.opt_path);
}

SECTION("freeform_path_plus_2")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+2"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+2" == cdd.opt_path);
}

SECTION("freeform_path_3_dash")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "---"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("---" == cdd.opt_path);
}

SECTION("freeform_path_minus_3")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-3"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-3" == cdd.opt_path);
}

SECTION("freeform_path_3_plus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+++"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+++" == cdd.opt_path);
}

SECTION("freeform_path_plus_3")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+3"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+3" == cdd.opt_path);
}

SECTION("freeform_direction_and_path_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+", "abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE("abc" == cdd.opt_path);
}

SECTION("freeform_direction_and_path_2")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-", "abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("-" == cdd.direction._direction);
    REQUIRE("abc" == cdd.opt_path);
}

SECTION("freeform_history")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("+" == cdd.direction._direction);
}

SECTION("freeform_history_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "?", "4"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE(4 == cdd.opt_limit_forwards);
}

SECTION("freeform_history_plus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("+" == cdd.direction._direction);
    // this is the default
    REQUIRE(0 == cdd.opt_limit_forwards);
}

SECTION("freeform_history_plus_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+?", "6"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE(6 == cdd.opt_limit_forwards);
}

SECTION("freeform_history_minus")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("-" == cdd.direction._direction);
    // this is the default
    REQUIRE(10 == cdd.opt_limit_backwards);
}

SECTION("freeform_history_minus_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?", "9"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("-" == cdd.direction._direction);
    REQUIRE(9 == cdd.opt_limit_backwards);
}

SECTION("freeform_history_comma")
{
    Cdd cdd;
    const char *av[] = {"_cdd", ",?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
    // this is the default
    REQUIRE(10 == cdd.opt_limit_common);
}

SECTION("freeform_history_comma_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", ",?", "4"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
    REQUIRE(4 == cdd.opt_limit_common);
}

SECTION("freeform_history_double_qmark")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "??"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
}

SECTION("freeform_history_double_qmark_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "??", "5"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
    REQUIRE(5 == cdd.opt_limit_common);
}

SECTION("freeform_history_error")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?", "abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == rc);
    REQUIRE(
        "** Options error: expecting number for second option: -? abc\n"
        "Use --help to see possible options\n" ==
        cdd.strm_err.str());
}

SECTION("freeform_error_2")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == rc);
    REQUIRE(
        "** Options error: unable to interpret options\n"
        "Use --help to see possible options\n" ==
        cdd.strm_err.str());
}

SECTION("freeform_error_3")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b", "c"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == rc);
    REQUIRE(
        "** Options error: too many options specified\n"
        "Use --help to see possible options\n" ==
        cdd.strm_err.str());
}

SECTION("freeform_error_4")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b", "c", "d"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == rc);
    REQUIRE(
        "** Options error: too many options specified\n"
        "Use --help to see possible options\n" ==
        cdd.strm_err.str());
}

//----------------------------------------------------------------------

SECTION("action_history")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action", ",?"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("," == cdd.direction._direction);
}

SECTION("action_history_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action", "? 4"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE(4 == cdd.opt_limit_forwards);
}

SECTION("action_direction_and_path")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action=+ abc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE("abc" == cdd.opt_path);
}

//----------------------------------------------------------------------

SECTION("env_limit")
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    bool rc = cdd.options(countof(av), av, "--limit-backwards=2 --limit-forwards=3");
    REQUIRE(true == rc);
    REQUIRE(2 == cdd.opt_limit_backwards);
    REQUIRE(3 == cdd.opt_limit_forwards);
}

SECTION("env_all")
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    bool rc = cdd.options(countof(av), av, "--all");
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_all);
}

SECTION("env_direction")
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    bool rc = cdd.options(countof(av), av, "--direction=+");
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.direction._direction);
}

SECTION("env_action")
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    bool rc = cdd.options(countof(av), av, "--action=0");
    REQUIRE(true == rc);
    REQUIRE("0" == cdd.opt_path);
    REQUIRE(false == cdd.opt_history);
}

SECTION("env_action_override_1")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history"};
    bool rc = cdd.options(countof(av), av, "--action=0");
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_history);
}

SECTION("env_action_override_2")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "+", "abc"};
    bool rc = cdd.options(countof(av), av, "--action=0");
    REQUIRE(true == rc);
    REQUIRE("+" == cdd.direction._direction);
    REQUIRE("abc" == cdd.opt_path);
}

//----------------------------------------------------------------------

SECTION("garbage_collect")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--gc"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_gc);
}

SECTION("del_none")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--del"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(false == cdd.opt_delete);
    REQUIRE(false == rc);
    REQUIRE(
        "** oopsie\n"
        "end\n" ==
        cdd.strm_err.str());
}

SECTION("del_one")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--del", "--path", "5"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_delete);
    REQUIRE("5" == cdd.opt_path);
}

SECTION("delete_one")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--delete", "--path", "5"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_delete);
    REQUIRE("5" == cdd.opt_path);
}

SECTION("reset")
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--reset"};
    bool rc = cdd.options(countof(av), av);
    REQUIRE(true == rc);
    REQUIRE(true == cdd.opt_reset);
}

}

// vim:ff=unix
