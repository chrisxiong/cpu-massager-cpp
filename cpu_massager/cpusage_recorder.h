// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_CPUSAGE_RECORDER_H_
#define CPU_MASSAGER_CPUSAGE_RECORDER_H_

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cpu_massager {

using std::vector;

const int kCounterNum = 10;

enum COUNTER_TYPE {
  CT_GTEQ_ZERO = 0,
  CT_GTEQ_TEN,
  CT_GTEQ_TWENTY,
  CT_GTEQ_THIRTY,
  CT_GTEQ_FORTY,
  CT_GTEQ_FIFTY,
  CT_GTEQ_SIXTY,
  CT_GTEQ_SEVENTY,
  CT_GTEQ_EIGHTY,
  CT_GTEQ_NINETY,
};

class CPUsageRecorder {
 public:
  static vector<COUNTER_TYPE> AllCounterTypes() {
    return {CT_GTEQ_ZERO,  CT_GTEQ_TEN,   CT_GTEQ_TWENTY,  CT_GTEQ_THIRTY, CT_GTEQ_FORTY,
            CT_GTEQ_FIFTY, CT_GTEQ_SIXTY, CT_GTEQ_SEVENTY, CT_GTEQ_EIGHTY, CT_GTEQ_NINETY};
  }

  CPUsageRecorder() : cpusage_counters_{0} {}
  ~CPUsageRecorder() {}

  void AddRecord(float cpusage);
  inline uint16_t RecordNumOf(COUNTER_TYPE counter_type) { return cpusage_counters_[counter_type]; }

 private:
  int cpusage_counters_[kCounterNum];
};

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_CPUSAGE_RECORDER_H_
