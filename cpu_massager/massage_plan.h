// Copyright (c) 2021 chrisxiong

#ifndef CPU_MASSAGER_MASSAGE_PLAN_H_
#define CPU_MASSAGER_MASSAGE_PLAN_H_

#include <sys/time.h>
#include <atomic>
#include <cstdint>
#include <memory>
#include "cpu_massager/cpusage_collector.h"
#include "cpu_massager/cpusage_recorder.h"

namespace cpu_massager {

using std::unique_ptr;

const float kMaxLoadStatusJudgeRatio = 1.0;
const float kDefaultLoadStatusJudgeRatio = 0.2;
const uint8_t kEmptyIntensity = 0;
const uint8_t kFullIntensity = 100;
const uint8_t kDefaultInitialIntensity = 20;
const uint8_t kMaxStepIntensity = 10;
const uint8_t kDefaultStepIntensity = 1;
const uint16_t kMaxIntensityAdjustPeriodInMilliseconds = 10000;
const uint16_t kDefaultIntensityAdjustPeriodInMilliseconds = 3000;
const uint16_t kMaxCPUSageCollectPeriodInMilliseconds = 5000;
const uint16_t kDefaultCPUSageCollectPeriodInMilliseconds = 1000;
const int kRecordSum = 100;

enum CPU_STATUS {
  CPU_STATUS_RELAXED = 0,  // CPU处于轻松状态，正常工作，处理所有任务
  CPU_STATUS_TIRED = 1,    // CPU处于疲累状态，按疲累程度拒绝一定比例的任务
};

class MassagePlan {
 public:
  static MassagePlan& ThePlan() {
    static MassagePlan massage_plan;
    return massage_plan;
  }

  // Start 开启马杀鸡计划，会启用一个线程来定期收集CPU使用率，并更新CPU_STATUS
  // 0 -- 成功
  // -1 -- 计划已启动
  // -2 -- 没有指定CPU使用率收集器
  // -3 -- CPU使用率收集周期不应该>=按摩力度调整周期，而是需要小于
  int Start();

  // NeedMassage 判断CPU当前是否需要做个马杀鸡(即拒绝处理当前的请求)
  bool NeedMassage();

  // CurrentIntensity 获取当前的按摩力度，可以在CPU_STATUS_TIRED的时候调用，以观察其动态调节情况
  uint8_t CurrentIntensity() { return current_intensity_; }

  // cpusage_collector CPU使用率收集器，当前实现了linux系统和docker的收集器
  MassagePlan& SetCPUsageCollector(unique_ptr<CPUsageCollector> cpusage_collector) {
    if (started_) {
      return *this;
    }
    cpusage_collector_ = std::move(cpusage_collector);
    return *this;
  }

  // cpusage_collect_period_in_milliseconds CPU使用率收集周期，默认是1000ms
  MassagePlan& SetCPUsageCollectPeriodInMilliseconds(uint16_t collect_period) {
    if (started_) {
      return *this;
    }
    if (collect_period > kMaxCPUSageCollectPeriodInMilliseconds) {
      return *this;
    }
    cpusage_collect_period_in_milliseconds_ = collect_period;
    return *this;
  }

  // high_load_level 高负荷等级，和CPU使用率计数等级对等，用来判断CPU负荷是否过高
  // 例如，如果high_load_level值为CT_GTEQ_EIGHTY，当CPU使用率>70的时候，则认为
  // 当前CPU高负荷，high_load_level需要和load_status_judge_ratio配合，单次的
  // 高负荷可能是毛刺引起的，马杀鸡计划会启动一个计数器，每隔一段时间(例如1秒)记录下
  // CPU使用率，最多记录100次，如果高负荷的数量超过100*load_status_judge_ratio，
  // 那么，就认为CPU当前处于Tired状态
  MassagePlan& SetHighLoadLevel(COUNTER_TYPE high_load_level) {
    if (started_) {
      return *this;
    }
    high_load_level_ = high_load_level;
    return *this;
  }

  // load_status_judge_ratio 表示高负荷占比判别阈值，如果100个CPU记录周期里面的
  // CPU高负荷计数占比超过改值，就认为CPU处于Tired状态
  MassagePlan& SetLoadStatusJudgeRatio(float load_status_judge_ratio) {
    if (started_) {
      return *this;
    }
    if (load_status_judge_ratio <= 0 || load_status_judge_ratio > kMaxLoadStatusJudgeRatio) {
      return *this;
    }
    load_status_judge_ratio_ = load_status_judge_ratio;
    return *this;
  }

  // inital_intensity 初始化按摩力度，即CPU初次进入疲累状态时，拒绝服务的概率
  // 例如，initial_intensity=20，代表CPU初次进入疲累状态，按20%的概率拒绝服务
  MassagePlan& SetInitialIntensity(uint8_t initial_intensity) {
    if (started_) {
      return *this;
    }
    if (initial_intensity > kFullIntensity) {
      return *this;
    }
    initial_intensity_ = initial_intensity;
    return *this;
  }

  // step_intensity 按摩力度调节步长，CPU进入疲累状态后，会周期检查其疲累的情况
  // 如果疲劳程度降低了，那么降低按摩力度，即降低拒绝服务的概率，降至0则归于轻松
  // 如果疲劳程度升高了，那么提升按摩力度，即提升拒绝服务的概率
  MassagePlan& SetStepIntensity(uint8_t step_intensity) {
    if (started_) {
      return *this;
    }
    if (step_intensity > kMaxStepIntensity) {
      return *this;
    }
    step_intensity_ = step_intensity;
    return *this;
  }

  // intensity_adjust_period_in_milliseconds 按摩力度调节周期，以毫秒计
  MassagePlan& SetIntensityAdjustPeriodInMilliSeconds(uint16_t adjust_period) {
    if (started_) {
      return *this;
    }
    if (adjust_period > kMaxIntensityAdjustPeriodInMilliseconds) {
      return *this;
    }
    intensity_adjust_period_in_milliseconds_ = adjust_period;
    return *this;
  }

 private:
  MassagePlan()
      : cpusage_collector_(nullptr),
        cpusage_collect_period_in_milliseconds_(kDefaultCPUSageCollectPeriodInMilliseconds),
        high_load_level_(CT_GTEQ_EIGHTY),
        load_status_judge_ratio_(kDefaultLoadStatusJudgeRatio),
        initial_intensity_(kDefaultInitialIntensity),
        step_intensity_(kDefaultStepIntensity),
        intensity_adjust_period_in_milliseconds_(kDefaultIntensityAdjustPeriodInMilliseconds),
        started_(false),
        cpu_status_(CPU_STATUS_RELAXED),
        current_intensity_(0),
        current_cpusage_record_time_(0),
        last_adjust_intensity_time_(0),
        last_high_load_count_(0),
        todo_tasks_(0),
        done_tasks_(0) {}

 private:
  bool CanWorkInTired();
  void AddACPUsageRecord();
  void SetTired();
  void SetRelaxed();
  void IncreaseIntensity();
  void DecreaseIntensity();
  inline bool IsHighLoad() {
    int high_load_count = cpusage_recorder_.RecordNumOf(high_load_level_);
    return high_load_count > int(kRecordSum * load_status_judge_ratio_);
  }
  inline bool IsTired() { return cpu_status_ == CPU_STATUS_TIRED; }
  inline bool IsRelaxed() { return cpu_status_ == CPU_STATUS_RELAXED; }
  inline bool IsAdjustDurationExceedAdjustPeriod() {
    uint16_t duration = current_cpusage_record_time_ - last_adjust_intensity_time_;
    return duration > intensity_adjust_period_in_milliseconds_;
  }
  inline void ClearWorkspace() {
    todo_tasks_ = 0;
    done_tasks_ = 0;
  }
  bool IsHighLoadCountIncreased() {
    int current_high_load_count = cpusage_recorder_.RecordNumOf(high_load_level_);
    bool is_increased =
        current_high_load_count > last_high_load_count_ || current_high_load_count == kRecordSum;
    last_high_load_count_ = current_high_load_count;
    return is_increased;
  }

 private:
  uint64_t CurrentTimeInMilliseconds() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
  }

 private:
  unique_ptr<CPUsageCollector> cpusage_collector_;
  uint16_t cpusage_collect_period_in_milliseconds_;
  COUNTER_TYPE high_load_level_;
  float load_status_judge_ratio_;
  uint8_t initial_intensity_;
  uint8_t step_intensity_;
  uint16_t intensity_adjust_period_in_milliseconds_;

  bool started_;
  CPUsageRecorder cpusage_recorder_;
  CPU_STATUS cpu_status_;
  uint8_t current_intensity_;
  uint64_t current_cpusage_record_time_;
  uint64_t last_adjust_intensity_time_;
  int last_high_load_count_;

  std::atomic_ullong todo_tasks_;
  std::atomic_ullong done_tasks_;
};

}  // namespace cpu_massager

#endif  // CPU_MASSAGER_MASSAGE_PLAN_H_
