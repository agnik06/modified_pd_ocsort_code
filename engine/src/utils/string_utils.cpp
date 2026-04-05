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

#include "string_utils.hpp"

bool has_suffix(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
inline std::string trim(const std::string &s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(),
                                  [](int c) { return std::isspace(c); });
  auto wsback = std::find_if_not(s.rbegin(), s.rend(),
                                 [](int c) { return std::isspace(c); })
                    .base();
  return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}
std::string strip(const std::string &s) {
  std::string result;
  result.reserve(s.length());

  std::copy_if(s.begin(), s.end(), std::back_inserter(result),
                [](const char& x){
                  return !isspace(x);
                }
              );

  return result;
}

std::string strip_non_alpha(const std::string &s) {
  std::string result;
  result.reserve(s.length());

  std::copy_if(s.begin(), s.end(), std::back_inserter(result),
                [](const char& x){
                  return !isalpha(x);
                }
              );

  return result;
}
std::vector<std::string> &split(const std::string &input, char delim,
                                std::vector<std::string> &elems) {
  std::stringstream ss(input);
  std::string item;
  while (std::getline(ss, item, delim)) {
    if (item.length() > 0) {
      elems.push_back(item);
    }
  }
  return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

#define SSTR(x) \
  static_cast<std::ostringstream &>(std::ostringstream() << std::dec << x).str()

template <typename T>
bool in(const std::unordered_set<T> &data, const T x) {
  return (data.find(x) != data.end());
}
template <typename T>
bool in(const std::set<T> &data, const T x) {
  return (data.find(x) != data.end());
}
template <typename T>
bool in(const std::unordered_set<T> *data, const T x) {
  return (data->find(x) != data->end());
}
template <typename T, typename T_ignore>
bool in(const std::unordered_map<T, T_ignore> &data, const T x) {
  return (data.find(x) != data.end());
}
template <typename T, typename T_ignore>
bool in(const std::map<T, T_ignore> &data, const T x) {
  return (data.find(x) != data.end());
}
std::vector<std::string> get_string_list_from_file(
    const std::string &file_name) {
  ListCreator list_creator(file_name);
  list_creator.process();
  return list_creator.get_list();
}
template <typename T>
void convert_vector_to_set(const std::vector<T> vec_,
                           std::unordered_set<T> &set_) {
  set_.clear();
  for (auto elem : vec_) {
    set_.insert(elem);
  }
}
inline bool file_exists(const std::string &name) {
  std::ifstream f(name.c_str());
  return f.good();
}

template bool in<std::string>(std::set<std::string, std::less<std::string>,
                                       std::allocator<std::string> > const &,
                              std::string);
template bool in<int, int>(
    std::map<int, int, std::less<int>,
             std::allocator<std::pair<int const, int> > > const &,
    int);
template bool in<int>(
    std::set<int, std::less<int>, std::allocator<int> > const &, int);
