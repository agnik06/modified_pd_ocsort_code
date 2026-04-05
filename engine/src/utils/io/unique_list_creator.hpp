/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   unique_list_creator.hpp
 * Author: user
 *
 * Created on 20 June, 2016, 10:13 AM
 */

#ifndef UNIQUE_LIST_CREATOR_HPP
#define UNIQUE_LIST_CREATOR_HPP
#include <iostream>
#include <unordered_set>
#include "../utils/string_utils.hpp"
#include "line_processor.hpp"
class UniqueListCreator : public LineProcessor {
 public:
  UniqueListCreator(const std::string& filename) : LineProcessor(filename) {
    images_list_file_name_ = split(filename, '.')[0] + "_images.txt";
    words_list_file_name_ = split(filename, '.')[0] + "_words.txt";

    std::cout << "Unique Images will be written to:\t" << images_list_file_name_
              << "\n";
    std::cout << "Unique words will be written to:\t" << words_list_file_name_
              << "\n";

    images_list_file_.open(images_list_file_name_);
    words_list_file_.open(words_list_file_name_);
  }

  void write_to_file() {
    for (auto image_name : images_list_) {
      images_list_file_ << image_name << "\n";
    }
    for (auto word : words_list_) {
      words_list_file_ << word << "\n";
    }
    images_list_file_.close();
    words_list_file_.close();
  }
  ~UniqueListCreator() {}

 private:
  // Expects a csv file, no comma anywhere
  virtual void process_line(const std::string& line) override {
    std::string image_name = split(line, ',')[0];
    std::string tag = split(line, ',')[1];
    words_list_.insert(tag);
    images_list_.insert(image_name);
  }
  std::string images_list_file_name_;
  std::string words_list_file_name_;

  std::ofstream images_list_file_;
  std::ofstream words_list_file_;

  std::unordered_set<std::string> images_list_;
  std::unordered_set<std::string> words_list_;
};

#endif /* UNIQUE_LIST_CREATOR_HPP */
