// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif
#include <iostream>
#include <string>
#include <vector>

#include <cdd/cdd.h>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

// vim:ff=unix
