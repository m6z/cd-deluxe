// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#include <sys/io.h>
#include <sys/param.h>
#endif
#include <iostream>
#include <string>
#include <vector>

#include <cdd/cdd.h>
#include <cdd/cdd_util.h>

// vim:ff=unix
