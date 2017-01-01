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

static string arr_test_dirs[] = {
    "/tmp/b",  // fourth visited
    "/tmp/c",  // third visited
    "/tmp/b",  // second visited
    "/tmp/a",  // first visited
};

BOOST_AUTO_TEST_SUITE(cdd_test)

BOOST_AUTO_TEST_CASE(initialization)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));

    // Check vec_dir_stack
    BOOST_REQUIRE_EQUAL(4, cdd.vec_dir_stack.size());
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[0]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_stack[1]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[2]);
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_stack[3]);

    // Check vec_dir_last_to_first
    BOOST_REQUIRE_EQUAL(3, cdd.vec_dir_last_to_first.size());
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_last_to_first[0]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_last_to_first[1]);
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_last_to_first[2]);

    // Check vec_dir_first_to_last
    BOOST_REQUIRE_EQUAL(3, cdd.vec_dir_first_to_last.size());
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_first_to_last[0]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_first_to_last[1]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_first_to_last[2]);

    // Check the most common
    BOOST_REQUIRE_EQUAL(3, cdd.vec_dir_most_to_least.size());
    BOOST_REQUIRE_EQUAL(Cdd::Common(2, 0, "/tmp/b"), cdd.vec_dir_most_to_least[0]);
    BOOST_REQUIRE_EQUAL(Cdd::Common(1, 1, "/tmp/c"), cdd.vec_dir_most_to_least[1]);
    BOOST_REQUIRE_EQUAL(Cdd::Common(1, 2, "/tmp/a"), cdd.vec_dir_most_to_least[2]);
}

BOOST_AUTO_TEST_CASE(initialization_with_current_dir)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "/tmp/d");

    // Check vec_dir_stack
#ifdef WIN32
    BOOST_REQUIRE_EQUAL(5, cdd.vec_dir_stack.size());
    BOOST_REQUIRE_EQUAL("/tmp/d", cdd.vec_dir_stack[0]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[1]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_stack[2]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[3]);
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_stack[4]);
#else
    BOOST_REQUIRE_EQUAL(4, cdd.vec_dir_stack.size());
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[0]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_stack[1]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_stack[2]);
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_stack[3]);
#endif
}

BOOST_AUTO_TEST_CASE(no_current_path)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs), "/tmp/b");

    // Check vec_dir_last_to_first
    // Current directory will not appear
    BOOST_REQUIRE_EQUAL(2, cdd.vec_dir_last_to_first.size());
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_last_to_first[0]);
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_last_to_first[1]);

    // Check vec_dir_first_to_last
    // Current directory will appear
    BOOST_REQUIRE_EQUAL(3, cdd.vec_dir_first_to_last.size());
    BOOST_REQUIRE_EQUAL("/tmp/a", cdd.vec_dir_first_to_last[0]);
    BOOST_REQUIRE_EQUAL("/tmp/b", cdd.vec_dir_first_to_last[1]);
    BOOST_REQUIRE_EQUAL("/tmp/c", cdd.vec_dir_first_to_last[2]);
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(back_none)
{
    Cdd cdd(NULL, 0);
    cdd.opt_path = "-";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No history of directories\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_dash_zero)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-0";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at -0\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_and_one)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.direction.assign("-");
    cdd.opt_path = "1";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_dash_one)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-1";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_one_dash)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_dash_two)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-2";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_two_dash)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "--";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_dash_three)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-3";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_three_dash)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "---";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_dash_four)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "-4";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at -4\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(back_four_dash)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "----";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at -4\n", cdd.strm_err.str());
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(forward_and_zero)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.direction.assign("+");
    cdd.opt_path = "0";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_plus_zero)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+0";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_one_plus)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_plus_one)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+1";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_two_plus)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "++";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_plus_two)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+2";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_three_plus)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+++";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_plus_three)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "+3";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at +3\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(forward_four_plus)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "++++";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at +3\n", cdd.strm_err.str());
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(common_comma_and_zero)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.direction.assign(",");
    cdd.opt_path = "0";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_comma_zero)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",0";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_one_comma)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/b\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/b'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/b\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_comma_one)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",1";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_two_commas)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",,";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/c\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/c'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/c\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_comma_two)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",2";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_three_commas)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",,,";
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd /tmp/a\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/a'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("cdd: /tmp/a\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_comma_three)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",3";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at ,3\n", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(common_four_commas)
{
    Cdd cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = ",,,,";
    cdd.process();
    BOOST_REQUIRE_EQUAL("", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("No directory at ,3\n", cdd.strm_err.str());
}

//----------------------------------------------------------------------

struct CddPath: Cdd
{
    bool _is_directory;
    bool _is_regular_file;
    CddPath(string arr_pushd[], int count) : Cdd(arr_pushd, count)
    {
        _is_directory = false;
        _is_regular_file = false;
    }
    virtual bool is_directory(string path) { return _is_directory; }
    virtual bool is_regular_file(string path) { return _is_regular_file; }
};

BOOST_AUTO_TEST_CASE(cdd_to_dir)
{
    CddPath cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "/tmp/z";
    cdd._is_directory = true;
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd \\tmp\\z\n", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("cdd: \\tmp\\z\n", cdd.strm_err.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp/z'\n", cdd.strm_out.str());
    // For unix since directory was not rewritten it will not be echoed
    // BOOST_REQUIRE_EQUAL("cdd: /tmp/z\n", cdd.strm_err.str());
#endif
}

BOOST_AUTO_TEST_CASE(cdd_to_dir_quiet)
{
    CddPath cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "alpha";
    cdd._is_directory = true;
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd alpha\n", cdd.strm_out.str());
#else
    BOOST_REQUIRE_EQUAL("pushd 'alpha'\n", cdd.strm_out.str());
#endif
    BOOST_REQUIRE_EQUAL("", cdd.strm_err.str());
}

BOOST_AUTO_TEST_CASE(cdd_to_dots)
{
    CddPath cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "...";
    cdd._is_directory = true;
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd ..\\..\n", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("cdd: ..\\..\n", cdd.strm_err.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '../..'\n", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("cdd: ../..\n", cdd.strm_err.str());
#endif
}

BOOST_AUTO_TEST_CASE(cdd_to_file)
{
    CddPath cdd(arr_test_dirs, countof(arr_test_dirs));
    cdd.opt_path = "/tmp/z.tmp";
    cdd._is_regular_file = true;
    cdd.process();
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("pushd \\tmp\n", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("cdd: \\tmp\n", cdd.strm_err.str());
#else
    BOOST_REQUIRE_EQUAL("pushd '/tmp'\n", cdd.strm_out.str());
    BOOST_REQUIRE_EQUAL("cdd: /tmp\n", cdd.strm_err.str());
#endif
}

//----------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
