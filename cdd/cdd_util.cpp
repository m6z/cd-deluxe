/*

Copyright 2010-2019 Michael Graz
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

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define MAX_PATH_LENGTH _MAX_PATH
#define getcwd _getcwd
#else
#include <sys/param.h>
#include <unistd.h>
#define MAX_PATH_LENGTH MAXPATHLEN
#endif

#include <chrono>
#include <ctime>
#include <iostream>

namespace fs = std::filesystem;

std::string get_working_path()
{
    char temp[MAX_PATH_LENGTH];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

std::string get_environment(std::string var_name)
{
    std::string result;

#ifdef _WIN32
    char* buffer = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buffer, &sz, var_name.c_str()) == 0 && buffer != nullptr)
    {
        result = buffer;
        free(buffer);
    }
#else
    char* var_value = getenv(var_name.c_str());
    if (var_value != nullptr)
    {
        result = var_value;
    }
#endif

    return result;
}

std::string expand_dots(std::string path)
{
    // Regex matches:
    // 1. Separator or Start
    // 2. (Group 1) Two or more dots
    // 3. (Group 2) Optional integer
    // 4. Separator or End
    static std::regex re_dots("(?:^|[\\\\/])([.]{2,})([0-9]*)(?:[\\\\/]|$)");
    std::smatch what;

    if (std::regex_search(path, what, re_dots))
    {
        std::string dots_str = what[1].str();
        std::string num_str = what[2].str();

        // If it is a standard ".." (2 dots, no number), do not expand.
        // We concatenate and recurse on the suffix to find valid matches later in the path.
        if (dots_str.length() == 2 && num_str.empty())
        {
            return std::string(what.prefix()) + what.str() + expand_dots(std::string(what.suffix()));
        }

        // Logic: (Dots - 2) + (Number if present, else 1)
        // Ex: "...2" -> (3 - 2) + 2 = 3 levels
        int base = dots_str.length() - 2;
        int count = num_str.empty() ? 1 : std::stoi(num_str);
        int total_levels = base + count;

        std::string replacement;
        for (int i = 0; i < total_levels; i++)
        {
            if (i > 0)
            {
                replacement += fs::path::preferred_separator;
            }
            replacement += "..";
        }

        // Reconstruct the path with separators preserved
        // s_start: text between the previous separator and the dots
        std::string s_start(what[0].first, what[1].first);
        // s_end: text between the digits (or dots) and the next separator
        std::string s_end(what[2].second, what[0].second);

        return std::string(what.prefix()) + s_start + replacement + expand_dots(s_end + std::string(what.suffix()));
    }
    return path;
}

bool is_special_dash_parameter(const std::string& s)
{
    // Regex Explanation:
    // ^         : Start of string
    // (         : Start of group
    //   -{2,}   : Match 2 or more dashes (and nothing else)
    //   |       : OR
    //   -[0-9]+ : Match 1 dash followed by at least one digit
    // )         : End of group
    // $         : End of string

    static const std::regex pattern("^(-{2,}|-[0-9]+)$");

    return std::regex_match(s, pattern);
}

bool is_parent_of(fs::path parent, fs::path child)
{
    parent = fs::absolute(parent).lexically_normal();
    child = fs::absolute(child).lexically_normal();

    // Calculate the path to get from parent to child
    fs::path rel = fs::relative(child, parent);

    // If the result is empty, they are the same path.
    // If the result starts with "..", child is outside parent.
    // Otherwise, child is inside parent.
    return !rel.empty() && rel.string().find("..") != 0;
}

std::vector<std::tuple<std::string, fs::path>> get_path_components(const fs::path& path)
{
    std::vector<std::tuple<std::string, fs::path>> components;

    fs::path current;
    for (const auto& part : fs::absolute(path).lexically_normal())
    {
        current /= part;
        if (part == current.root_path())
        {
            // Skip root path component
            continue;
        }
        if (part.empty())
        {
            // Skip empty components (can happen with trailing slashes)
            continue;
        }

        // here, add it
        components.emplace_back(part.string(), current);
    }
    std::reverse(components.begin(), components.end());
    return components;
}

// trim string from first occurrence of character ch plus any following spaces
std::string trim_from_char(const std::string& str, char ch)
{
    size_t pos = str.find(ch);
    if (pos != std::string::npos)
    {
        size_t start_pos = pos + 1;
        while (start_pos < str.size() && std::isspace(static_cast<unsigned char>(str[start_pos])))
        {
            start_pos++;
        }
        return str.substr(start_pos);
    }
    return str;
}

int get_year()
{
    // 1. Get current time point
    auto now = std::chrono::system_clock::now();

    // 2. Convert to time_t
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // 3. Convert to local time struct
    std::tm now_tm = *std::localtime(&now_c);

    // 4. Get year (tm_year is years since 1900)
    return 1900 + now_tm.tm_year;
}
