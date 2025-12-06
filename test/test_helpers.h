#pragma once

#include "stdafx.h"

string trim(const string& str);
vector<string> split_text(const string& dirs);

class Cdd2_Test : public Cdd2
{
public:
    bool _is_directory = false;
    bool _is_regular_file = false;

    using Cdd2::Cdd2;
    using Cdd2::create_dirs_first_to_last;
    using Cdd2::create_dirs_last_to_first;
    using Cdd2::create_dirs_most_to_least;

    virtual bool is_directory(const fs::path& /*path*/) { return _is_directory; }
    virtual bool is_regular_file(const fs::path& /*path*/) { return _is_regular_file; }
};

// factory functions for Cdd2_Test
Cdd2_Test cdd_test(const vector<string>& args, const string& env, const fs::path& cwd, const string& dirs);
Cdd2_Test cdd_test(const vector<string>& args, const string& env, const fs::path& cwd, const vector<string>& dirs);
