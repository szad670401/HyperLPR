//
// Created by tunm on 2023/2/11.
//
#pragma once
#ifndef ZEPHYRLPR_TEST_SETTINGS_H
#define ZEPHYRLPR_TEST_SETTINGS_H
#include "catch2/catch.hpp"
#include <iostream>

using namespace Catch::Detail;

#define ENABLE_BENCHMARK_TEST 0  // 是否开启性能测试相关用例执行，默认不开启

#define TEST_MSG(...) SPDLOG_LOGGER_CALL(spdlog::get("TEST"), spdlog::level::trace, __VA_ARGS__)
#define GET_DIR getTestDataDir()
#define GET_DATA(filename) getTestData(filename)

#define GET_TMP_DIR getTestSaveDir()
#define GET_TMP_DATA(filename) getTestSaveData(filename)

std::string getTestDataDir();

std::string getTestData(const std::string &name);

std::string getTestSaveDir();

std::string getTestSaveData(const std::string &name);

struct test_case_line {
    ~test_case_line() {
        std::cout
                << "==============================================================================="
                << std::endl;
    }


#define PRINT_SPLIT_LINE test_case_line split_line_x;

};

#endif //ZEPHYRLPR_TEST_SETTINGS_H
