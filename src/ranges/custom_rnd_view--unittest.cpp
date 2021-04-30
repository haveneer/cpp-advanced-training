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

#include "custom_rnd_view.hpp"
#include <algorithm>
#include <cassert>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>

namespace views = std::ranges::views;
namespace ranges = std::ranges;

// Parametrized fixture
class VectorsParametrizedTest
    : public ::testing::TestWithParam<std::tuple<int, std::vector<int>>> {};

using CustomRndView = VectorsParametrizedTest;
TEST_P(CustomRndView, Direct) {
  const unsigned int seed = 1;
  auto [n, output] = GetParam();
  std::size_t index = 0;
  for (auto const &i : custom_views::rnd(seed) | views::drop(4) | views::take(n)) {
    EXPECT_EQ(i, output[index]);
    index++;
  }
}

TEST_P(CustomRndView, Reverse) {
  const unsigned int seed = 1;
  auto [n, output] = GetParam();
  std::size_t index = n;
  for (auto const &i :
       custom_views::rnd(seed) | views::drop(4) | views::take(n) | views::reverse) {
    EXPECT_EQ(i, output[index - 1]);
    index--;
  }
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomRndView,
    testing::Values(
        std::make_tuple(0, std::vector<int>{}),
        std::make_tuple(1, std::vector{245631, 275145156, 649254245, 2145423170}),
        std::make_tuple(2, std::vector{245631, 275145156, 649254245, 2145423170}),
        std::make_tuple(3, std::vector{245631, 275145156, 649254245, 2145423170}),
        std::make_tuple(4, std::vector{245631, 275145156, 649254245, 2145423170})));

TEST(CustomRndView, CombinedTest) {
  const unsigned int seed = 10;
  auto v = custom_views::rnd(seed);
  auto i1 = v.begin();
  auto i2 = i1;
  for (auto &n : {0, 10, 0, -2, -4, 2, -4, -2}) {
    for (int i = 0; i < std::abs(n); ++i) {
      if (n > 0)
        ++i1;
      else
        --i1;
    }
    i2 += n;
    ASSERT_TRUE(i1 == i2);
    ASSERT_EQ(*i1, *i2);
  }
  EXPECT_ANY_THROW(--i1);
  EXPECT_ANY_THROW(i2 -= 2);
}

#endif