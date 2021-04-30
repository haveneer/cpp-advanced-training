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

// https://github.com/joboccara/ranges/blob/master/adjacent.hpp
// or/and custom generator

#include "custom_take_view.hpp"
#include <algorithm>
#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>

namespace views = std::ranges::views;
namespace ranges = std::ranges;

// Parametrized fixture
class VectorsParametrizedTest
    : public ::testing::TestWithParam<
          std::tuple<std::vector<int>, std::vector<int>, int>> {};

using CustomTakeDirect = VectorsParametrizedTest;
TEST_P(CustomTakeDirect, Job) {
  auto [input, output, n] = GetParam();
  std::size_t index = 0;
  for (auto const &i : input | custom_views::custom_take(n)) {
    EXPECT_EQ(i, output[index]);
    index++;
  }
  EXPECT_EQ(index, output.size());
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomTakeDirect,
    testing::Values(
        std::make_tuple(std::vector<int>{}, std::vector<int>{}, 0),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector<int>{}, 0),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{1}, 1),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{1, 2}, 2),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{1, 2, 3, 4, 5}, 5)));

using CustomTakeReverse = VectorsParametrizedTest;
TEST_P(CustomTakeReverse, Job) {
  auto [input, output, n] = GetParam();
  std::size_t index = 0;
  for (auto const &i :
       input | custom_views::custom_take(n) | ranges::views::reverse) {
    EXPECT_EQ(i, output[index]);
    index++;
  }
  EXPECT_EQ(index, output.size());
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomTakeReverse,
    testing::Values(
        std::make_tuple(std::vector<int>{}, std::vector<int>{}, 0),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector<int>{}, 0),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{1}, 1),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{2, 1}, 2),
        std::make_tuple(std::vector{1, 2, 3, 4, 5}, std::vector{5, 4, 3, 2, 1}, 5)));

TEST(CustomTake, CombinedTest) {
  auto is_even = [](int const n) { return n % 2 == 0; };

  std::vector<int> n{1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377};
  auto v =
      n | views::filter(is_even) | views::reverse | custom_views::custom_take(2);

  auto i = v.begin();
  for (auto &&expected : {144, 34}) {
    EXPECT_EQ(*i++, expected);
  }
  EXPECT_EQ(i, v.end());
}

#endif