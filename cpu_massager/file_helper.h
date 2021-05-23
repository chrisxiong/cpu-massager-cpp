// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_CPUSAGE_FILE_HELPER_H_
#define CPU_MASSAGER_CPUSAGE_FILE_HELPER_H_

#include <string>

namespace cpu_massager {

using std::string;

string FirstLineOf(const string& file_name);

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_CPUSAGE_FILE_HELPER_H_
