/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   line_processor.hpp
 * Author: Biplab Ch Das
 *
 * Created on 16 June, 2016, 9:33 AM
 */

#include "line_processor.hpp"
#include <fstream>
#include <iostream>
#include <string>

LineProcessor::LineProcessor(const std::string& filename) {
  filename_ = filename;
}

void LineProcessor::process_line(const std::string& line) {
  std::cout << "[" << line << "]" << std::endl;
}

void LineProcessor::process() {
  std::ifstream textFile(filename_);
  std::string line = "";
  while (!textFile.eof()) {
    std::getline(textFile, line);
    if (line != "") {
      process_line(line);
    }
  }
  textFile.close();
}
