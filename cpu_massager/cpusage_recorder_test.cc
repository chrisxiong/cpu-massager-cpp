// Copyright (c) 2021 chrisxiong

#include <string>

#include "cpu_massager/cpusage_recorder.h"
#include "gtest/gtest.h"

using cpu_massager::CPUsageRecorder;
using std::string;

TEST(CPUMassager, CPUSageRecorder) {
  CPUsageRecorder recorder;
  for (auto counter_type : recorder.AllCounterTypes()) {
    EXPECT_EQ(0, recorder.RecordNumOf(counter_type));
  }

  // 不接收超出范围的记录数
  recorder.AddRecord(-1);
  recorder.AddRecord(101);
  for (auto counter_type : recorder.AllCounterTypes()) {
    EXPECT_EQ(0, recorder.RecordNumOf(counter_type));
  }

  // 先灌满100条记录，>=80、>=90的计数器为0，其他为100
  for (int i = 0; i < 100; i++) {
    recorder.AddRecord(75);
  }
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_ZERO));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_TEN));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_TWENTY));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_THIRTY));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_FORTY));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_FIFTY));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_SIXTY));
  EXPECT_EQ(100, recorder.RecordNumOf(cpu_massager::CT_GTEQ_SEVENTY));
  EXPECT_EQ(0, recorder.RecordNumOf(cpu_massager::CT_GTEQ_EIGHTY));
  EXPECT_EQ(0, recorder.RecordNumOf(cpu_massager::CT_GTEQ_NINETY));

  recorder.AddRecord(69);
  EXPECT_EQ(99, recorder.RecordNumOf(cpu_massager::CT_GTEQ_SEVENTY));  // >=70的计数器-1

  recorder.AddRecord(51);
  EXPECT_EQ(98, recorder.RecordNumOf(cpu_massager::CT_GTEQ_SEVENTY));  // >=70的计数器-1
  EXPECT_EQ(99, recorder.RecordNumOf(cpu_massager::CT_GTEQ_SIXTY));    // >=60的计数器-1
}

GTEST_API_ int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
