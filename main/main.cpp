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

#include "cdd/cdd_options_init.h"
#include "stdafx.h"

#ifdef _WIN32
#    include <direct.h>
#    define isatty _isatty
#    define fileno _fileno
#else
#    include <unistd.h>
#endif

int main(int argc, const char* argv[])
{
    try
    {
        if (isatty(fileno(stdin)))
        {
            CddOptionsInit options_init;
            return options_init.parse(argc, const_cast<char**>(argv));
        }

        // convert argv to vector<string>
        std::vector<std::string> args(argv, argv + argc);
        CddOptions options;
        if (!options.initialize(args, get_environment(CddOptions::environment_variable_name)))
        {
            if (!options.error_message.empty())
            {
                cerr << options.error_message << endl;
            }
            return 1;
        }
        if (options.show_help || options.show_version)
        {
            // help/version output is handled in options.initialize
            return 0;
        }

        // options.output();

        // read output of directory stack from stdin
        vector<string> dirs;

        string line;
        while (getline(cin, line))
        {
            dirs.push_back(line);
        }

        Cdd2 cdd(options, dirs);
        cdd.process();
        cout << cdd.get_out_str();
        cerr << cdd.get_err_str();
    }
    catch (exception& e)
    {
        cerr << "** Caught exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}

// vim:ff=unix
