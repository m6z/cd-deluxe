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

TEST_CASE("util_test")
{

SECTION("normalize_path")
{
    REQUIRE("/tmp/a" == Cdd::normalize_path("/tmp/a"));
    // Remove trailing slash
    REQUIRE("/tmp/a" == Cdd::normalize_path("/tmp/a/"));
#ifdef WIN32
    REQUIRE("c:/tmp/a" == Cdd::normalize_path("C:\\tmp\\a\\"));
#endif
}

SECTION("windowize_path")
{
#ifdef WIN32
    REQUIRE("\\tmp\\a" == Cdd::windowize_path("/tmp/a"));
#endif
}

SECTION("expand_dots")
{
    auto fun = [](string s) { return Cdd::normalize_path(Cdd::expand_dots(s)); };

    REQUIRE("/tmp/a" == fun("/tmp/a"));
    REQUIRE("../.." == fun("..."));
    REQUIRE("../../.." == fun("...."));
    REQUIRE("abc..." == fun("abc..."));
    REQUIRE("...def" == fun("...def"));
    REQUIRE("abc...def" == fun("abc...def"));
    REQUIRE("abc/../../def" == fun("abc/.../def"));
    REQUIRE("abc/../../../def" == fun("abc\\....\\def"));
    REQUIRE("abc/../../def/../../ghi" == fun("abc\\...\\def/.../ghi"));
}

}
