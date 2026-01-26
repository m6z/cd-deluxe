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
#include "test_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("util_test")
{
    SECTION("expand_dots")
    {
        auto fun = [](string s)
        {
            // replace all backward slashes with forward slashes for test comparison
            s = expand_dots(s);
            std::replace(s.begin(), s.end(), '\\', '/');
            return s;
        };

        REQUIRE("/tmp/a" == fun("/tmp/a"));
        REQUIRE("../.." == fun("..."));
        REQUIRE("../../.." == fun("...."));
        REQUIRE("abc/../../def" == fun("abc/.../def"));
        REQUIRE("abc/../../../def" == fun("abc\\....\\def"));
        REQUIRE("abc/../../def/../../ghi" == fun("abc\\...\\def/.../ghi"));
        // invalid cases - should be no change
        REQUIRE("abc..." == fun("abc..."));
        REQUIRE("...def" == fun("...def"));
        REQUIRE("abc...def" == fun("abc...def"));

        REQUIRE(".." == fun("..1"));
        REQUIRE("../.." == fun("..2"));
        REQUIRE("../../.." == fun("..3"));
        REQUIRE("../.." == fun("...1"));
        REQUIRE("../../.." == fun("...2"));
        REQUIRE("../../../../../../.." == fun("....5"));
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
        REQUIRE(is_parent_of("../..", "../../a/b") == true);
    }

    SECTION("get_path_components")
    {
        //         auto clean = [](const fs::path& p) -> string
        //         {
        //             string s = p.string();
        //
        //             // remove drive letter on Windows for comparison
        //             if (s.size() >= 2 && std::isalpha(s[0]) && s[1] == ':')
        //             {
        //                 s = s.substr(2);
        //             }
        //
        //             // replace all backward slashes with forward slashes for test comparison
        //             std::replace(s.begin(), s.end(), '\\', '/');
        //
        //             return s;
        //         };

        auto components = get_path_components("/a/b/c");
        REQUIRE(components.size() == 3);
        REQUIRE(std::get<0>(components[0]) == "c");
        REQUIRE(nix_path(std::get<1>(components[0])) == "/a/b/c");
        REQUIRE(std::get<0>(components[1]) == "b");
        REQUIRE(nix_path(std::get<1>(components[1])) == "/a/b");
        REQUIRE(std::get<0>(components[2]) == "a");
        REQUIRE(nix_path(std::get<1>(components[2])) == "/a");
    }
}
