#include "cdd2.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <set>

#include "cdd_util.h"
#include "fzf_wrapper.h"

namespace fs = std::filesystem;

//----------------------------------------------------------------------

std::string Cdd2::KeyedPath::generate_key_from_path(const fs::path& p, bool ignore_case)
{
    std::string s = p.generic_string();
    if (ignore_case)
    {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
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
        kp_cwd = KeyedPath(cwd, options_.ignore_case);
    }
    set<KeyedPath> set_dir;

    for (const auto& dir : dirs_)
    {
        KeyedPath kp(dir, options_.ignore_case);

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
    for (auto rit = dirs_.rbegin(); rit != dirs_.rend(); ++rit)
    {
        auto dir = *rit;
        KeyedPath kp(dir, options_.ignore_case);
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

    for (const auto& dir : dirs_)
    {
        KeyedPath kp(dir, options_.ignore_case);
        auto it = common_paths.find(kp);
        if (it == common_paths.end())
        {
            common_paths.insert(MapCommonPaths::value_type(kp, CommonPath(1, static_cast<int>(common_paths.size()), kp)));
        }
        else
        {
            it->second.count++;
        }
    }
    result.reserve(common_paths.size());
    std::transform(common_paths.begin(), common_paths.end(),
                   std::back_inserter(result), //
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

std::vector<Cdd2::TaggedPath> Cdd2::create_dirs_upwards()
{
    std::vector<TaggedPath> result;
    fs::path cwd;
    if (!get_cwd_path(cwd))
    {
        return result; // error
    }

    std::regex re;
    std::string target;
    bool use_regex = false;
    if (get_target(target))
    {
        re = get_upwards_regex(target);
        use_regex = true;
    }

    auto path_components = get_path_components(cwd);
    size_t count = 0;
    for (const auto& [part_str, part_path] : path_components)
    {
        if (count++ == 0)
        {
            continue; // skip the current directory
        }

        // if there is a target, filter by it
        if (use_regex)
        {
            if (!std::regex_search(part_str, re))
            {
                continue;
            }
        }

        string prefix = ".." + std::to_string(count - 1);
        result.emplace_back(part_path, prefix);
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
            strm_err_ << "** cdd: error getting current working directory: " << ec.message() << endl;
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
    if (options_.unmatched_args.empty())
    {
        return false;
    }
    target = options_.unmatched_args[0];
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
        if (options_.ignore_case)
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
        strm_err_ << "Regex error: " << e.what() << std::endl;
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
    return results;
}

std::vector<Cdd2::TaggedPath> Cdd2::filter_dirs_first_to_last(const std::optional<RegexFilter>& rf)
{
    std::vector<TaggedPath> results;
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

void Cdd2::process()
{
    if (options_.list_history)
    {
        show_history();
        return;
    }

    if (options_.use_fzf)
    {
        filter_with_fzf();
        return;
    }

    if (options_.reset_history)
    {
        process_reset();
        return;
    }

    if (options_.garbage_collect)
    {
        garbage_collect();
        return;
    }

    if (options_.delete_entry)
    {
        process_delete();
        return;
    }

    if (!options_.unmatched_args.empty())
    {
        change_to_path_spec();
        return;
    }

    // default action is to show history
    show_history();
}

bool Cdd2::change_to_path_spec()
{
    bool rc = false;
    string target;
    if (!get_target(target))
    {
        strm_err_ << "cdd: no target specified" << endl;
        return rc;
    }
    fs::path path_target = target;
    TaggedPath tagged_path;
    vector<string> path_extra;

    if (process_path_spec_including_filesystem(path_target.string(), tagged_path, path_extra))
    {
#ifdef WIN32
        strm_out_ << "pushd " << tagged_path.path << endl;
#else
        strm_out_ << "pushd '" << tagged_path.path.string() << "'" << endl;
#endif

        if (tagged_path.path != path_target || path_extra.size())
        {
            // explain the action taken
            // don't print prefix1 here
            strm_err_ << "cdd:";
            if (!tagged_path.prefix2.empty())
            {
                strm_err_ << ' ' << tagged_path.prefix2;
            }
            strm_err_ << ' ' << tagged_path.path.string() << endl;
        }
        for (const auto& extra : path_extra)
        {
            strm_err_ << extra << endl;
        }
        rc = true;
    }
    return rc;
}

bool Cdd2::process_path_spec_including_filesystem(string target, TaggedPath& tagged_path, vector<string>& path_extra)
{
    if (options_.direction == CddOptions::direction_upwards)
    {
        if (process_path_spec_searching_upwards(tagged_path))
        {
            return true;
        }
    }

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

bool Cdd2::process_path_spec_searching_upwards(TaggedPath& tagged_path)
{
    fs::path cwd;
    if (!get_cwd_path(cwd))
    {
        // need current working directory
        return false;
    }

    if (options_.unmatched_args.size() == 0)
    {
        // need extra parameter to match against
        return false;
    }

    string pattern = options_.unmatched_args[0];
    std::regex re = get_upwards_regex(pattern);

    // look at all the components of the target path
    // if there is a match with the pattern, return that path

    auto path_components = get_path_components(cwd);
    for (const auto& [part_str, part_path] : path_components)
    {
        if (std::regex_search(part_str, re))
        {
            tagged_path.path = part_path;
            return true;
        }
    }

    return false;
}

std::regex Cdd2::get_upwards_regex(string pattern)
{
    // if pattern ends with path separator, anchor match to end of string
    if (!pattern.empty() && (pattern.back() == fs::path::preferred_separator))
    {
        pattern.pop_back();
        pattern = pattern + "$";
    }

    std::regex re;
    try
    {
        if (options_.ignore_case)
        {
            re = std::regex(pattern, std::regex_constants::icase);
        }
        else
        {
            re = std::regex(pattern);
        }
    }
    catch (std::regex_error& e)
    {
        stringstream strm;
        strm << "Cannot process pattern \"" << pattern << "\" due to regex error: " << e.what();
        throw std::runtime_error(strm.str());
    }
    return re;
}

bool Cdd2::process_path_spec_only_from_history(string target, TaggedPath& tagged_path, vector<string>& path_extra)
{
    if (dirs_.empty())
    {
        strm_err_ << "No history of directories" << endl;
        return false;
    }

    // [Regex setup for numerics omitted for brevity, assuming existing logic
    // remains]
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
        if (options_.direction == CddOptions::direction_backwards)
        {
            return go_backwards(amount, tagged_path);
        }
        if (options_.direction == CddOptions::direction_forwards)
        {
            return go_forwards(amount, tagged_path);
        }
        if (options_.direction == CddOptions::direction_common)
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
        int amount = static_cast<int>(match[0].str().size());
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
        int amount = static_cast<int>(match[0].str().size());
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
        int amount = static_cast<int>(match[0].str().size());
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
    size_t entry_count = options_.max_history;
    try
    {
        auto rf = get_target_regex_filter();
        if (options_.direction == CddOptions::direction_backwards)
        {
            matches = filter_dirs_last_to_first(rf);
            showing_direction = "last";
            entry_count = std::min(entry_count, options_.max_backwards);
        }
        else if (options_.direction == CddOptions::direction_forwards)
        {
            matches = filter_dirs_first_to_last(rf);
            showing_direction = "first";
            entry_count = std::min(entry_count, options_.max_forwards);
        }
        else if (options_.direction == CddOptions::direction_common)
        {
            matches = filter_dirs_most_to_least(rf);
            showing_direction = "top";
            entry_count = std::min(entry_count, options_.max_common);
        }
        else
        {
            strm_err_ << "Cannot process direction: " << options_.direction << endl;
            return false;
        }
    }
    catch (std::regex_error& e)
    {
        strm_err_ << "Cannot process pattern: '" << target << "'" << endl << e.what() << endl;
        return false;
    }

    if (matches.empty())
    {
        strm_err_ << "Cannot match pattern: '" << target << "'" << endl;
        return false;
    }

    // The first match is the target
    tagged_path = matches[0];

    // Remaining matches (if any) are added to info output
    // Note: Skip index 0 in the formatting loop because index 0 is the jump
    // target
    size_t count = 0;
    if (options_.all_history)
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
    auto tagged_paths = create_dirs_last_to_first();
    if (amount < 1 || amount > tagged_paths.size())
    {
        strm_err_ << "No directory at -" << amount << endl;
        return false;
    }
    tagged_path = tagged_paths[amount - 1];
    return true;
}

bool Cdd2::go_forwards(unsigned amount, TaggedPath& tagged_path)
{
    auto tagged_paths = create_dirs_first_to_last();
    if (amount >= tagged_paths.size())
    {
        strm_err_ << "No directory at +" << amount << endl;
        return false;
    }
    tagged_path = tagged_paths[amount];
    return true;
}

bool Cdd2::go_common(unsigned amount, TaggedPath& tagged_path)
{
    auto tagged_paths = create_dirs_most_to_least();
    if (amount >= tagged_paths.size())
    {
        strm_err_ << "No directory at ," << amount << endl;
        return false;
    }
    tagged_path = TaggedPath(tagged_paths[amount].get_keyed_path().get_dir_path(),                //
                             tagged_paths[amount].tag_prefix1, tagged_paths[amount].tag_prefix2); //
    return true;
}

//----------------------------------------------------------------------
// Display Methods (Updated to use create_*)
//----------------------------------------------------------------------

void Cdd2::show_history()
{
    if (options_.direction == CddOptions::direction_backwards)
    {
        show_history_last_to_first(strm_err_, options_.all_history);
    }
    else if (options_.direction == CddOptions::direction_forwards)
    {
        show_history_first_to_last(strm_err_, options_.all_history);
    }
    else if (options_.direction == CddOptions::direction_common)
    {
        show_history_most_to_least(strm_err_, options_.all_history);
    }
    else if (options_.direction == CddOptions::direction_upwards)
    {
        show_history_upwards(strm_err_, options_.all_history);
    }
}

void Cdd2::filter_with_fzf()
{
    stringstream strm;
    char trim_char = ':';
    if (options_.direction == CddOptions::direction_backwards)
    {
        show_history_last_to_first(strm, true);
    }
    else if (options_.direction == CddOptions::direction_forwards)
    {
        show_history_first_to_last(strm, true);
    }
    else if (options_.direction == CddOptions::direction_common)
    {
        trim_char = ')';
        show_history_most_to_least(strm, true);
    }
    else if (options_.direction == CddOptions::direction_upwards)
    {
        show_history_upwards(strm, true);
    }

    if (!strm.str().empty())
    {
        auto result = run_fzf(strm.str());
        if (!result.empty())
        {
            strm_err_ << "cdd: " << trim_from_char(result, ':') << endl;
            // TODO : handle Windows quoting
            auto dir = trim_from_char(result, trim_char);
#ifdef WIN32
            strm_out_ << "pushd \"" << dir << '"' << endl;
#else
            strm_out_ << "pushd '" << dir << "'" << endl;
#endif
        }
    }
    else
    {
        strm_err_ << "** No history to filter with fzf" << endl;
    }
}

bool Cdd2::verify_history_matches(const std::vector<TaggedPath>& matches, const std::optional<RegexFilter>& rf)
{
    if (matches.empty())
    {
        if (rf.has_value())
        {
            strm_err_ << "** No history of directories matching: " << rf->target << endl;
        }
        else
        {
            strm_err_ << "** No history of directories" << endl;
        }
        return false;
    }
    return true;
}

void Cdd2::show_history_last_to_first(std::ostream& strm, bool all_output)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_last_to_first(rf);
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t count_output = 0;
    size_t max_output = all_output ? matches.size() : std::min(options_.max_history, options_.max_backwards);
    for (const auto& filtered_path : matches)
    {
        strm << filtered_path.to_string() << endl;
        if (++count_output >= max_output)
        {
            break;
        }
    }
    if (count_output < matches.size())
    {
        strm << " ... showing last " << count_output << " of " << matches.size() << endl;
    }
}

void Cdd2::show_history_first_to_last(std::ostream& strm, bool all_output)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_first_to_last(rf);
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t count_output = 0;
    size_t max_output = all_output ? matches.size() : std::min(options_.max_history, options_.max_forwards);
    for (const auto& filtered_path : matches)
    {
        strm << filtered_path.to_string() << endl;
        if (++count_output >= max_output)
        {
            break;
        }
    }
    if (count_output < matches.size())
    {
        strm << " ... showing first " << count_output << " of " << matches.size() << endl;
    }
}

void Cdd2::show_history_most_to_least(std::ostream& strm, bool all_output)
{
    auto rf = get_target_regex_filter();
    auto matches = filter_dirs_most_to_least(rf);
    if (!verify_history_matches(matches, rf))
    {
        return;
    }

    size_t count_output = 0;
    size_t max_output = all_output ? matches.size() : std::min(options_.max_history, options_.max_common);
    for (const auto& filtered_path : matches)
    {
        strm << filtered_path.to_string() << endl;
        if (++count_output >= max_output)
        {
            break;
        }
    }
    if (count_output < matches.size())
    {
        strm << " ... showing top " << count_output << " of " << matches.size() << endl;
    }
}

void Cdd2::show_history_upwards(std::ostream& strm, bool all_output)
{
    auto matches = create_dirs_upwards();
    if (matches.empty())
    {
        strm_err_ << "** No parent directories from current working directory" << endl;
        return;
    }

    size_t count_output = 0;
    size_t max_output = all_output ? matches.size() : std::min(options_.max_history, static_cast<size_t>(matches.size()));
    for (const auto& filtered_path : matches)
    {
        strm << filtered_path.to_string() << endl;
        if (++count_output >= max_output)
        {
            break;
        }
    }
    if (count_output < matches.size())
    {
        strm << " ... showing " << count_output << " of " << matches.size() << endl;
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

void Cdd2::process_reset()
{
    vector<fs::path> empty_paths;
    command_generator(empty_paths);
    strm_err_ << "cdd reset" << endl;
}

void Cdd2::garbage_collect()
{
    auto tagged_paths = create_dirs_first_to_last();
    if (tagged_paths.empty() || tagged_paths.size() == dirs_.size())
    {
        strm_err_ << "** Nothing to gc" << endl;
        return;
    }
    vector<fs::path> paths;
    paths.reserve(tagged_paths.size());
    for (const auto& tp : tagged_paths)
    {
        paths.push_back(tp.path);
    }
    command_generator(paths);
    strm_err_ << "cdd reduced dirs: from " << dirs_.size() << " to " << paths.size() << endl;
}

void Cdd2::process_delete()
{
    string target;
    if (!get_target(target))
    {
        strm_err_ << "cdd: delete requires a target" << endl;
        return;
    }

    TaggedPath tagged_path;
    vector<string> path_extra;
    if (!process_path_spec_only_from_history(target, tagged_path, path_extra))
    {
        // Here: error
        strm_err_ << "** Could not resolve for delete: " << target << endl;
        return;
    }

    // reverse copy excluding found path
    vector<fs::path> reversed;
    for (auto rit = dirs_.rbegin(); rit != dirs_.rend(); ++rit)
    {
        auto dir = *rit;
        if (dir != tagged_path.path)
        {
            reversed.push_back(dir);
        }
    }
    if (reversed.size() == dirs_.size())
    {
        strm_err_ << "** Could not delete from history: " << tagged_path.path.string() << endl;
        return;
    }
    command_generator(reversed);
    strm_err_ << "cdd del: " << tagged_path.path.string() << endl;
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
    strm_out_ << "for /l %%i in (1,1," << pushd_count() << ") do popd" << endl;
    int count = 0;
    for (const auto& path_remaining : paths_remaining)
    {
        strm_out_ << (count++ ? "pushd " : "chdir/d ") << path_remaining << " 2>nul" << endl;
    }

    fs::path cwd;
    if (get_cwd_path(cwd))
    {
        strm_out_ << (count ? "pushd " : "chdir/d ") << cwd << endl;
    }
}

void Cdd2::command_generator_bash(const vector<fs::path>& paths_remaining)
{
    strm_out_ << "dirs -c" << endl;

    int count = 0;

    for (const auto& path_remaining : paths_remaining)
    {
        strm_out_ << (count++ ? "pushd" : "\\cd") << " '" << path_remaining.string() << "'" << endl;
    }
}

size_t Cdd2::pushd_count() const
{
    auto count = dirs_.size();
#if WIN32
    count--;
#endif
    return count;
}
