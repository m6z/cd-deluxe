/*

Copyright 2010-2019 Michael Graz
http://www.plan10.com/cdd

This file is part of Cd Deluxe.

Cd Deluxe is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cd Deluxe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cd Deluxe.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CDD_H
#define CDD_H

#include <string>
#include <vector>
#include <sstream>
#include <exception>
using namespace std;

struct Cdd
{
    struct Exception : public exception
    {
        string _msg;
        Exception(const string& msg) : _msg(msg) {}
        ~Exception() throw() {}
        virtual const char* what() const throw() { return _msg.c_str(); }
    };

    // The raw list of of pushed directories
    vector<string> vec_dir_stack;
    // The vector of pushed directories,
    // stored in last visited to first visited order
    vector<string> vec_dir_last_to_first;
    // The vector of set of directories visited (duplicates removed),
    // stored in first visited to last visited order
    vector<string> vec_dir_first_to_last;
    bool has_directory_stack = false;

    // This tracks the most common directories
    struct Common
    {
        int count;
        int sequence;
        string dir;
        Common(int count, int sequence, string dir) : count(count), sequence(sequence), dir(dir) {}
        bool operator==(const Common& obj) const
        {
            return count == obj.count && sequence == obj.sequence && dir == obj.dir;
        }
        bool operator<(const Common& obj) const
        {
            // Order by count in descending order, then by sequence in ascending order
            if (count == obj.count)
                return sequence < obj.sequence;
            return count > obj.count;
        }
        friend ostream& operator<<(ostream& out, const Common& obj)
        {
            out << "Common(" << obj.count << "," << obj.sequence << ",\"" << obj.dir << "\")";
            return out;
        }
    };
    vector<Common> vec_dir_most_to_least;

    stringstream strm_out;
    stringstream strm_err;

    string current_path;
    bool current_path_added;
    bool opt_help;
    bool opt_version;
    string opt_path;
    string opt_path_original;
    bool opt_history;
    bool opt_gc;
    bool opt_delete;
    bool opt_reset;
    unsigned opt_limit_backwards;
    unsigned opt_limit_forwards;
    unsigned opt_limit_common;
    bool opt_all;
    static const string env_options_name;

    struct Direction
    {
        string _direction;
        bool _direction_assigned;
        // default direction is backwards
        Direction(const string& direction="-") : _direction(direction), _direction_assigned(false) {}
        static bool is_valid_direction(string s) { return s == "+" || s == "-" || s == ","; }
        void assign(string direction)
        {
            if (!is_valid_direction(direction))
            {
                stringstream strm;
                strm << "Unknown direction '" << direction << "'";
                throw Exception(strm.str());
            }
            _direction = direction;
            _direction_assigned = true;
        }
        bool is_forwards() { return _direction == "+"; }
        bool is_backwards() { return _direction == "-"; }
        bool is_common() { return _direction == ","; }
        bool is_assigned() { return _direction_assigned; }
    };
    Direction direction;

    Cdd(void);
    Cdd(vector<string>& vec_pushd, string current_path);
    Cdd(string arr_pushd[], int count, string current_path=string());
    void assign(vector<string>& vec_pushd, string current_path);
    void assign(string arr_pushd[], int count, string current_path=string());
    void assign_debug_input(const string& input_path);
    void initialize(void);
    bool options(int ac, const char *av[], const string& options=string());
    void help_tip(void);
    static void help(void);
    static void version(void);
    static string normalize_path(const string& path);
    static string windowize_path(const string& path);
    static string get_parent_path(const string& path);
    static bool paths_equal(const string& path1, int inode1, const string& path2);
    static int get_inode(const string& path);
    static string expand_dots(string path);
    int pushd_count();

    void process(void);
    bool change_to_path_spec(void);
    bool process_path_spec(string& path_found, vector<string>& path_extra, stringstream& path_error);
    bool go_backwards(unsigned amount, string& path_found, stringstream& path_error);
    bool go_forwards(unsigned amount, string& path_found, stringstream& path_error);
    bool go_common(unsigned amount, string& path_found, stringstream& path_error);
    bool process_match(string& path_found, vector<string>& path_extra, stringstream& path_error);
    void show_history(void);
    void show_history_first_to_last(void);
    void show_history_last_to_first(void);
    void show_history_most_to_least(void);
    void garbage_collect(void);
    void process_delete(void);
    void process_reset(void);
    void command_generator(vector<string>& vec_dir, const string& dir_delete=string());
    void command_generator_win32(vector<string>& vec_dir, const string& dir_delete=string());
    void command_generator_bash(vector<string>& vec_dir, const string& dir_delete=string());
    void set_opt_path(const string& opt_path);
    bool set_history_direction(const string& spec);

    virtual bool is_directory(string path);
    virtual bool is_regular_file(string path);
};

#endif

// vim:ff=unix
