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

#include <filesystem>
#include <string>

std::string get_cdd_version();
std::string get_working_path();
std::string get_environment(std::string var_name);
std::string expand_dots(std::string path);
bool is_special_dash_parameter(const std::string& s);
bool is_parent_of(std::filesystem::path parent, std::filesystem::path child);
std::vector<std::tuple<std::string, std::filesystem::path>> get_path_components(const std::filesystem::path& path);
std::string trim_from_char(const std::string& str, char ch);
int get_year();
