#pragma once

std::string get_working_path();
std::string get_environment(std::string var_name);
std::string expand_dots(std::string path);
bool is_special_dash_parameter(const std::string& s);
