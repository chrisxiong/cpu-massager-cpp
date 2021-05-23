// Copyright (c) 2021 chrisxiong

#include "cpu_massager/cpusage_collector_linux.h"
#include "cpu_massager/file_helper.h"

namespace cpu_massager {

CPUDataLinux CurrentCPUDataLinux() {
  CPUDataLinux cpu_data_linux;
  string proc_stat = FirstLineOf("/proc/stat");
  sscanf(proc_stat.c_str(), "cpu%lu%lu%lu%lu%lu%lu%lu%lu%lu%lu", &cpu_data_linux.user,
         &cpu_data_linux.nice, &cpu_data_linux.system, &cpu_data_linux.idle, &cpu_data_linux.iowait,
         &cpu_data_linux.irq, &cpu_data_linux.softirq, &cpu_data_linux.steal, &cpu_data_linux.guest,
         &cpu_data_linux.guestnice);
  return cpu_data_linux;
}

float CPUsageCollectorLinux::CPUsage() {
  cur_ = CurrentCPUDataLinux();
  if (0 == last_.Total()) {
    last_ = cur_;
    return 0.0;
  }

  uint64_t user_period = (cur_.user - cur_.guest) - (last_.user - last_.guest);
  uint64_t nice_period = (cur_.nice - cur_.guestnice) - (last_.nice - last_.guestnice);
  uint64_t system_period =
      (cur_.system + cur_.irq + cur_.softirq) - (last_.system + last_.irq + last_.softirq);
  uint64_t steal_period = cur_.steal - last_.steal;
  uint64_t guest_period = (cur_.guest + cur_.guestnice) - (last_.guest + last_.guestnice);
  uint64_t used_period = user_period + nice_period + system_period + steal_period + guest_period;
  uint64_t total_period = cur_.Total() - last_.Total();

  if (user_period > 0 && total_period > 0) {
    last_ = cur_;
    return used_period * 100.0 / total_period;
  }

  return 0.0;
}

}  // namespace cpu_massager
