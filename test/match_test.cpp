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

#include "catch.hpp"

#define countof(x) (sizeof(x)/sizeof(x[0]))

static string arr_test_dirs[] = {
    "/cc/dd",   // sixth visited
    "/bb/ee",   // fifth visited
    "/aa/bb",   // fourth visited
    "/cc/dd",   // third visited
    "/bb/cc",   // second visited
    "/aa/bb",   // first visited
};

TEST_CASE("match_test")
{

SECTION("simple_match")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "ee";
    cdd.process();
#ifdef WIN32
    REQUIRE("pushd /bb/ee\n" == cdd.strm_out.str());
#else
    REQUIRE("pushd '/bb/ee'\n" == cdd.strm_out.str());
#endif
    REQUIRE("cdd: /bb/ee\n" == cdd.strm_err.str());
}

SECTION("multi_match_backwards")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "bb";
    cdd.direction.assign("-");
    cdd.process();
#ifdef WIN32
    REQUIRE("pushd /bb/ee\n" == cdd.strm_out.str());
#else
    REQUIRE("pushd '/bb/ee'\n" == cdd.strm_out.str());
#endif
    REQUIRE("cdd: /bb/ee\n -3: /aa/bb\n -4: /bb/cc\n" == cdd.strm_err.str());
}

SECTION("multi_match_default_backwards")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "cc";
    cdd.process();
#ifdef WIN32
    REQUIRE("pushd /cc/dd\n" == cdd.strm_out.str());
#else
    REQUIRE("pushd '/cc/dd'\n" == cdd.strm_out.str());
#endif
    REQUIRE("cdd: /cc/dd\n -4: /bb/cc\n" == cdd.strm_err.str());
}

SECTION("multi_match_forwards")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "bb";
    cdd.direction.assign("+");
    cdd.process();
#ifdef WIN32
    REQUIRE("pushd /aa/bb\n" == cdd.strm_out.str());
#else
    REQUIRE("pushd '/aa/bb'\n" == cdd.strm_out.str());
#endif
    REQUIRE("cdd: /aa/bb\n  1: /bb/cc\n  3: /bb/ee\n" == cdd.strm_err.str());
}

SECTION("multi_match_common")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "cc";
    cdd.direction.assign(",");
    cdd.process();
#ifdef WIN32
    REQUIRE("pushd /cc/dd\n" == cdd.strm_out.str());
#else
    REQUIRE("pushd '/cc/dd'\n" == cdd.strm_out.str());
#endif
    REQUIRE("cdd: /cc/dd\n ,3: ( 1) /bb/cc\n" == cdd.strm_err.str());
}

}
