#include <version>
#ifndef __cpp_lib_ranges
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_ranges)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include "custom_window_fold_view.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>
#include <string>

namespace views = std::ranges::views;
namespace ranges = std::ranges;

// Parametrized fixture
class StringVectorParametrizedTest
    : public ::testing::TestWithParam<
          std::tuple<int, int, std::vector<std::string>>> {};

using CustomWindowFoldDirect = StringVectorParametrizedTest;
TEST_P(CustomWindowFoldDirect, Job) {
  auto [n, p, output] = GetParam();
  auto v = views::iota(1)                                                 //
           | views::filter([](auto &&x) { return x % 2; })                //
           | views::transform([](auto &&x) { return std::to_string(x); }) //
           | views::take(n)                                               //
           | custom_views::window_fold<std::string>(p)                    //
      ;

  std::vector<std::string> result;
  ranges::copy(v, std::back_inserter(result));
  EXPECT_THAT(result, ::testing::ElementsAreArray(output));
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomWindowFoldDirect,
    testing::Values(std::make_tuple(0, 3, std::vector<std::string>{}),
                    std::make_tuple(1, 3, std::vector<std::string>{"1"}),
                    std::make_tuple(2, 3, std::vector<std::string>{"13"}),
                    std::make_tuple(3, 3, std::vector<std::string>{"135"}),
                    std::make_tuple(4, 3, std::vector<std::string>{"135", "7"}),
                    std::make_tuple(5, 3, std::vector<std::string>{"135", "79"}),
                    std::make_tuple(6, 3, std::vector<std::string>{"135", "7911"}),
                    std::make_tuple(7, 3,
                                    std::vector<std::string>{"135", "7911", "13"})));

using CustomWindowFoldDirect2 = StringVectorParametrizedTest;
TEST_P(CustomWindowFoldDirect2, Job) {
  auto [n, p, output] = GetParam();
  auto v = views::iota(0, n)                                              //
           | views::filter([](auto &&x) { return x % 2; })                //
           | views::transform([](auto &&x) { return std::to_string(x); }) //
           | custom_views::window_fold<std::string>(p)                    //
      ;

  std::vector<std::string> result;
  ranges::copy(v, std::back_inserter(result));
  EXPECT_THAT(result, ::testing::ElementsAreArray(output));
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomWindowFoldDirect2,
    testing::Values(std::make_tuple(0, 3, std::vector<std::string>{}),
                    std::make_tuple(1, 3, std::vector<std::string>{}),
                    std::make_tuple(2, 3, std::vector<std::string>{"1"}),
                    std::make_tuple(3, 3, std::vector<std::string>{"1"}),
                    std::make_tuple(4, 3, std::vector<std::string>{"13"}),
                    std::make_tuple(5, 3, std::vector<std::string>{"13"}),
                    std::make_tuple(6, 3, std::vector<std::string>{"135"}),
                    std::make_tuple(7, 3, std::vector<std::string>{"135"}),
                    std::make_tuple(8, 3, std::vector<std::string>{"135", "7"}),
                    std::make_tuple(9, 3, std::vector<std::string>{"135", "7"}),
                    std::make_tuple(10, 3, std::vector<std::string>{"135", "79"})));

using CustomWindowFoldReverse = StringVectorParametrizedTest;
TEST_P(CustomWindowFoldReverse, Job) {
  auto [n, p, output] = GetParam();
  auto v = views::iota(0, n)                                              //
           | views::transform([](auto &&x) { return std::to_string(x); }) //
           | custom_views::window_fold<std::string>(p)                    //
           | views::reverse | views::take(4);

  std::vector<std::string> result;
  ranges::copy(v, std::back_inserter(result));
  EXPECT_THAT(result, ::testing::ElementsAreArray(output));
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomWindowFoldReverse,
    testing::Values(std::make_tuple(0, 3, std::vector<std::string>{}),
                    std::make_tuple(1, 3, std::vector<std::string>{"0"}),
                    std::make_tuple(2, 3, std::vector<std::string>{"01"}),
                    std::make_tuple(3, 3, std::vector<std::string>{"012"}),
                    std::make_tuple(4, 3, std::vector<std::string>{"123", "0"}),
                    std::make_tuple(5, 3, std::vector<std::string>{"234", "01"}),
                    std::make_tuple(6, 3, std::vector<std::string>{"345", "012"}),
                    std::make_tuple(7, 3,
                                    std::vector<std::string>{"456", "123", "0"})));

#endif