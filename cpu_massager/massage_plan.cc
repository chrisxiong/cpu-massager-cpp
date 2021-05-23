// Copyright (c) 2021 chrisxiong

#include "cpu_massager/massage_plan.h"

#include <cstdint>
#include <thread>
#include "cpu_massager/cpusage_collector.h"
#include "cpu_massager/cpusage_recorder.h"

namespace cpu_massager {

int MassagePlan::Start() {
  if (started_) {
    return -1;
  }

  if (cpusage_collector_ == nullptr) {
    return -2;
  }

  if (cpusage_collect_period_in_milliseconds_ >= intensity_adjust_period_in_milliseconds_) {
    return -3;
  }

  std::thread excuter([this]() {
    while (true) {
      AddACPUsageRecord();
      std::this_thread::sleep_for(
          std::chrono::milliseconds(cpusage_collect_period_in_milliseconds_));
    }
  });
  excuter.detach();

  started_ = true;
  return 0;
}

bool MassagePlan::NeedMassage() {
  if (IsRelaxed()) {
    return false;
  }
  if (CanWorkInTired()) {
    return false;
  }
  return true;
}

bool MassagePlan::CanWorkInTired() {
  todo_tasks_++;
  uint64_t required_tasks = todo_tasks_ * (kFullIntensity - current_intensity_) / kFullIntensity;
  if (done_tasks_ < required_tasks) {
    done_tasks_++;
    return true;
  }
  return false;
}

void MassagePlan::AddACPUsageRecord() {
  cpusage_recorder_.AddRecord(cpusage_collector_->CPUsage());
  current_cpusage_record_time_ = CurrentTimeInMilliseconds();
  if (IsRelaxed()) {
    if (IsHighLoad()) {
      SetTired();
    }
  } else {
    if (!IsAdjustDurationExceedAdjustPeriod()) {
      return;
    }
    if (IsHighLoadCountIncreased()) {
      IncreaseIntensity();
    } else {
      DecreaseIntensity();
    }
  }
  return;
}

void MassagePlan::SetTired() {
  cpu_status_ = CPU_STATUS_TIRED;
  current_intensity_ = initial_intensity_;
  last_high_load_count_ = cpusage_recorder_.RecordNumOf(high_load_level_);
  last_adjust_intensity_time_ = current_cpusage_record_time_;
  ClearWorkspace();
  return;
}

void MassagePlan::SetRelaxed() {
  cpu_status_ = CPU_STATUS_RELAXED;
  current_intensity_ = 0;
  last_high_load_count_ = 0;
  last_adjust_intensity_time_ = 0;
  ClearWorkspace();
  return;
}

void MassagePlan::IncreaseIntensity() {
  current_intensity_ += step_intensity_;
  if (current_intensity_ > kFullIntensity) {
    current_intensity_ = kFullIntensity;
  }
  last_adjust_intensity_time_ = current_cpusage_record_time_;
  ClearWorkspace();
  return;
}

void MassagePlan::DecreaseIntensity() {
  if (current_intensity_ == kEmptyIntensity) {
    SetRelaxed();
  } else {
    if (current_intensity_ > step_intensity_) {
      current_intensity_ -= step_intensity_;
    } else {
      current_intensity_ = kEmptyIntensity;
    }
    last_adjust_intensity_time_ = current_cpusage_record_time_;
    ClearWorkspace();
  }
  return;
}

}  // namespace cpu_massager
