/*

Copyright 2010-2021 Michael Graz
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

static string arr_test_dirs[] = {
    "/opt/d",  // fourth visited
    "/opt/c",  // third visited
    "/opt/b",  // second visited
    "/opt/a",  // first visited
};

TEST_CASE("history_test")
{

SECTION("history_forward")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("+");
    cdd.opt_limit_backwards = 2;  // will be ignored
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE("  0: /opt/a\n  1: /opt/b\n  2: /opt/c\n  3: /opt/d\n" == cdd.strm_err.str());
}

SECTION("history_forward_limit")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("+");
    cdd.opt_limit_forwards = 2;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE("  0: /opt/a\n  1: /opt/b\n ... showing first 2 of 4\n" == cdd.strm_err.str());
}

SECTION("history_forward_limit_all")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("+");
    cdd.opt_limit_forwards = 2;
    cdd.opt_all = true;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE("  0: /opt/a\n  1: /opt/b\n  2: /opt/c\n  3: /opt/d\n" == cdd.strm_err.str());
}

SECTION("history_backwards")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("-");
    cdd.opt_limit_forwards = 2;  // will be ignored
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" -1: /opt/d\n -2: /opt/c\n -3: /opt/b\n -4: /opt/a\n" == cdd.strm_err.str());
}

SECTION("history_backwards_limit")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("-");
    cdd.opt_limit_backwards = 2;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" -1: /opt/d\n -2: /opt/c\n ... showing last 2 of 4\n" == cdd.strm_err.str());
}

SECTION("history_backwards_limit_all")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_history = true;
    cdd.direction.assign("-");
    cdd.opt_limit_backwards = 2;
    cdd.opt_all = true;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" -1: /opt/d\n -2: /opt/c\n -3: /opt/b\n -4: /opt/a\n" == cdd.strm_err.str());
}

//----------------------------------------------------------------------

static string arr_common_dirs[] = {
    "/var/a",  // seventh visited
    "/var/c",  // sixth visited
    "/var/b",  // fifth visited
    "/var/a",  // fourth visited
    "/var/c",  // third visited
    "/var/b",  // second visited
    "/var/a",  // first visited
};

SECTION("history_common")
{
    Cdd cdd(arr_common_dirs, countof(arr_common_dirs));
    cdd.opt_history = true;
    cdd.direction.assign(",");
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" ,0: ( 3) /var/a\n ,1: ( 2) /var/c\n ,2: ( 2) /var/b\n" == cdd.strm_err.str());
}

SECTION("history_common_limit")
{
    Cdd cdd(arr_common_dirs, countof(arr_common_dirs));
    cdd.opt_history = true;
    cdd.direction.assign(",");
    cdd.opt_limit_common = 2;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" ,0: ( 3) /var/a\n ,1: ( 2) /var/c\n ... showing top 2 of 3\n" == cdd.strm_err.str());
}

SECTION("history_common_limit_all")
{
    Cdd cdd(arr_common_dirs, countof(arr_common_dirs));
    cdd.opt_history = true;
    cdd.direction.assign(",");
    cdd.opt_limit_common = 2;
    cdd.opt_all = true;
    cdd.process();
    REQUIRE("" == cdd.strm_out.str());
    REQUIRE(" ,0: ( 3) /var/a\n ,1: ( 2) /var/c\n ,2: ( 2) /var/b\n" == cdd.strm_err.str());
}

}

// vim:ff=unix
