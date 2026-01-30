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

#include <catch2/catch_test_macros.hpp>

namespace
{

static vector<string> test_dirs = {
    "aa", // fourth visited,   na   -1
    "cc", // third visited,    2    -2
    "bb", // second visited,   1    -3
    "aa", // first visited,    0
};

}

TEST_CASE("cleanup_test")
{
    SECTION("garbage_collect")
    {
        auto cdd = cdd_test({"_cdd", "--gc"}, "", "/current/working/dir", test_dirs);
        cdd.process();

        vector<string> act = split_text(cdd.get_out_str());
        vector<string> exp = {
#ifdef WIN32
            "for /l %%i in (1,1,4) do popd", //
            "chdir/d aa 2>nul",              //
            "pushd bb 2>nul",                //
            "pushd cc 2>nul",                //
            "pushd /current/working/dir",    //
#else
            "\\cd 'aa'",  //
            "dirs -c",    //
            "pushd 'bb'", //
            "\\cd 'cc'",  //
#endif
        };
        REQUIRE(exp == act);
        REQUIRE("cdd reduced dirs from 4 to 3\n" == cdd.get_err_str());
    }

    SECTION("delete_one")
    {
        auto cdd = cdd_test({"_cdd", "--del", "+1"}, "", "/current/working/dir", test_dirs);
        cdd.process();

        vector<string> act = split_text(cdd.get_out_str());
        vector<string> exp = {
#ifdef WIN32
            "for /l %%i in (1,1,4) do popd", //
            "chdir/d aa 2>nul",              //
            "pushd cc 2>nul",                //
            "pushd aa 2>nul",                //
            "pushd /current/working/dir",    //
#else
            "\\cd 'aa'",  //
            "dirs -c",    //
            "pushd 'cc'", //
            "\\cd 'aa'",  //
#endif
        };
        REQUIRE(exp == act);
        REQUIRE("cdd del: bb\n" == cdd.get_err_str());
    }

    SECTION("reset")
    {
        auto cdd = cdd_test({"_cdd", "--reset"}, "", "/current/working/dir", test_dirs);
        cdd.process();

        vector<string> act = split_text(cdd.get_out_str());
        vector<string> exp = {
#ifdef WIN32
            "for /l %%i in (1,1,4) do popd",
            "chdir/d /current/working/dir",
#else
            "dirs -c",
#endif
        };
        REQUIRE(exp == act);
        REQUIRE("cdd reset\n" == cdd.get_err_str());
    }
}

// vim:ff=unix
