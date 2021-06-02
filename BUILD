# Copyright (c) 2021 chrisxiong

cc_library(
    name = "cpu_massager",
    srcs = [
        "cpu_massager/cpu_massager.h",
        "cpu_massager/cpusage_collector.h",
        "cpu_massager/cpusage_collector_docker.cc",
        "cpu_massager/cpusage_collector_docker.h",
        "cpu_massager/cpusage_collector_linux.cc",
        "cpu_massager/cpusage_collector_linux.h",
        "cpu_massager/cpusage_recorder.cc",
        "cpu_massager/cpusage_recorder.h",
        "cpu_massager/file_helper.cc",
        "cpu_massager/file_helper.h",
        "cpu_massager/massage_plan.cc",
        "cpu_massager/massage_plan.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "cpusage_collector_mock",
    srcs = [
        "cpu_massager/cpusage_collector.h",
        "cpu_massager/cpusage_collector_mock.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@gtest",
    ],
)

cc_binary(
    name = "show_cpusage",
    srcs = [
        "tools/show_cpusage.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//:cpu_massager",
    ],
)

cc_test(
    name = "cpusage_recorder_test",
    srcs = [
        "cpu_massager/cpusage_recorder.cc",
        "cpu_massager/cpusage_recorder.h",
        "cpu_massager/cpusage_recorder_test.cc",
    ],
    deps = [
        "@gtest",
    ],
)

cc_test(
    name = "massage_plan_test",
    srcs = [
        "cpu_massager/massage_plan_test.cc",
    ],
    deps = [
        "//:cpu_massager",
        "//:cpusage_collector_mock",
        "@gtest",
    ],
)
