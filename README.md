# cpu-massager-cpp
给cpu配备一个按摩器，必要时做个马杀鸡，放松放松～

帮助文档参考这里：https://github.com/chrisxiong/cpu-massager-go/blob/master/README.md

使用说明：
```
#include "cpu_massager/cpu_massager.h"

// 服务处理流程中，每次都先检查是否需要做马杀鸡
void serve() {
  if (cpu_massager::MassagePlan::ThePlan().NeedMassage()) {
    Report("CPU在做马杀鸡-按摩次数", 1);  // 上报过载次数
    Report("CPU在做马杀鸡-按摩力度", cpu_massager::MassagePlan::ThePlan().CurrentIntensity());  // 上报拒绝服务的概率
    response();  // 过载的时候，直接给出响应，其他啥也不要干了
  }
  process();  // 正常处理该请求
}

// 程序启动，开启马杀鸡计划，例如
int main(int argc, char** argv) {
  int ret = cpu_massager::MassagePlan::ThePlan()
            .SetCPUsageCollector(cpu_massager::CPUsageCollectorDocker::NewCPUsageCollectorDocker())
            .Start();
  if (ret != 0) {
    HandleError();  // 处理出错的情况，一般打印下出错信息
    exit(1);  // 然后推出就好了
  }
  serve();  // 进入服务的正常处理流程
}
```
