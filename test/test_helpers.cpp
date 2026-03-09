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

#include "test_helpers.h"

string trim(const string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

vector<string> split_text(const string& dirs)
{
    vector<string> result;
    stringstream ss(dirs);
    string line;
    while (getline(ss, line))
    {
        // trim leading and trailing whitespace
        line = trim(line);
        if (!line.empty())
        {
            result.push_back(line);
        }
    }
    return result;
}

Cdd2_Test cdd_test(const vector<string>& args, const string& env, const fs::path& cwd, const string& dirs)
{
    CddOptions options(args, env);
    return Cdd2_Test(options, split_text(dirs), cwd);
}

Cdd2_Test cdd_test(const vector<string>& args, const string& env, const fs::path& cwd, const vector<string>& dirs)
{
    CddOptions options(args, env);
    return Cdd2_Test(options, dirs, cwd);
}

std::string nix_path(const fs::path& p)
{
    string s = p.string();

    // remove drive letter on Windows for comparison
    if (s.size() >= 2 && std::isalpha(s[0]) && s[1] == ':')
    {
        s = s.substr(2);
    }

    // replace all backward slashes with forward slashes for test comparison
    std::replace(s.begin(), s.end(), '\\', '/');

    return s;
};

std::string swap_drive_letter(string s, char new_drive_letter)
{
    // iterate through the string to find and replace each drive letter pattern (e.g., "C:")
    for (size_t i = 0; i + 1 < s.size(); ++i)
    {
        if (std::isalpha(s[i]) && s[i + 1] == ':')
        {
            s[i] = new_drive_letter;
            i++; // skip the colon
        }
    }
    return s;
}
