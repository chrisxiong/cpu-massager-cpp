// Copyright (c) 2021 chrisxiong

#include "cpu_massager/cpusage_recorder.h"

#include <sys/shm.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace cpu_massager {

using std::vector;

void CPUsageRecorder::AddRecord(float cpusage) {
  if (cpusage < 0 || cpusage > 100) {
    return;
  }

  for (auto counter_type : AllCounterTypes()) {
    int& cpusage_counter = cpusage_counters_[counter_type];
    if (cpusage > counter_type * 10) {
      if (cpusage_counter < 100) {
        cpusage_counter++;
      }
    } else {
      if (cpusage_counter > 0) {
        cpusage_counter--;
      }
    }
  }

  return;
}

}  // namespace cpu_massager
