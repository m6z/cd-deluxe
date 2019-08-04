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
    #include <io.h>
    #include <direct.h>
    #define MAX_PATH_LENGTH _MAX_PATH
    #define getcwd _getcwd
#else
    #include <unistd.h>
    #include <sys/param.h>
    #define MAX_PATH_LENGTH MAXPATHLEN
#endif

std::string get_working_path()
{
   char temp[MAX_PATH_LENGTH];
   return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
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
    char *var_value = getenv(var_name.c_str());
    if ( var_value != nullptr )
    {
        result = var_value;
    }
#endif

    return result;
}

