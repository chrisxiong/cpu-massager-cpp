// Copyright (c) 2021 chrisxiong

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>

#include "cpu_massager/cpusage_collector_mock.h"
#include "cpu_massager/massage_plan.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using cpu_massager::CT_GTEQ_EIGHTY;
using cpu_massager::MassagePlan;
using cpu_massager::MockCPUsageCollector;
using std::string;
using std::unique_ptr;
using ::testing::Return;

TEST(CPUMassager, MassagePlanExcution) {
  MockCPUsageCollector* collector = new MockCPUsageCollector;
  // 前面6次的CPU使用率是85，后面都是75
  EXPECT_CALL(*collector, CPUsage())
      .WillOnce(Return(85))
      .WillOnce(Return(85))
      .WillOnce(Return(85))
      .WillOnce(Return(85))
      .WillOnce(Return(85))
      .WillOnce(Return(85))
      .WillRepeatedly(Return(75));
  int adjust_period = 100;
  int collect_period = 10;
  int ret = MassagePlan::ThePlan()
                .SetCPUsageCollector(unique_ptr<MockCPUsageCollector>(collector))
                .SetInitialIntensity(20)  // 以20%的概率拒绝服务
                .SetIntensityAdjustPeriodInMilliSeconds(adjust_period)  // 以30ms为周期调节按摩力度
                .SetStepIntensity(10)  //  按摩力度的调整步长为10
                .SetCPUsageCollectPeriodInMilliseconds(collect_period)  //  每10ms记录一次CPU使用率
                .SetHighLoadLevel(CT_GTEQ_EIGHTY)  // CPU使用率>80就认为是高负荷
                .SetLoadStatusJudgeRatio(0.05)     // 有超过5个高负荷，则CPU疲累
                .Start();
  EXPECT_EQ(0, ret);
  EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());
  EXPECT_EQ(0, MassagePlan::ThePlan().CurrentIntensity());
  std::this_thread::sleep_for(std::chrono::milliseconds(40));  // 4个高负荷，CPU轻松
  EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());
  EXPECT_EQ(0, MassagePlan::ThePlan().CurrentIntensity());

  std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 5个高负荷，CPU轻松
  EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());
  EXPECT_EQ(0, MassagePlan::ThePlan().CurrentIntensity());

  std::this_thread::sleep_for(std::chrono::milliseconds(10));  // 6个高负荷，CPU疲累
  EXPECT_EQ(20, MassagePlan::ThePlan().CurrentIntensity());
  for (int idx = 0; idx < 100; idx++) {
    if (idx % 5 == 0) {
      EXPECT_TRUE(MassagePlan::ThePlan().NeedMassage());  // 6个高负荷，CPU疲累，需要放松，20%
    } else {
      EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());  // 6个高负荷，CPU疲累，硬抗工作，80%
    }
  }
  EXPECT_EQ(20, MassagePlan::ThePlan().CurrentIntensity());

  // 经过一个调节周期，调节按摩力度为10
  std::this_thread::sleep_for(std::chrono::milliseconds(adjust_period + 5));  // 0个高负荷，CPU疲累
  EXPECT_EQ(10, MassagePlan::ThePlan().CurrentIntensity());
  for (int idx = 0; idx < 100; idx++) {
    if (idx % 10 == 0) {
      EXPECT_TRUE(MassagePlan::ThePlan().NeedMassage());  // 0个高负荷，CPU疲累，需要放松，10%
    } else {
      EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());  // 0个高负荷，CPU疲累，硬抗工作，90%
    }
  }
  EXPECT_EQ(10, MassagePlan::ThePlan().CurrentIntensity());

  // 再经过一个调节周期，调节按摩力度为0
  std::this_thread::sleep_for(std::chrono::milliseconds(adjust_period + 5));
  EXPECT_EQ(0, MassagePlan::ThePlan().CurrentIntensity());
  for (int i = 0; i < 100; i++) {
    EXPECT_FALSE(MassagePlan::ThePlan().NeedMassage());  // 0个高负荷，CPU轻松，需要工作，100%
  }
  EXPECT_EQ(0, MassagePlan::ThePlan().CurrentIntensity());
}

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
