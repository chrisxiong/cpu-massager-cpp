// Copyright (c) 2020 chrisxiong

#include <stdio.h>
#include <string>
#include "cpu_massager/cpu_massager.h"

using cpu_massager::CPUsageCollectorDocker;
using cpu_massager::CPUsageCollectorLinux;
using std::string;

int main(int argc, char** argv) {
  auto collector_linux = CPUsageCollectorLinux::NewCPUsageCollectorLinux();
  auto collector_docker = CPUsageCollectorDocker::NewCPUsageCollectorDocker();
  const int kInterval = 1;
  while (1) {
    sleep(kInterval);
    printf("linux cpusage:%.2f, docker cpusage:%.2f\n", collector_linux->CPUsage(),
           collector_docker->CPUsage());
  }
  return 0;
}
