#include "cdd2.h"

#include <iostream>
#include <map>
#include <regex>
#include <set>

#include "cdd_util.h"

namespace fs = std::filesystem;

//----------------------------------------------------------------------

std::string Cdd2::KeyedPath::generate_key_from_path(const fs::path& p, bool ignore_case)
{
    std::string s = p.generic_string();
    if (ignore_case)
    {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    }
    return s;
}

//----------------------------------------------------------------------

void Cdd2::initialize()
{
#ifdef WIN32
    if (!cwd.empty())
    {
        vec_dir_stack.insert(vec_dir_stack.begin(), current_path);
        current_path_added = true;
    }
#endif

    // TODO is this still important?
    // int current_path_inode = get_inode(current_path_normalized);

    KeyedPath kp_cwd(cwd, options.ignore_case);

    // First, save the vector of pushed directories
    // skipping over duplicates and the current working directory
    set<KeyedPath> set_dir1;
    for (const auto& dir : dirs)
    {
        KeyedPath kp(dir, options.ignore_case);

        // Check to see if this directory has already been seen
        if (set_dir1.find(kp) == set_dir1.end())
        {
            // Filter out the current_path after the set inclusion test,
            if (kp == kp_cwd)
            {
                continue;
            }
            // Directory has not been seen, add it to the vector
            dirs_last_to_first.push_back(dir);
            set_dir1.insert(kp);
        }
    }

    // build up reverse unique list
    // current directory is okay here
    set<KeyedPath> set_dir2;
    for (auto rit = dirs.rbegin(); rit != dirs.rend(); ++rit)
    {
        auto dir = *rit;
        KeyedPath kp(dir, options.ignore_case);
        // Check to see if this directory has already been seen
        if (set_dir2.find(kp) == set_dir2.end())
        {
            // Directory has not been seen, add it to the vector
            dirs_first_to_last.push_back(dir);
            set_dir2.insert(kp);
        }
    }

    // Third, build up the vector of most common directories
    typedef map<KeyedPath, CommonPath> MapCommonPaths;
    MapCommonPaths common_paths;
    for (const auto& dir : dirs)
    {
        KeyedPath kp(dir, options.ignore_case);
        auto it = common_paths.find(kp);
        if (it == common_paths.end())
        {
            common_paths.insert(MapCommonPaths::value_type(kp, CommonPath(1, common_paths.size(), kp)));
        }
        else
        {
            it->second.count++;
        }
    }
    dirs_most_to_least.reserve(common_paths.size());
    std::transform(common_paths.begin(), common_paths.end(), std::back_inserter(dirs_most_to_least), //
                   [](auto& pair) { return std::move(pair.second); });
    sort(dirs_most_to_least.begin(), dirs_most_to_least.end());
}

void Cdd2::process(void)
{
    if (!options.unmatched_args.empty())
    {
        change_to_path_spec();
    }
}

bool Cdd2::change_to_path_spec(void)
{
    bool rc = false;

    // Save the original specification before any changes applied
    // opt_path_original = opt_path; // TODO - old - remove

    // TODO may consider if there are multiple parts of the target path separated by spaces
    fs::path path_target = options.unmatched_args[0];

    fs::path path_found;
    vector<string> path_extra;
    stringstream path_error;
    if (process_path_spec(path_target, path_found, path_extra, path_error))
    {
#ifdef WIN32
        strm_out << "pushd " << path_found << endl;
#else
        // convert to string so that quotes can be added
        strm_out << "pushd '" << path_found.string() << "'" << endl;
#endif
        // Writing to err stream gives the user feedback about the decision
        if (path_found != path_target || path_extra.size())
        {
            strm_err << "cdd: " << path_found.string() << endl;
        }
        // iterate over any extra informational paths
        for (const auto& extra : path_extra)
        {
            strm_err << extra << endl;
        }
        rc = true;
    }
    string error_msg = path_error.str();
    if (!error_msg.empty())
        strm_err << error_msg;
    return rc;
}

bool Cdd2::process_path_spec(string target, fs::path& path_found, vector<string>& path_extra, stringstream& path_error)
{
    // Process any '...' sequences in the path
    target = expand_dots(target);

    // Normal cd operation: if it is a directory, change to it
    if (!target.empty())
    {
        if (is_directory(target))
        {
            path_found = target;
            return true;
        }

        // If it is a file, change to the directory containing the file
        if (is_regular_file(target))
        {
            path_found = fs::path(target).parent_path();
            return true;
        }
    }

    if (dirs.empty())
    {
        path_error << "No history of directories" << endl;
        return false;
    }

    static std::regex re_num("(\\d+)");
    static std::regex re_dashes("-+");
    static std::regex re_two_or_more_dashes("--+");
    static std::regex re_dash_num("-(\\d+)");
    static std::regex re_pluses("[+]+");
    static std::regex re_plus_num("[+](\\d+)");
    static std::regex re_commas(",+");
    static std::regex re_comma_num(",(\\d+)");
    std::smatch match;
    if (std::regex_match(target, match, re_num))
    {
        int amount = std::stoi(match[1]);
        if (direction.is_backwards())
            return go_backwards(amount, path_found, path_error);
        // TODO put back
        // if (direction.is_forwards())
        //     return go_forwards(amount, path_found, path_error);
        // if (direction.is_common())
        //     return go_common(amount, path_found, path_error);
        return false;
    }

    // backwards
    if (std::regex_match(target, match, re_dash_num))
    {
        int amount = std::stoi(match[1]);
        return go_backwards(amount, path_found, path_error);
    }
    if (std::regex_match(target, match, re_dashes))
    {
        int amount = match[0].str().size();
        return go_backwards(amount, path_found, path_error);
    }

    // forwards
    if (std::regex_match(target, match, re_plus_num))
    {
        int amount = std::stoi(match[1]);
        return go_forwards(amount, path_found, path_error);
    }
    if (std::regex_match(target, match, re_pluses))
    {
        int amount = match[0].str().size();
        return go_forwards(amount - 1, path_found, path_error);
    }

    // common (comma)
    if (std::regex_match(target, match, re_comma_num))
    {
        int amount = std::stoi(match[1]);
        return go_common(amount, path_found, path_error);
    }
    if (std::regex_match(target, match, re_commas))
    {
        int amount = match[0].str().size();
        return go_common(amount - 1, path_found, path_error);
    }

    // TODO ...
    return false; // should return true if no errors
}

bool Cdd2::is_directory(const fs::path& path)
{
    return fs::is_directory(path);
}

bool Cdd2::is_regular_file(const fs::path& path)
{
    return fs::is_regular_file(path);
}

bool Cdd2::go_backwards(unsigned amount, fs::path& path_found, stringstream& path_error)
{
    if (amount < 1 || amount > dirs_last_to_first.size())
    {
        path_error << "No directory at -" << amount << endl;
        return false;
    }
    path_found = dirs_last_to_first[amount - 1];
    return true;
}

bool Cdd2::go_forwards(unsigned amount, fs::path& path_found, stringstream& path_error)
{
    if (amount < 0 || amount >= dirs_first_to_last.size())
    {
        path_error << "No directory at +" << amount << endl;
        return false;
    }
    path_found = dirs_first_to_last[amount];
    return true;
}

bool Cdd2::go_common(unsigned amount, fs::path& path_found, stringstream& path_error)
{
    if (amount < 0 || amount >= dirs_most_to_least.size())
    {
        path_error << "No directory at ," << amount << endl;
        return false;
    }
    path_found = dirs_most_to_least[amount].get_keyed_path().get_dir_path();
    return true;
}
