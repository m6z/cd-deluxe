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

BOOST_AUTO_TEST_SUITE(util_test)

BOOST_AUTO_TEST_CASE(normalize_path)
{
    BOOST_REQUIRE_EQUAL("/tmp/a", Cdd::normalize_path("/tmp/a"));
    // Remove trailing slash
    BOOST_REQUIRE_EQUAL("/tmp/a", Cdd::normalize_path("/tmp/a/"));
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("c:/tmp/a", Cdd::normalize_path("C:\\tmp\\a\\"));
#endif
}

BOOST_AUTO_TEST_CASE(windowize_path)
{
#ifdef WIN32
    BOOST_REQUIRE_EQUAL("\\tmp\\a", Cdd::windowize_path("/tmp/a"));
#endif
}

string fun(string s)
{
    return Cdd::normalize_path(Cdd::expand_dots(s));
}

BOOST_AUTO_TEST_CASE(expand_dots)
{
    BOOST_REQUIRE_EQUAL("/tmp/a", fun("/tmp/a"));
    BOOST_REQUIRE_EQUAL("../..", fun("..."));
    BOOST_REQUIRE_EQUAL("../../..", fun("...."));
    BOOST_REQUIRE_EQUAL("abc...", fun("abc..."));
    BOOST_REQUIRE_EQUAL("...def", fun("...def"));
    BOOST_REQUIRE_EQUAL("abc...def", fun("abc...def"));
    BOOST_REQUIRE_EQUAL("abc/../../def", fun("abc/.../def"));
    BOOST_REQUIRE_EQUAL("abc/../../../def", fun("abc\\....\\def"));
    BOOST_REQUIRE_EQUAL("abc/../../def/../../ghi", fun("abc\\...\\def/.../ghi"));
}

BOOST_AUTO_TEST_SUITE_END()
