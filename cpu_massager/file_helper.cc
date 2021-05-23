// Copyright (c) 2021 chrisxiong

#include "cpu_massager/file_helper.h"

#include <fstream>
#include <string>

namespace cpu_massager {

using std::ifstream;
using std::string;

string FirstLineOf(const string& file_name) {
  string first_line;
  ifstream ifs(file_name);
  if (!ifs) {
    return first_line;
  }
  getline(ifs, first_line);
  ifs.close();
  return first_line;
}

}  // namespace cpu_massager
