/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   string_utils.hpp
 * Author: Biplab Ch Das
 *
 * Created on 16 June, 2016, 8:53 AM
 */

#ifndef __STRING_UTILS_HPP__
#define __STRING_UTILS_HPP__

#include <algorithm>
#include <cctype>
#include <functional>
#include <locale>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "io/list_creator.hpp"
bool has_suffix(const std::string &str, const std::string &suffix);
inline std::string trim(const std::string &s);
std::string strip(const std::string &s);

std::string strip_non_alpha(const std::string &s);
std::vector<std::string> &split(const std::string &input, char delim,
                                std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

#define SSTR(x) \
  static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()

template <typename T>
bool in(const std::unordered_set<T> &data, const T x);
template <typename T>
bool in(const std::set<T> &data, const T x);
template <typename T>
bool in(const std::unordered_set<T> *data, const T x);
template <typename T, typename T_ignore>
bool in(const std::unordered_map<T, T_ignore> &data, const T x);
template <typename T, typename T_ignore>
bool in(const std::map<T, T_ignore> &data, const T x);
std::vector<std::string> get_string_list_from_file(
    const std::string &file_name);
template <typename T>
void convert_vector_to_set(const std::vector<T> vec_,
                           std::unordered_set<T> &set_);
inline bool file_exists(const std::string &name);
#endif /* STRING_UTILS_HPP */
