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
    "aa",   // fourth visited,   na   -1
    "cc",   // third visited,    2    -2
    "bb",   // second visited,   1    -3
    "aa",   // first visited,    0
};

vector<string> splitlines(stringstream& strm)
{
    vector<string> result;
    std::string line;
    while (std::getline(strm, line))
        result.push_back(line);
    return result;
}

TEST_CASE("stack_test")
{

SECTION("garbage_collect")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_gc = true;
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,4) do popd",
        "chdir/d aa 2>nul",
        "pushd bb 2>nul",
        "pushd cc 2>nul",
        "pushd dd 2>nul",
        "pushd dd",
#else
        "dirs -c",
        "\\cd 'aa'",
        "pushd 'bb'",
        "pushd 'cc'",
#endif
    };
    REQUIRE( exp == act );
    REQUIRE("cdd gc\n" == cdd.strm_err.str());
}

SECTION("delete_one")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_delete = true;
    cdd.direction.assign("+");
    cdd.opt_path = "1";
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,4) do popd",
        "chdir/d aa 2>nul",
        "pushd cc 2>nul",
        "pushd aa 2>nul",
        "pushd dd 2>nul",
        "pushd dd",
#else
        "dirs -c",
        "\\cd 'aa'",
        "pushd 'cc'",
        "pushd 'aa'",
#endif
    };
    REQUIRE( exp == act );
    REQUIRE("cdd del: bb\n" == cdd.strm_err.str());
}

SECTION("reset")
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_reset = true;
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,4) do popd",
        "chdir/d dd",
#else
        "dirs -c",
#endif
    };
    REQUIRE( exp == act );
    REQUIRE("cdd reset\n" == cdd.strm_err.str());
}

SECTION("command_generator_1")
{
    string arr_dir[] = {
        "def",   // second visited
        "abc",   // first visited
    };
    vector<string> vec_test_dirs(arr_dir, arr_dir+countof(arr_dir));
    Cdd cdd(vec_test_dirs, "ghi");
    vector<string> vec_dir;
    vec_dir.assign(vec_test_dirs.rbegin(), vec_test_dirs.rend());
    cdd.command_generator(vec_dir);
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,2) do popd",
        "chdir/d abc 2>nul",
        "pushd def 2>nul",
        "pushd ghi",
#else
        "dirs -c",
        "\\cd 'abc'",
        "pushd 'def'",
#endif
    };
    REQUIRE( exp == act );
}

SECTION("command_generator_2")
{
    string arr_dir[] = {
        "def",   // second visited
        "abc",   // first visited
    };
    vector<string> vec_test_dirs(arr_dir, arr_dir+countof(arr_dir));
    Cdd cdd(vec_test_dirs, "ghi");
    vector<string> vec_dir;
    vec_dir.assign(vec_test_dirs.rbegin(), vec_test_dirs.rend());
    cdd.command_generator(vec_dir, "def");
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,2) do popd",
        "chdir/d abc 2>nul",
        "pushd ghi",
#else
        "dirs -c",
        "\\cd 'abc'",
#endif
    };
    REQUIRE( exp == act );
}

SECTION("command_generator_3")
{
    string arr_dir[] = {
        "def",   // second visited
        "abc",   // first visited
    };
    vector<string> vec_test_dirs(arr_dir, arr_dir+countof(arr_dir));
    Cdd cdd(vec_test_dirs, "ghi");
    vector<string> vec_dir;
    cdd.command_generator(vec_dir, "def");
    vector<string> act = splitlines(cdd.strm_out);
    vector<string> exp = {
#ifdef WIN32
        "for /l %%i in (1,1,2) do popd",
        "chdir/d ghi",
#else
        "dirs -c",
#endif
    };
    REQUIRE( exp == act );
}

//----------------------------------------------------------------------

}

// vim:ff=unix
