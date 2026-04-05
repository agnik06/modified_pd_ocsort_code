/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   list_creator.hpp
 * Author: user
 *
 * Created on 21 June, 2016, 9:29 AM
 */

#ifndef LIST_CREATOR_HPP
#define LIST_CREATOR_HPP

#include <vector>
#include "line_processor.hpp"
class ListCreator : public LineProcessor {
 public:
  ListCreator(const std::string& filename) : LineProcessor(filename) {
    list_.clear();
  }
  std::vector<std::string> get_list() { return list_; }

 private:
  virtual void process_line(const std::string& line) override {
    list_.push_back(line);
  }
  std::vector<std::string> list_;
};

#endif /* LIST_CREATOR_HPP */
