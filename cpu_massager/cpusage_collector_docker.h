// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_CPUSAGE_COLLECTOR_DOCKER_H_
#define CPU_MASSAGER_CPUSAGE_COLLECTOR_DOCKER_H_

#include <cstdint>
#include <memory>

#include "cpu_massager/cpusage_collector.h"

namespace cpu_massager {

using std::unique_ptr;

struct CPUDataDocker {
  CPUDataDocker() : docker_usage(0), system_usage(0) {}
  uint64_t docker_usage;  // 从/sys/fs/cgroup/cpuacct/cpuacct.usage获取，以纳秒为单位
  uint64_t system_usage;  // 从/proc/stat中获取的以clock_tick为单位的值，换算后得到纳秒
};

CPUDataDocker CurrentCPUDataDocker();

class CPUsageCollectorDocker : public CPUsageCollector {
 public:
  static unique_ptr<CPUsageCollectorDocker> NewCPUsageCollectorDocker() {
    return unique_ptr<CPUsageCollectorDocker>(new CPUsageCollectorDocker);
  }
  float CPUsage();

 private:
  CPUDataDocker last_;
  CPUDataDocker cur_;
};

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_CPUSAGE_COLLECTOR_DOCKER_H_
