#pragma once

#include "cdd/cdd2.h"

std::string trim(const std::string& str);
std::vector<std::string> split_text(const std::string& dirs);

class Cdd2_Test : public Cdd2
{
public:
    bool _is_directory = false;
    bool _is_regular_file = false;

    using Cdd2::Cdd2;
    using Cdd2::create_dirs_first_to_last;
    using Cdd2::create_dirs_last_to_first;
    using Cdd2::create_dirs_most_to_least;
    using Cdd2::create_dirs_upwards;

    virtual bool is_directory(const fs::path& /*path*/) { return _is_directory; }
    virtual bool is_regular_file(const fs::path& /*path*/) { return _is_regular_file; }
};

// factory functions for Cdd2_Test
Cdd2_Test cdd_test(const std::vector<std::string>& args, const std::string& env, const fs::path& cwd, const std::string& dirs);
Cdd2_Test cdd_test(const std::vector<std::string>& args, const std::string& env, const fs::path& cwd, const std::vector<std::string>& dirs);

// convert a filesystem path to a Unix/Linux style string
std::string nix_path(const fs::path& p);

// simplify windows specific tests
std::string swap_drive_letter(std::string s, char new_drive_letter = 'C');
