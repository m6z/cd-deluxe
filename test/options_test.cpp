/*

Copyright 2010-2011 Michael Graz
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

#include <boost/test/unit_test.hpp>

#define countof(x) (sizeof(x)/sizeof(x[0]))

BOOST_AUTO_TEST_SUITE(options_test)

BOOST_AUTO_TEST_CASE(options_default)
{
    Cdd cdd;
    const char *av[] = {"_cdd"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(false, cdd.direction.is_assigned());
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
}

//----------------------------------------------------------------------
// Explicit named options

BOOST_AUTO_TEST_CASE(options_help)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--help"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_help);
}

BOOST_AUTO_TEST_CASE(options_version)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--version"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_version);
}

BOOST_AUTO_TEST_CASE(options_path)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(options_direction_plus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--direction=+"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(options_direction_minus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--direction", "-"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(options_direction_error)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "z"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(false, rc);
    BOOST_REQUIRE_EQUAL(
        "** Options error: in option 'direction': invalid option value\n"
        "Use --help to see possible options\n",
        cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(options_direction_minus_and_qmark)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "-", "?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(options_direction_minus_and_double_qmark)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--direction", "-", "??"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(options_history)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
}

BOOST_AUTO_TEST_CASE(options_limit_backwards)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--limit-backwards=3"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(3, cdd.opt_limit_backwards);
}

BOOST_AUTO_TEST_CASE(options_limit_forwards)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--limit-forwards", "7", "--history"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(7, cdd.opt_limit_forwards);
}

BOOST_AUTO_TEST_CASE(options_limit_common)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--limit-common", "5", "--history"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(5, cdd.opt_limit_common);
}

BOOST_AUTO_TEST_CASE(options_limit_all)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "--all"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_all);
}

BOOST_AUTO_TEST_CASE(options_limit_all_override)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--all", ",?", "5"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(5, cdd.opt_limit_common);
    BOOST_REQUIRE_EQUAL(false, cdd.opt_all);
}

BOOST_AUTO_TEST_CASE(options_path_equals_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=1"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("1", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(options_path_equals_minus_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=-1"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-1", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(options_path_equals_plus_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--path=+1"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+1", cdd.opt_path);
}

//----------------------------------------------------------------------
// Freeform (positional) options

BOOST_AUTO_TEST_CASE(freeform_path_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_zero)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "0"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("0", cdd.opt_path);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_path_1_dash)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_minus_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-1"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-1", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_1_plus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_plus_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+1"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+1", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_2_dash)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("--", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_minus_2)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-2"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-2", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_2_plus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "++"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("++", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_plus_2)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+2"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+2", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_3_dash)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "---"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("---", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_minus_3)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-3"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-3", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_3_plus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+++"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+++", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_path_plus_3)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+3"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+3", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_direction_and_path_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+", "abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_direction_and_path_2)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-", "abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(freeform_history)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_history_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "?", "4"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(4, cdd.opt_limit_forwards);
}

BOOST_AUTO_TEST_CASE(freeform_history_plus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_history_plus_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "+?", "6"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(6, cdd.opt_limit_forwards);
}

BOOST_AUTO_TEST_CASE(freeform_history_minus)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_history_minus_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?", "8"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("-", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(8, cdd.opt_limit_backwards);
}

BOOST_AUTO_TEST_CASE(freeform_history_comma)
{
    Cdd cdd;
    const char *av[] = {"_cdd", ",?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_history_comma_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", ",?", "4"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(4, cdd.opt_limit_common);
}

BOOST_AUTO_TEST_CASE(freeform_history_double_qmark)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "??"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(freeform_history_double_qmark_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "??", "5"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(5, cdd.opt_limit_common);
}

BOOST_AUTO_TEST_CASE(freeform_history_error)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "-?", "abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(false, rc);
    BOOST_REQUIRE_EQUAL(
        "** Options error: expecting number for second option: -? abc\n"
        "Use --help to see possible options\n",
        cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(freeform_error_2)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(false, rc);
    BOOST_REQUIRE_EQUAL(
        "** Options error: unable to interpret options\n"
        "Use --help to see possible options\n",
        cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(freeform_error_3)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b", "c"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(false, rc);
    BOOST_REQUIRE_EQUAL(
        "** Options error: too many options specified\n"
        "Use --help to see possible options\n",
        cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(freeform_error_4)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "a", "b", "c", "d"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(false, rc);
    BOOST_REQUIRE_EQUAL(
        "** Options error: too many options specified\n"
        "Use --help to see possible options\n",
        cdd.strm_err.str());
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(action_history)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action", ",?"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL(",", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(action_history_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action", "? 4"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL(4, cdd.opt_limit_forwards);
}

BOOST_AUTO_TEST_CASE(action_direction_and_path)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--action=+ abc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(env_limit)
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    int rc = cdd.options(countof(av), av, "--limit-backwards=2 --limit-forwards 3");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(2, cdd.opt_limit_backwards);
    BOOST_REQUIRE_EQUAL(3, cdd.opt_limit_forwards);
}

BOOST_AUTO_TEST_CASE(env_all)
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    int rc = cdd.options(countof(av), av, "--all");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_all);
}

BOOST_AUTO_TEST_CASE(env_direction)
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    int rc = cdd.options(countof(av), av, "--direction=+");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
}

BOOST_AUTO_TEST_CASE(env_action)
{
    Cdd cdd;
    const char *av[] = {"_cdd", };
    int rc = cdd.options(countof(av), av, "--action=0");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("0", cdd.opt_path);
    BOOST_REQUIRE_EQUAL(false, cdd.opt_history);
}

BOOST_AUTO_TEST_CASE(env_action_override_1)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history"};
    int rc = cdd.options(countof(av), av, "--action=0");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_history);
}

BOOST_AUTO_TEST_CASE(env_action_override_2)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--history", "+", "abc"};
    int rc = cdd.options(countof(av), av, "--action=0");
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL("+", cdd.direction._direction);
    BOOST_REQUIRE_EQUAL("abc", cdd.opt_path);
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(garbage_collect)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--gc"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_gc);
}

BOOST_AUTO_TEST_CASE(delete_one)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--delete", "--path", "5"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_delete);
    BOOST_REQUIRE_EQUAL("5", cdd.opt_path);
}

BOOST_AUTO_TEST_CASE(reset)
{
    Cdd cdd;
    const char *av[] = {"_cdd", "--reset"};
    int rc = cdd.options(countof(av), av);
    BOOST_REQUIRE_EQUAL(true, rc);
    BOOST_REQUIRE_EQUAL(true, cdd.opt_reset);
}

BOOST_AUTO_TEST_SUITE_END()

// vim:ff=unix
