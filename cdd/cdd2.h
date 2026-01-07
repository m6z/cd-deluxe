#pragma once

#include <exception>
#include <filesystem>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "cdd_options.h"

using namespace std;
namespace fs = std::filesystem;

class Cdd2
{
public:
    Cdd2(const CddOptions& options, const vector<string> dirs, fs::path cwd = {}) : options(options), dirs(dirs), cwd_(cwd)
    {
        if (!cwd.empty())
        {
            cwd_assigned_ = true;
        }
    }

    void process(void);
    const CddOptions& get_options() const { return options; }
    string get_out_str() const { return strm_out.str(); }
    string get_err_str() const { return strm_err.str(); }

    struct Exception : public exception
    {
        string _msg;
        Exception(const string& msg) : _msg(msg) {}
        ~Exception() throw() {}
        virtual const char* what() const throw() { return _msg.c_str(); }
    };

    struct TaggedPath
    {
        fs::path path;
        string prefix1;
        string prefix2;
        TaggedPath() = default;
        TaggedPath(fs::path path, string prefix1 = {}, string prefix2 = {}) : path(path), prefix1(prefix1), prefix2(prefix2) {}
        string to_string() const
        {
            stringstream strm;
            strm << prefix1 << ": ";
            if (!prefix2.empty())
            {
                strm << prefix2 << ' ';
            }
            strm << path.string();
            return strm.str();
        }
    };

    class KeyedPath
    {
    public:
        KeyedPath() = default;
        KeyedPath(const fs::path& p, bool ignore_case) : dir_path(p), dir_key(generate_key_from_path(p, ignore_case)) {}

        const string& get_dir_key() const { return dir_key; }
        const fs::path& get_dir_path() const { return dir_path; }

        bool operator==(const KeyedPath& other) const { return dir_key == other.dir_key; }
        auto operator<=>(const KeyedPath& other) const { return dir_key <=> other.dir_key; }

        friend ostream& operator<<(ostream& out, const KeyedPath& obj)
        {
            out << "KeyedPath(" << obj.dir_path << ", \"" << obj.dir_key << "\")";
            return out;
        }

    private:
        fs::path dir_path;
        string dir_key;

        string generate_key_from_path(const fs::path& p, bool ignore_case);
    };

    // This tracks the most common directories
    class CommonPath
    {
    public:
        int count;
        std::string tag_prefix1;
        std::string tag_prefix2;

        CommonPath(int count, int sequence, const KeyedPath& kp) : count(count), sequence(sequence), kp(kp) {}

        const KeyedPath& get_keyed_path() const { return kp; }

        bool operator==(const CommonPath& obj) const { return sequence == obj.sequence && kp == obj.kp; }
        bool operator<(const CommonPath& obj) const
        {
            // Order by count in descending order, then by sequence in ascending order
            if (count == obj.count)
                return sequence < obj.sequence;
            return count > obj.count;
        }

        friend ostream& operator<<(ostream& out, const CommonPath& obj)
        {
            out << "CommonPath(" << obj.count << "," << obj.sequence << ",\"" << obj.kp << "\")";
            return out;
        }

    private:
        int sequence;
        KeyedPath kp;
    };

    struct RegexFilter
    {
        std::string target;
        std::regex re;
        bool check_all_parts = true;
    };

protected:
    // Creation methods (Migrated from initialize)
    std::vector<TaggedPath> create_dirs_last_to_first();
    std::vector<TaggedPath> create_dirs_first_to_last();
    std::vector<CommonPath> create_dirs_most_to_least();

private:
    void initialize();

    CddOptions options;          // The options for cdd2
    vector<string> dirs;         // The raw list of pushed directories
    fs::path cwd_;               // Current working directory (use get_cwd() to access)
    bool cwd_assigned_ = false;  // Whether cwd has been assigned
    bool cwd_retrieved_ = false; // Whether cwd has been retrieved

    stringstream strm_out;
    stringstream strm_err;

    // Helper to compile regex and determine if we check all parts
    std::regex compile_regex(string target, bool& check_all_parts);

    // new
    std::vector<TaggedPath> filter_dirs_last_to_first(const std::optional<RegexFilter>& rf);
    std::vector<TaggedPath> filter_dirs_first_to_last(const std::optional<RegexFilter>& rf);
    std::vector<TaggedPath> filter_dirs_most_to_least(const std::optional<RegexFilter>& rf);

    bool get_cwd_path(fs::path& cwd);
    bool get_target(string& target);
    bool get_target_regex(std::regex& re, bool& check_all_parts);
    std::optional<RegexFilter> get_target_regex_filter();

    // old
    void assign(vector<string>& vec_pushd, string current_path);
    void assign(string arr_pushd[], int count, string current_path = string());
    void assign_debug_input(const string& input_path);

    // TODO - old - remove
    // void help_tip(void);
    // static void help(void);
    // static void version(void);
    // string normalize_path(const string& path);
    // string windowize_path(const string& path);
    // string get_parent_path(const string& path);
    // static bool paths_equal(const string& path1, int inode1, const string& path2);
    // static int get_inode(const string& path);

    bool change_to_path_spec(void);
    bool process_path_spec_including_filesystem(string target, TaggedPath& tagged_path, vector<string>& path_extra);
    bool process_path_spec_only_from_history(string target, TaggedPath& tagged_path, vector<string>& path_extra);

    bool go_backwards(unsigned amount, TaggedPath& tagged_path);
    bool go_forwards(unsigned amount, TaggedPath& tagged_path);
    bool go_common(unsigned amount, TaggedPath& tagged_path);

    bool process_match(const string& target, TaggedPath& tagged_path, vector<string>& path_extra);

    void show_history(void);
    void show_history_first_to_last(void);
    void show_history_last_to_first(void);
    void show_history_most_to_least(void);
    bool verify_history_matches(const std::vector<TaggedPath>& matches, const std::optional<RegexFilter>& rf);

    void process_reset(void);
    void garbage_collect(void);
    void process_delete(void);

    void command_generator(const vector<fs::path>& paths_remaining);
    void command_generator_win32(const vector<fs::path>& paths_remaining);
    void command_generator_bash(const vector<fs::path>& paths_remaining);
    std::size_t pushd_count() const;

    virtual bool is_directory(const fs::path& path);
    virtual bool is_regular_file(const fs::path& path);
};

// vim:ff=unix
