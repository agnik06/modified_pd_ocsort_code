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

#ifndef LINE_PROCESSOR_HPP
#define LINE_PROCESSOR_HPP
#include <fstream>
#include <iostream>
#include <string>
class LineProcessor {
 public:
  LineProcessor(const std::string& filename);
  void process();

 protected:
  // hook that can be overrriden  by derived classes.
  virtual void process_line(const std::string& line);
  std::string filename_;
};

#endif /* LINE_PROCESSOR_HPP */
