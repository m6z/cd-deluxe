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
// Creation Methods (Refactored from initialize)
//----------------------------------------------------------------------

std::vector<Cdd2::TaggedPath> Cdd2::create_dirs_last_to_first()
{
    std::vector<TaggedPath> result;
    fs::path cwd;
    KeyedPath kp_cwd;
    if (get_cwd_path(cwd))
    {
        kp_cwd = KeyedPath(cwd, options.ignore_case);
    }
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

            // generate the prefix
            int number = -static_cast<int>(result.size() + 1);
            stringstream strm;
            strm << setw(3) << number;

            result.emplace_back(dir, strm.str());
            set_dir.insert(kp);
        }
    }
    return result;
}

std::vector<Cdd2::TaggedPath> Cdd2::create_dirs_first_to_last()
{
    std::vector<TaggedPath> result;
    set<KeyedPath> set_dir;

    // build up reverse unique list
    for (auto rit = dirs.rbegin(); rit != dirs.rend(); ++rit)
    {
        auto dir = *rit;
        KeyedPath kp(dir, options.ignore_case);
        if (set_dir.find(kp) == set_dir.end())
        {
            stringstream strm;
            strm << setw(3) << result.size();
            result.emplace_back(dir, strm.str());
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

    // need to assign tag_prefixes
    for (size_t i = 0; i < result.size(); ++i)
    {
        // relative offset
        {
            stringstream strm;
            if (i < 10)
                strm << ' ';
            strm << ',' << i;
            result[i].tag_prefix1 = strm.str();
        }
        // count
        {
            stringstream strm;
            strm << '(' << setw(2) << result[i].count << ')';
            result[i].tag_prefix2 = strm.str();
        }
    }

    return result;
}

//----------------------------------------------------------------------
// Filter Helper Logic
//----------------------------------------------------------------------

bool Cdd2::get_cwd_path(fs::path& cwd)
{
    if (!cwd_assigned_ && !cwd_retrieved_)
    {
        // make sure to only try once
        cwd_retrieved_ = true;
        error_code ec;
        cwd_ = fs::current_path(ec);
        if (ec)
        {
            strm_err << "** cdd: error getting current working directory: " << ec.message() << endl;
            return false;
        }
        cwd_assigned_ = true;
    }

    if (cwd_assigned_)
    {
        cwd = cwd_;
        return true;
    }
    else
    {
        return false;
    }
}

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
    RegexFilter rf;
    if (!get_target(rf.target))
    {
        return std::nullopt;
    }

    // if pattern ends with path separator, match full directory names only
    if (!rf.target.empty() && (rf.target.back() == fs::path::preferred_separator))
    {
        rf.target.pop_back();
        rf.check_all_parts = false;
    }

    try
    {
        if (options.ignore_case)
        {
            rf.re = std::regex(rf.target, std::regex_constants::icase);
        }
        else
        {
            rf.re = std::regex(rf.target);
        }
    }
    catch (std::regex_error& e)
    {
        // print error to std::cerr and return nullopt
        cerr << "Regex error: " << e.what() << std::endl;
        return std::nullopt;
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

std::vector<Cdd2::TaggedPath> Cdd2::filter_dirs_last_to_first(const std::optional<RegexFilter>& rf)
{
    std::vector<TaggedPath> results;
    auto tagged_paths = create_dirs_last_to_first();
    for (const auto& tp : tagged_paths)
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, tp.path))
        {
            results.push_back(tp);
        }
    }

    // TODO - old - remove
    // for (size_t i = 0; i < tagged_paths.size(); ++i)
    // {
    //     if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, tagged_paths[i].path))
    //     {
    //         stringstream strm;
    //         int number = -static_cast<int>(i) - 1;
    //         strm << setw(3) << number << ": ";
    //         results.emplace_back(strm.str(), dirs[i]);
    //     }
    // }
    return results;
}

std::vector<Cdd2::TaggedPath> Cdd2::filter_dirs_first_to_last(const std::optional<RegexFilter>& rf)
{
    std::vector<TaggedPath> results;

    // TODO - old - remove
    // auto dirs = create_dirs_first_to_last();
    // for (size_t i = 0; i < dirs.size(); ++i)
    // {
    //     if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, dirs[i]))
    //     {
    //         stringstream strm;
    //         strm << setw(3) << i << ": ";
    //         results.emplace_back(strm.str(), dirs[i]);
    //     }
    // }

    for (const auto& tp : create_dirs_first_to_last())
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, tp.path))
        {
            results.push_back(tp);
        }
    }
    return results;
}

std::vector<Cdd2::TaggedPath> Cdd2::filter_dirs_most_to_least(const std::optional<RegexFilter>& rf)
{
    std::vector<TaggedPath> results;
    auto commons = create_dirs_most_to_least();
    auto projector = [](const CommonPath& cp) { return cp.get_keyed_path().get_dir_path(); };

    // TODO - old - remove
    // for (size_t i = 0; i < commons.size(); ++i)
    // {
    //     if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, commons[i], projector))
    //     {
    //         stringstream strm;
    //         if (i < 10)
    //             strm << " ";
    //         strm << "," << i << ": (" << setw(2) << commons[i].count << ") ";
    //         results.emplace_back(strm.str(), projector(commons[i]));
    //     }
    // }

    for (const auto& cp : commons)
    {
        if (!rf.has_value() || check_match(rf->re, rf->check_all_parts, cp, projector))
        {
            results.emplace_back(projector(cp), cp.tag_prefix1, cp.tag_prefix2);
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

    if (options.reset_history)
    {
        process_reset();
        return;
    }

    if (options.garbage_collect)
    {
        garbage_collect();
        return;
    }

    if (options.delete_entry)
    {
        process_delete();
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
    string target;
    if (!get_target(target))
    {
        strm_err << "cdd: no target specified" << endl;
        return rc;
    }
    fs::path path_target = target;
    TaggedPath tagged_path;
    vector<string> path_extra;

    if (process_path_spec_including_filesystem(path_target, tagged_path, path_extra))
    {
#ifdef WIN32
        strm_out << "pushd " << tagged_path.path << endl;
#else
        strm_out << "pushd '" << tagged_path.path.string() << "'" << endl;
#endif
        if (tagged_path.path != path_target || path_extra.size())
        {
            // explain the action taken
            // don't print prefix1 here
            strm_err << "cdd:";
            if (!tagged_path.prefix2.empty())
            {
                strm_err << ' ' << tagged_path.prefix2;
            }
            strm_err << ' ' << tagged_path.path.string() << endl;
        }
        for (const auto& extra : path_extra)
        {
            strm_err << extra << endl;
        }
        rc = true;
    }
    return rc;
}

bool Cdd2::process_path_spec_including_filesystem(string target, TaggedPath& tagged_path, vector<string>& path_extra)
{
    target = expand_dots(target);

    if (!target.empty())
    {
        if (is_directory(target))
        {
            tagged_path.path = target;
            return true;
        }
        if (is_regular_file(target))
        {
            tagged_path.path = fs::path(target).parent_path();
            return true;
        }
    }
    return process_path_spec_only_from_history(target, tagged_path, path_extra);
}

bool Cdd2::process_path_spec_only_from_history(string target, TaggedPath& tagged_path, vector<string>& path_extra)
{
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
            return go_backwards(amount, tagged_path);
        }
        if (options.direction == CddOptions::direction_forwards)
        {
            return go_forwards(amount, tagged_path);
        }
        if (options.direction == CddOptions::direction_common)
        {
            return go_common(amount, tagged_path);
        }
        return false;
    }

    // backwards
    if (std::regex_match(target, match, re_dash_num))
    {
        int amount = std::stoi(match[1]);
        return go_backwards(amount, tagged_path);
    }
    if (std::regex_match(target, match, re_dashes))
    {
        int amount = match[0].str().size();
        return go_backwards(amount, tagged_path);
    }

    // forwards
    if (std::regex_match(target, match, re_plus_num))
    {
        int amount = std::stoi(match[1]);
        return go_forwards(amount, tagged_path);
    }
    if (std::regex_match(target, match, re_pluses))
    {
        int amount = match[0].str().size();
        return go_forwards(amount - 1, tagged_path);
    }

    // common (comma)
    if (std::regex_match(target, match, re_comma_num))
    {
        int amount = std::stoi(match[1]);
        return go_common(amount, tagged_path);
    }
    if (std::regex_match(target, match, re_commas))
    {
        int amount = match[0].str().size();
        return go_common(amount - 1, tagged_path);
    }

    return process_match(target, tagged_path, path_extra);
}

//----------------------------------------------------------------------
// Refactored process_match
//----------------------------------------------------------------------

bool Cdd2::process_match(const string& target, TaggedPath& tagged_path, vector<string>& path_extra)
{
    vector<TaggedPath> matches;

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
    tagged_path = matches[0];

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
        path_extra.push_back(matches[i].to_string());
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

bool Cdd2::go_backwards(unsigned amount, TaggedPath& tagged_path)
{
    auto dirs = create_dirs_last_to_first();
    if (amount < 1 || amount > dirs.size())
    {
        strm_err << "No directory at -" << amount << endl;
        return false;
    }
    tagged_path = dirs[amount - 1];
    return true;
}

bool Cdd2::go_forwards(unsigned amount, TaggedPath& tagged_path)
{
    auto dirs = create_dirs_first_to_last();
    if (amount >= dirs.size())
    {
        strm_err << "No directory at +" << amount << endl;
        return false;
    }
    tagged_path = dirs[amount];
    return true;
}

bool Cdd2::go_common(unsigned amount, TaggedPath& tagged_path)
{
    auto dirs = create_dirs_most_to_least();
    if (amount >= dirs.size())
    {
        strm_err << "No directory at ," << amount << endl;
        return false;
    }
    tagged_path = TaggedPath(dirs[amount].get_keyed_path().get_dir_path(), dirs[amount].tag_prefix1, dirs[amount].tag_prefix2);
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

bool Cdd2::verify_history_matches(const std::vector<TaggedPath>& matches, const std::optional<RegexFilter>& rf)
{
    if (matches.empty())
    {
        if (rf.has_value())
        {
            strm_err << "** No history of directories matching: " << rf->target << endl;
        }
        else
        {
            strm_err << "** No history of directories" << endl;
        }
        return false;
    }
    return true;
}

void Cdd2::show_history_last_to_first(void)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_last_to_first(rf);
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t entry_count = std::min(options.max_history, options.max_backwards);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.to_string() << endl;
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
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t entry_count = std::min(options.max_history, options.max_forwards);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.to_string() << endl;
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
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t entry_count = std::min(options.max_history, options.max_common);
    size_t count = 0;
    for (const auto& filtered_path : matches)
    {
        strm_err << filtered_path.to_string() << endl;
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

void Cdd2::process_reset(void)
{
    vector<fs::path> empty_paths;
    command_generator(empty_paths);
    strm_err << "cdd reset" << endl;
}

void Cdd2::garbage_collect(void)
{
    auto tagged_paths = create_dirs_first_to_last();
    if (tagged_paths.empty() || tagged_paths.size() == this->dirs.size())
    {
        strm_err << "** Nothing to gc" << endl;
        return;
    }
    vector<fs::path> dirs;
    dirs.reserve(tagged_paths.size());
    for (const auto& tp : tagged_paths)
    {
        dirs.push_back(tp.path);
    }
    command_generator(dirs);
    strm_err << "cdd reduced dirs: from " << this->dirs.size() << " to " << dirs.size() << endl;
}

void Cdd2::process_delete(void)
{
    string target;
    if (!get_target(target))
    {
        strm_err << "cdd: delete requires a target" << endl;
        return;
    }

    TaggedPath tagged_path;
    vector<string> path_extra;
    if (!process_path_spec_only_from_history(target, tagged_path, path_extra))
    {
        // Here: error
        strm_err << "** Could not resolve for delete: " << target << endl;
        return;
    }

    // reverse copy excluding found path
    vector<fs::path> reversed;
    for (auto rit = dirs.rbegin(); rit != dirs.rend(); ++rit)
    {
        auto dir = *rit;
        if (dir != tagged_path.path)
        {
            reversed.push_back(dir);
        }
    }
    if (reversed.size() == dirs.size())
    {
        strm_err << "** Could not delete from history: " << tagged_path.path.string() << endl;
        return;
    }
    command_generator(reversed);
    strm_err << "cdd del: " << tagged_path.path.string() << endl;
}

void Cdd2::command_generator(const vector<fs::path>& paths_remaining)
{
#ifdef WIN32
    command_generator_win32(paths_remaining);
#else
    command_generator_bash(paths_remaining);
#endif
}

void Cdd2::command_generator_win32(const vector<fs::path>& paths_remaining)
{
    strm_out << "for /l %%i in (1,1," << pushd_count() << ") do popd" << endl;
    int count = 0;
    for (const auto& path_remaining : paths_remaining)
    {
        strm_out << (count++ ? "pushd " : "chdir/d ") << path_remaining << " 2>nul" << endl;
    }

    fs::path cwd;
    if (get_cwd_path(cwd))
    {
        strm_out << (count ? "pushd " : "chdir/d ") << cwd << endl;
    }
}

void Cdd2::command_generator_bash(const vector<fs::path>& paths_remaining)
{
    strm_out << "dirs -c" << endl;

    int count = 0;

    for (const auto& path_remaining : paths_remaining)
    {
        strm_out << (count++ ? "pushd" : "\\cd") << " '" << path_remaining.string() << "'" << endl;
    }
}

size_t Cdd2::pushd_count() const
{
    auto count = dirs.size();
#if WIN32
    count--;
#endif
    return count;
}
