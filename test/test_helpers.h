/*

Copyright 2010-2026 Michael Graz
https://github.com/m6z/cd-deluxe

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

#pragma once

#include "cdd/cdd2.h"

std::string trim(const std::string& str);
std::vector<std::string> split_text(const std::string& dirs);

class Cdd2_Test : public Cdd2
{
public:
    bool _is_directory = false;
    bool _is_regular_file = false;

    using Cdd2::Cdd2;
    using Cdd2::create_dirs_first_to_last;
    using Cdd2::create_dirs_last_to_first;
    using Cdd2::create_dirs_most_to_least;
    using Cdd2::create_dirs_upwards;

    virtual bool is_directory(const fs::path& /*path*/) { return _is_directory; }
    virtual bool is_regular_file(const fs::path& /*path*/) { return _is_regular_file; }
};

// factory functions for Cdd2_Test
Cdd2_Test cdd_test(const std::vector<std::string>& args, const std::string& env, const fs::path& cwd, const std::string& dirs);
Cdd2_Test cdd_test(const std::vector<std::string>& args, const std::string& env, const fs::path& cwd, const std::vector<std::string>& dirs);

// convert a filesystem path to a Unix/Linux style string
std::string nix_path(const fs::path& p);

// simplify windows specific tests
std::string swap_drive_letter(std::string s, char new_drive_letter = 'C');
