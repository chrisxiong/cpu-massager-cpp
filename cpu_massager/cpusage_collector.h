// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_CPUSAGE_COLLECTOR_H_
#define CPU_MASSAGER_CPUSAGE_COLLECTOR_H_

namespace cpu_massager {

class CPUsageCollector {
 public:
  virtual ~CPUsageCollector() {}
  virtual float CPUsage() = 0;
};

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_CPUSAGE_COLLECTOR_H_
