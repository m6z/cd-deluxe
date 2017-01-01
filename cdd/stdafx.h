// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include "cdd.h"

#ifndef WIN32
#include <sys/stat.h>
#endif

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <iomanip>

#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#define BOOST_FILESYSTEM_VERSION 2
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

