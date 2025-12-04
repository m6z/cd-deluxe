#include "cdd2.h"

#include <algorithm>
#include <iomanip>
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
        // Logic to manipulate dirs_stack or similar if needed
        // current_path_added = true;
    }
#endif
    // Logic from initialize has been moved to create_* methods
}

//----------------------------------------------------------------------
// Creation Methods (Refactored from initialize)
//----------------------------------------------------------------------

std::vector<fs::path> Cdd2::create_dirs_last_to_first()
{
    std::vector<fs::path> result;
    KeyedPath kp_cwd(cwd, options.ignore_case);
    set<KeyedPath> set_dir;

    for (const auto& dir : dirs)
    {
        KeyedPath kp(dir, options.ignore_case);

        // Check to see if this directory has already been seen
        if (set_dir.find(kp) == set_dir.end())
        {
            // Filter out the current_path after the set inclusion test,
            if (kp == kp_cwd)
            {
                continue;
            }
            // Directory has not been seen, add it to the vector
            result.push_back(dir);
            set_dir.insert(kp);
        }
    }
    return result;
}

std::vector<fs::path> Cdd2::create_dirs_first_to_last()
{
    std::vector<fs::path> result;
    set<KeyedPath> set_dir;

    // build up reverse unique list
    for (auto rit = dirs.rbegin(); rit != dirs.rend(); ++rit)
    {
        auto dir = *rit;
        KeyedPath kp(dir, options.ignore_case);
        if (set_dir.find(kp) == set_dir.end())
        {
            result.push_back(dir);
            set_dir.insert(kp);
        }
    }
    return result;
}

std::vector<Cdd2::CommonPath> Cdd2::create_dirs_most_to_least()
{
    std::vector<CommonPath> result;
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
    result.reserve(common_paths.size());
    std::transform(common_paths.begin(), common_paths.end(), std::back_inserter(result), //
                   [](auto& pair) { return std::move(pair.second); });
    sort(result.begin(), result.end());
    return result;
}

//----------------------------------------------------------------------
// Filter Helper Logic
//----------------------------------------------------------------------

bool Cdd2::get_target(string& target)
{
    if (options.unmatched_args.empty())
    {
        return false;
    }
    target = options.unmatched_args[0];
    return true;
}

std::optional<Cdd2::RegexFilter> Cdd2::get_target_regex_filter()
{
    string target;
    if (!get_target(target))
    {
        return std::nullopt;
    }

    RegexFilter rf;
    // if pattern ends with path separator, match full directory names only
    if (!target.empty() && (target.back() == fs::path::preferred_separator))
    {
        target.pop_back();
        rf.check_all_parts = false;
    }

    if (options.ignore_case)
    {
        rf.re = std::regex(target, std::regex_constants::icase);
    }
    else
    {
        rf.re = std::regex(target);
    }
    return rf;
}

// Helper function to match paths against regex
// TElement can be fs::path or CommonPath
template <typename TElement, typename Projector = std::identity>
bool check_match(const std::regex& re, bool check_all_parts, const TElement& element, const Projector& get_path = {})
{
    const auto path = get_path(element);
    if (check_all_parts)
    {
        for (const auto& part : path)
        {
            if (std::regex_search(part.string(), re))
            {
                return true;
            }
        }
    }
    else
    {
        if (path.has_filename() && std::regex_search(path.filename().string(), re))
        {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------------------
// Filter Methods (Refactored from process_match)
//----------------------------------------------------------------------

std::vector<Cdd2::FilteredPath> Cdd2::filter_dirs_last_to_first(const std::optional<RegexFilter>& rf)
{
    std::vector<FilteredPath> results;
    auto dirs = create_dirs_last_to_first();
    for (size_t i = 0; i < dirs.size(); ++i)
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, dirs[i]))
        {
            stringstream strm;
            int number = -static_cast<int>(i) - 1;
            strm << setw(3) << number << ": ";
            results.emplace_back(strm.str(), dirs[i]);
        }
    }
    return results;
}

std::vector<Cdd2::FilteredPath> Cdd2::filter_dirs_first_to_last(const std::optional<RegexFilter>& rf)
{
    std::vector<FilteredPath> results;
    auto dirs = create_dirs_first_to_last();
    for (size_t i = 0; i < dirs.size(); ++i)
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, dirs[i]))
        {
            stringstream strm;
            strm << setw(3) << i << ": ";
            results.emplace_back(strm.str(), dirs[i]);
        }
    }
    return results;
}

std::vector<Cdd2::FilteredPath> Cdd2::filter_dirs_most_to_least(const std::optional<RegexFilter>& rf)
{
    std::vector<FilteredPath> results;
    auto commons = create_dirs_most_to_least();
    auto projector = [](const CommonPath& cp) { return cp.get_keyed_path().get_dir_path(); };

    for (size_t i = 0; i < commons.size(); ++i)
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, commons[i], projector))
        {
            stringstream strm;
            if (i < 10)
                strm << " ";
            strm << "," << i << ": (" << setw(2) << commons[i].count << ") ";
            results.emplace_back(strm.str(), projector(commons[i]));
        }
    }
    return results;
}

//----------------------------------------------------------------------
// Processing
//----------------------------------------------------------------------

void Cdd2::process(void)
{
    if (options.list_history)
    {
        show_history();
        return;
    }

    if (!options.unmatched_args.empty())
    {
        change_to_path_spec();
        return;
    }

    // default action is to show history
    show_history();
}

bool Cdd2::change_to_path_spec(void)
{
    bool rc = false;
    fs::path path_target = options.unmatched_args[0];
    fs::path path_found;
    vector<string> path_extra;

    if (process_path_spec(path_target, path_found, path_extra))
    {
#ifdef WIN32
        strm_out << "pushd " << path_found << endl;
#else
        strm_out << "pushd '" << path_found.string() << "'" << endl;
#endif
        if (path_found != path_target || path_extra.size())
        {
            strm_err << "cdd: " << path_found.string() << endl;
        }
        for (const auto& extra : path_extra)
        {
            strm_err << extra << endl;
        }
        rc = true;
    }
    return rc;
}

bool Cdd2::process_path_spec(string target, fs::path& path_found, vector<string>& path_extra)
{
    target = expand_dots(target);

    if (!target.empty())
    {
        if (is_directory(target))
        {
            path_found = target;
            return true;
        }
        if (is_regular_file(target))
        {
            path_found = fs::path(target).parent_path();
            return true;
        }
    }

    if (dirs.empty())
    {
        strm_err << "No history of directories" << endl;
        return false;
    }

    // [Regex setup for numerics omitted for brevity, assuming existing logic remains]
    static std::regex re_num("(\\d+)");
    static std::regex re_dashes("-+");
    static std::regex re_dash_num("-(\\d+)");
    static std::regex re_pluses("[+]+");
    static std::regex re_plus_num("[+](\\d+)");
    static std::regex re_commas(",+");
    static std::regex re_comma_num(",(\\d+)");

    std::smatch match;

    if (std::regex_match(target, match, re_num))
    {
        int amount = std::stoi(match[1]);
        if (options.direction == CddOptions::direction_backwards)
        {
            return go_backwards(amount, path_found);
        }
        if (options.direction == CddOptions::direction_forwards)
        {
            return go_forwards(amount, path_found);
        }
        if (options.direction == CddOptions::direction_common)
        {
            return go_common(amount, path_found);
        }
        return false;
    }

    // backwards
    if (std::regex_match(target, match, re_dash_num))
    {
        int amount = std::stoi(match[1]);
        return go_backwards(amount, path_found);
    }
    if (std::regex_match(target, match, re_dashes))
    {
        int amount = match[0].str().size();
        return go_backwards(amount, path_found);
    }

    // forwards
    if (std::regex_match(target, match, re_plus_num))
    {
        int amount = std::stoi(match[1]);
        return go_forwards(amount, path_found);
    }
    if (std::regex_match(target, match, re_pluses))
    {
        int amount = match[0].str().size();
        return go_forwards(amount - 1, path_found);
    }

    // common (comma)
    if (std::regex_match(target, match, re_comma_num))
    {
        int amount = std::stoi(match[1]);
        return go_common(amount, path_found);
    }
    if (std::regex_match(target, match, re_commas))
    {
        int amount = match[0].str().size();
        return go_common(amount - 1, path_found);
    }

    return process_match(target, path_found, path_extra);
}

//----------------------------------------------------------------------
// Refactored process_match
//----------------------------------------------------------------------

bool Cdd2::process_match(const string& target, fs::path& path_found, vector<string>& path_extra)
{
    vector<FilteredPath> matches;

    std::string showing_direction;
    size_t entry_count = options.max_history;
    try
    {
        auto rf = get_target_regex_filter();
        if (options.direction == CddOptions::direction_backwards)
        {
            matches = filter_dirs_last_to_first(rf);
            showing_direction = "last";
            entry_count = std::min(entry_count, options.max_backwards);
        }
        else if (options.direction == CddOptions::direction_forwards)
        {
            matches = filter_dirs_first_to_last(rf);
            showing_direction = "first";
            entry_count = std::min(entry_count, options.max_forwards);
        }
        else if (options.direction == CddOptions::direction_common)
        {
            matches = filter_dirs_most_to_least(rf);
            showing_direction = "top";
            entry_count = std::min(entry_count, options.max_common);
        }
        else
        {
            strm_err << "Cannot process direction: " << options.direction << endl;
            return false;
        }
    }
    catch (std::regex_error& e)
    {
        strm_err << "Cannot process pattern: '" << target << "'" << endl << e.what() << endl;
        return false;
    }

    if (matches.empty())
    {
        strm_err << "Cannot match pattern: '" << target << "'" << endl;
        return false;
    }

    // The first match is the target
    path_found = matches[0].path;

    // Remaining matches (if any) are added to info output
    // Note: Skip index 0 in the formatting loop because index 0 is the jump target
    size_t count = 0;
    if (options.all_history)
    {
        entry_count = matches.size();
    }
    entry_count = std::min(entry_count + 1, matches.size());
    for (size_t i = 1; i < entry_count; ++i)
    {
        path_extra.push_back(matches[i].prefix + matches[i].path.string());
        count++;
    }
    if (count + 1 < matches.size())
    {
        path_extra.push_back(" ... showing " + showing_direction + " " + std::to_string(count) + " of " + std::to_string(matches.size()));
    }

    return true;
}

//----------------------------------------------------------------------
// Navigation Methods (Updated to use create_*)
//----------------------------------------------------------------------

bool Cdd2::go_backwards(unsigned amount, fs::path& path_found)
{
    auto dirs = create_dirs_last_to_first();
    if (amount < 1 || amount > dirs.size())
    {
        strm_err << "No directory at -" << amount << endl;
        return false;
    }
    path_found = dirs[amount - 1];
    return true;
}

bool Cdd2::go_forwards(unsigned amount, fs::path& path_found)
{
    auto dirs = create_dirs_first_to_last();
    if (amount < 0 || amount >= dirs.size())
    {
        strm_err << "No directory at +" << amount << endl;
        return false;
    }
    path_found = dirs[amount];
    return true;
}

bool Cdd2::go_common(unsigned amount, fs::path& path_found)
{
    auto dirs = create_dirs_most_to_least();
    if (amount < 0 || amount >= dirs.size())
    {
        strm_err << "No directory at ," << amount << endl;
        return false;
    }
    path_found = dirs[amount].get_keyed_path().get_dir_path();
    return true;
}

//----------------------------------------------------------------------
// Display Methods (Updated to use create_*)
//----------------------------------------------------------------------

void Cdd2::show_history(void)
{
    if (options.direction == CddOptions::direction_backwards)
    {
        show_history_last_to_first();
    }
    else if (options.direction == CddOptions::direction_forwards)
    {
        show_history_first_to_last();
    }
    else if (options.direction == CddOptions::direction_common)
    {
        show_history_most_to_least();
    }
}

void Cdd2::show_history_last_to_first(void)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_last_to_first(rf);
    size_t entry_count = std::min(options.max_history, options.max_backwards);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.prefix << filtered_path.path.string() << endl;
        if (++count >= entry_count && entry_count > 0 && !options.all_history)
        {
            break;
        }
    }
    if (count < matches.size())
    {
        strm_err << " ... showing last " << count << " of " << matches.size() << endl;
    }
}

void Cdd2::show_history_first_to_last(void)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_first_to_last(rf);
    size_t entry_count = std::min(options.max_history, options.max_forwards);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.prefix << filtered_path.path.string() << endl;
        if (++count >= entry_count && entry_count > 0 && !options.all_history)
        {
            break;
        }
    }
    if (count < matches.size())
    {
        strm_err << " ... showing first " << count << " of " << matches.size() << endl;
    }
}

void Cdd2::show_history_most_to_least(void)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_most_to_least(rf);
    size_t entry_count = std::min(options.max_history, options.max_common);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.prefix << filtered_path.path.string() << endl;
        if (++count >= entry_count && entry_count > 0 && !options.all_history)
        {
            break;
        }
    }
    if (count < matches.size())
    {
        strm_err << " ... showing top " << count << " of " << matches.size() << endl;
    }
}

bool Cdd2::is_directory(const fs::path& path)
{
    return fs::is_directory(path);
}

bool Cdd2::is_regular_file(const fs::path& path)
{
    return fs::is_regular_file(path);
}
