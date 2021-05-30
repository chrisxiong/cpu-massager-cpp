// Copyright (c) 2021 chrisxiong

#include "cpu_massager/cpusage_collector_docker.h"

#include <cstdio>
#include <string>
#include "cpu_massager/cpusage_collector_linux.h"
#include "cpu_massager/file_helper.h"

namespace cpu_massager {

using std::string;

uint64_t CurrentCPUAcctUsageDocker() {
  uint64_t cpu_acct_usage = 0;
  string usage = FirstLineOf("/sys/fs/cgroup/cpuacct/cpuacct.usage");
  sscanf(usage.c_str(), "%lu", &cpu_acct_usage);
  return cpu_acct_usage;
}

int HostCPUNum() {
  int host_cpu_num = 0;
  string usage_percpu = FirstLineOf("/sys/fs/cgroup/cpuacct/cpuacct.usage_percpu");
  size_t pos = -1;
  while ((pos = usage_percpu.find(" ", pos + 1)) != string::npos) {
    host_cpu_num++;
  }
  return host_cpu_num;
}

// ContainerCPUNumWithCFS 获取CFS模式下的CPU使用率
// CFS作为SHCED_NORMAL的CPU使用率控制机制，被docker推荐使用，SCHED_RT未支持
float ContainerCPUNumWithCFS() {
  string cfs_quota_us = FirstLineOf("/sys/fs/cgroup/cpu/cpu.cfs_quota_us");
  float quota = 0.0;
  sscanf(cfs_quota_us.c_str(), "%f", &quota);
  if (quota < 0) {
    return HostCPUNum() * 1.0;
  }

  string cfs_period_us = FirstLineOf("/sys/fs/cgroup/cpu/cpu.cfs_period_us");
  float period = 0.0;
  sscanf(cfs_period_us.c_str(), "%f", &period);
  if (period <= 0.0) {
    return HostCPUNum() * 1.0;
  }

  return quota / period;
}

CPUDataDocker CurrentCPUDataDocker() {
  CPUDataDocker cpu_data_docker;

  CPUDataLinux cpu_data_linux = CurrentCPUDataLinux();
  const int kClockTicksPerSecond = 100;
  const int kNanoSecondsPerSecond = 1e9;
  cpu_data_docker.system_usage =
      cpu_data_linux.Total() / kClockTicksPerSecond * kNanoSecondsPerSecond;

  cpu_data_docker.docker_usage = CurrentCPUAcctUsageDocker();

  return cpu_data_docker;
}

float CPUsageCollectorDocker::CPUsage() {
  cur_ = CurrentCPUDataDocker();
  if (last_.system_usage == 0 || last_.docker_usage == 0) {
    last_ = cur_;
    return 0.0;
  }

  float docker_delta = (cur_.docker_usage - last_.docker_usage) * 1.0;
  float system_delta = (cur_.system_usage - last_.system_usage) * 1.0;
  if (docker_delta > 0.0 && system_delta > 0.0) {
    last_ = cur_;
    return (docker_delta / system_delta) * HostCPUNum() * 100.0 / ContainerCPUNumWithCFS();
  }

  return 0.0;
}

}  // namespace cpu_massager
