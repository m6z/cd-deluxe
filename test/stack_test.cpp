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
#include <boost/algorithm/string.hpp>

#define countof(x) (sizeof(x)/sizeof(x[0]))

static string arr_test_dirs[] = {
    "aa",   // fourth visited,   na   -1
    "cc",   // third visited,    2    -2
    "bb",   // second visited,   1    -3
    "aa",   // first visited,    0
};

BOOST_AUTO_TEST_SUITE(stack_test)

vector<string> splitlines(stringstream& strm)
{
    string input = strm.str();
    vector<string> result;
    boost::split(result, input, boost::is_any_of("\n"));
    if (result.size())
        result.pop_back();
    return result;
}

BOOST_AUTO_TEST_CASE(garbage_collect)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_gc = true;
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    const char *exp[] = {
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
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
    BOOST_REQUIRE_EQUAL("cdd gc\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(delete_one)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_delete = true;
    cdd.direction.assign("+");
    cdd.opt_path = "1";
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    const char *exp[] = {
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
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
    BOOST_REQUIRE_EQUAL("cdd del: bb\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(reset)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "dd");
    cdd.opt_reset = true;
    cdd.process();
    vector<string> act = splitlines(cdd.strm_out);
    const char *exp[] = {
#ifdef WIN32
        "for /l %%i in (1,1,4) do popd",
        "chdir/d dd",
#else
        "dirs -c",
#endif
    };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
    BOOST_REQUIRE_EQUAL("cdd reset\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(command_generator_1)
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
    const char *exp[] = {
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
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
}

BOOST_AUTO_TEST_CASE(command_generator_2)
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
    const char *exp[] = {
#ifdef WIN32
        "for /l %%i in (1,1,2) do popd",
        "chdir/d abc 2>nul",
        "pushd ghi",
#else
        "dirs -c",
        "\\cd 'abc'",
#endif
    };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
}

BOOST_AUTO_TEST_CASE(command_generator_3)
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
    const char *exp[] = {
#ifdef WIN32
        "for /l %%i in (1,1,2) do popd",
        "chdir/d ghi",
#else
        "dirs -c",
#endif
    };
    BOOST_REQUIRE_EQUAL_COLLECTIONS(exp, exp+countof(exp), act.begin(), act.end());
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
