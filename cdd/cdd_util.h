#pragma once

#include <filesystem>
#include <string>

std::string get_working_path();
std::string get_environment(std::string var_name);
std::string expand_dots(std::string path);
bool is_special_dash_parameter(const std::string& s);
bool is_parent_of(std::filesystem::path parent, std::filesystem::path child);
std::vector<std::tuple<std::string, std::filesystem::path>> get_path_components(const std::filesystem::path& path);
std::string trim_from_char(const std::string& str, char ch);
int get_year();
