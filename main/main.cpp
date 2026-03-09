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

#include "cdd/cdd2.h"
#include "cdd/cdd_options.h"
#include "cdd/cdd_options_init.h"
#include "cdd/cdd_util.h"

#ifdef _WIN32
#include <fcntl.h> // for _O_BINARY
#include <io.h>    // for _isatty() on Windows
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h> // for isatty() on Unix/Linux/macOS
#endif

using namespace std;

// Check if any of the arguments starts with --init
// These should always go through CddOptionsInit, regardless of stdin state
static bool has_init_args(int argc, const char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg.starts_with("--init"))
        {
            return true;
        }
    }
    return false;
}

int main(int argc, const char* argv[])
{
    try
    {
        // If stdin is a tty OR if init/help/version/force args are present,
        // use CddOptionsInit (handles setup commands)
        if (isatty(fileno(stdin)) || has_init_args(argc, argv))
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
