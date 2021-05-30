# Copyright (c) 2021 chrisxiong

.PHONY: all clean build test

all: build
clean:
	@echo "***************[bazel clean]**************"
	bazel clean
	@echo

build:
	@echo "***************[bazel build ...]**************"
	bazel build ...
	@echo

cpu_massager:
	@echo "***************[bazel build $@]**************"
	bazel build $@
	@echo

show_cpusage:
	@echo "***************[bazel build $@]**************"
	bazel build $@
	@echo

test:
	@echo "***************[bazel test ...]**************"
	bazel test --test_output=all ...
	@echo

cpusage_recorder_test:
	@echo "***************[bazel test $@]**************"
	bazel test --test_output=all $@
	@echo

massage_plan_test:
	@echo "***************[bazel test $@]**************"
	bazel test --test_output=all $@
	@echo
