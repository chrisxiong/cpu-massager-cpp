// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_CPUSAGE_COLLECTOR_LINUX_H_
#define CPU_MASSAGER_CPUSAGE_COLLECTOR_LINUX_H_

#include <fcntl.h>
#include <unistd.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>

#include "cpu_massager/cpusage_collector.h"

namespace cpu_massager {

using std::unique_ptr;

struct CPUDataLinux {
  CPUDataLinux()
      : user(0),
        nice(0),
        system(0),
        idle(0),
        iowait(0),
        irq(0),
        softirq(0),
        steal(0),
        guest(0),
        guestnice(0) {}
  uint64_t user;
  uint64_t nice;
  uint64_t system;
  uint64_t idle;
  uint64_t iowait;
  uint64_t irq;
  uint64_t softirq;
  uint64_t steal;
  uint64_t guest;
  uint64_t guestnice;
  uint64_t Total() { return user + nice + system + idle + iowait + irq + softirq + steal; }
};

CPUDataLinux CurrentCPUDataLinux();

class CPUsageCollectorLinux : public CPUsageCollector {
 public:
  static unique_ptr<CPUsageCollectorLinux> NewCPUsageCollectorLinux() {
    return unique_ptr<CPUsageCollectorLinux>(new CPUsageCollectorLinux);
  }
  float CPUsage();

 private:
  CPUDataLinux last_;
  CPUDataLinux cur_;
};

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_CPUSAGE_COLLECTOR_LINUX_H_
