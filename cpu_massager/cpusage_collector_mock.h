// Copyright (c) 2021 chrisxiong

#ifndef CPUSAGE_COLLECOTR_MOCK_H
#define CPUSAGE_COLLECOTR_MOCK_H

#include <cstdint>
#include <string>

#include "cpu_massager/cpusage_collector.h"
#include "gmock/gmock.h"

namespace cpu_massager {

class MockCPUsageCollector : public CPUsageCollector {
 public:
  MockCPUsageCollector() : CPUsageCollector() {}
  MOCK_METHOD(float, CPUsage, (), (override));
};

}  // namespace cpu_massager

#endif  // CPUSAGE_COLLECOTR_MOCK_H
