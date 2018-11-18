/*

Copyright 2010-2018 Michael Graz
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
    #define MAX_PATH_LENGTH _MAX_PATH
    #define getcwd _getcwd
    #define isatty _isatty
    #define fileno _fileno
#else
    #include <unistd.h>
    #define MAX_PATH_LENGTH MAXPATHLEN
#endif

std::string get_working_path()
{
   char temp[MAX_PATH_LENGTH];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}

string get_environment(string var_name)
{
    string result;

#ifdef _WIN32
    char* buffer = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buffer, &sz, var_name.c_str()) == 0 && buffer != nullptr)
    {
        result = buffer;
        free(buffer);
    }
#else
    char *var_value = getenv(var_name.c_str());
    if ( var_value != nullptr )
    {
        result = var_value;
    }
#endif

    return result;
}

int main(int argc, const char* argv[])
{
    if (isatty(fileno(stdin)))
    {
        cout << "stdin is a terminal, expecting piped directory stack" << endl;
        Cdd::help();
        return 1;
    }

    vector<string> vec_pushd;
    string line;
    while (getline(cin, line))
        vec_pushd.push_back(line);

    try
    {
        Cdd cdd(vec_pushd, get_working_path());

        if (cdd.options(argc, argv, get_environment(Cdd::env_options_name)))
            cdd.process();

        cout << cdd.strm_out.str();
        cerr << cdd.strm_err.str();
    }
    catch (exception& e)
    {
        cerr << "** Caught exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}

// vim:ff=unix
