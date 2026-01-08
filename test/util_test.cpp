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

#include "cdd/cdd_util.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("util_test")
{
    SECTION("expand_dots")
    {
        auto fun = [](string s)
        {
#ifdef WIN32
            // replace all forward slashes with backward slashes for testing
            std::replace(s.begin(), s.end(), '/', '\\');
#else
            // replace all backward slashes with forward slashes for testing
            std::replace(s.begin(), s.end(), '\\', '/');
#endif
            return expand_dots(s);
        };

        REQUIRE("/tmp/a" == fun("/tmp/a"));
        REQUIRE("../.." == fun("..."));
        REQUIRE("../../.." == fun("...."));
        REQUIRE("abc..." == fun("abc..."));
        REQUIRE("...def" == fun("...def"));
        REQUIRE("abc...def" == fun("abc...def"));
        REQUIRE("abc/../../def" == fun("abc/.../def"));
        REQUIRE("abc/../../../def" == fun("abc\\....\\def"));
        REQUIRE("abc/../../def/../../ghi" == fun("abc\\...\\def/.../ghi"));

        // TODO add tests for path_separator
    }

    SECTION("special_dash_param")
    {
        REQUIRE(is_special_dash_parameter("-") == false);
        REQUIRE(is_special_dash_parameter("--") == true);
        REQUIRE(is_special_dash_parameter("----") == true);
        REQUIRE(is_special_dash_parameter("-123") == true);
        REQUIRE(is_special_dash_parameter("-1") == true);
        REQUIRE(is_special_dash_parameter("45") == false);
    }

    SECTION("is_parent_of")
    {
        if (false) // TODO - old - remove
        {
            fs::path parent1 = "/abc/def";
            fs::path child1 = "/abc/def/ghi/xyz.txt";
            fs::path nonchild1 = "/abc/defxyz/ghi.txt";

            REQUIRE(is_parent_of(parent1, child1) == true);
            REQUIRE(is_parent_of(parent1, nonchild1) == false);
        }

        REQUIRE(is_parent_of("../..", "../../a/b") == true);
    }

    SECTION("get_path_components")
    {
        auto components = get_path_components("/a/b/c");
        REQUIRE(components.size() == 3);
        REQUIRE(std::get<0>(components[0]) == "c");
        REQUIRE(std::get<1>(components[0]) == "/a/b/c");
        REQUIRE(std::get<0>(components[1]) == "b");
        REQUIRE(std::get<1>(components[1]) == "/a/b");
        REQUIRE(std::get<0>(components[2]) == "a");
        REQUIRE(std::get<1>(components[2]) == "/a");
    }
}

// vim:ff=unix
