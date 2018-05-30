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

// namespace fs = boost::filesystem;

std::string get_working_path()
{
   char temp[MAXPATHLEN];
   return ( getcwd(temp, MAXPATHLEN) ? std::string( temp ) : std::string("") );
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
        // Cdd cdd(vec_pushd, fs::current_path().string());
        Cdd cdd(vec_pushd, get_working_path());

        const char *env_options = getenv(Cdd::env_options_name.c_str());
        if (cdd.options_new(argc, argv, env_options ? env_options : string()))
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
