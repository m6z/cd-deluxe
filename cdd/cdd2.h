#pragma once

#include <exception>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include "cdd_options.h"

using namespace std;
namespace fs = std::filesystem;

class Cdd2
{
public:
    Cdd2(const CddOptions& options, const fs::path& cwd, const vector<string> dirs) : options(options), cwd(cwd), dirs(dirs) { initialize(); }

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

    class KeyedPath
    {
    public:
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

protected:
    const auto& get_dirs_last_to_first() const { return dirs_last_to_first; }
    const auto& get_dirs_first_to_last() const { return dirs_first_to_last; }
    const auto& get_dirs_most_to_least() const { return dirs_most_to_least; }

private:
    void initialize();

    CddOptions options;  // The options for cdd2
    fs::path cwd;        // Current working directory
    vector<string> dirs; // The raw list of pushed directories

    // The vector of pushed directories,
    // stored in last visited to first visited order
    vector<fs::path> dirs_last_to_first;

    // The vector of set of directories visited (duplicates removed),
    // stored in first visited to last visited order
    vector<fs::path> dirs_first_to_last;

    // This tracks the most common directories
    vector<CommonPath> dirs_most_to_least;

    bool has_directory_stack = false;

    stringstream strm_out;
    stringstream strm_err;

    // TODO - old - remove
    // struct Direction
    // {
    //     string _direction;
    //     bool _direction_assigned;
    //     // default direction is backwards
    //     Direction(const string& direction = "-") : _direction(direction), _direction_assigned(false) {}
    //     static bool is_valid_direction(string s) { return s == "+" || s == "-" || s == ","; }
    //     void assign(string direction)
    //     {
    //         if (!is_valid_direction(direction))
    //         {
    //             stringstream strm;
    //             strm << "Unknown direction '" << direction << "'";
    //             throw Exception(strm.str());
    //         }
    //         _direction = direction;
    //         _direction_assigned = true;
    //     }
    //     bool is_forwards() { return _direction == "+"; }
    //     bool is_backwards() { return _direction == "-"; }
    //     bool is_common() { return _direction == ","; }
    //     bool is_assigned() { return _direction_assigned; }
    // };
    // Direction direction;

    void assign(vector<string>& vec_pushd, string current_path);
    void assign(string arr_pushd[], int count, string current_path = string());
    void assign_debug_input(const string& input_path);
    // void initialize(void);
    // bool options(int ac, const char* av[], const string& options = string());
    void help_tip(void);
    static void help(void);
    static void version(void);
    string normalize_path(const string& path);
    string windowize_path(const string& path);
    string get_parent_path(const string& path);
    static bool paths_equal(const string& path1, int inode1, const string& path2);
    static int get_inode(const string& path);
    int pushd_count();

    bool change_to_path_spec(void);
    bool process_path_spec(string target, fs::path& path_found, vector<string>& path_extra);

    bool go_backwards(unsigned amount, fs::path& path_found);
    bool go_forwards(unsigned amount, fs::path& path_found);
    bool go_common(unsigned amount, fs::path& path_found);

    bool process_match(const string& target, fs::path& path_found, vector<string>& path_extra);

    void show_history(void);
    void show_history_first_to_last(void);
    void show_history_last_to_first(void);
    void show_history_most_to_least(void);

    void garbage_collect(void);
    void process_delete(void);
    void process_reset(void);
    void command_generator(vector<string>& vec_dir, const string& dir_delete = string());
    void command_generator_win32(vector<string>& vec_dir, const string& dir_delete = string());
    void command_generator_bash(vector<string>& vec_dir, const string& dir_delete = string());
    void set_opt_path(const string& opt_path);
    bool set_history_direction(const string& spec);

    virtual bool is_directory(const fs::path& path);
    virtual bool is_regular_file(const fs::path& path);
};

// vim:ff=unix
