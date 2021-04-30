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
#include <deque>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>
#include <sstream>

namespace views = std::ranges::views;
namespace ranges = std::ranges;

// Parametrized fixture
class ParametrizedTest : public ::testing::TestWithParam<int> {};

using CustomRndView = ParametrizedTest;
TEST_P(CustomRndView, DirectReverseComparaison) {
  auto n = GetParam();
  const unsigned int seed = n;

  std::deque<int> direct_result;
  for (auto const &i : custom_views::rnd(seed) | views::drop(4) | views::take(n)) {
    direct_result.push_back(i);
  }

  std::deque<int> reverse_result;
  for (auto const &i :
       custom_views::rnd(seed) | views::drop(4) | views::take(n) | views::reverse) {
    reverse_result.push_front(i);
  }

  ASSERT_EQ(direct_result.size(), reverse_result.size());
  auto i1 = direct_result.begin();
  auto i2 = reverse_result.begin();
  while (i1 != direct_result.end()) {
    EXPECT_EQ(*i1++, *i2++);
  }
  EXPECT_EQ(i1 == direct_result.end(), i2 == reverse_result.end());
}

INSTANTIATE_TEST_SUITE_P(Jobs, CustomRndView, testing::Range(0, 10, 1));

TEST(CustomRndView, SymmetricMove) {
  const unsigned int seed = 1;
  auto v = custom_views::rnd(seed);
  auto i1 = v.begin();
  auto i2 = i1;
  ASSERT_TRUE(i1 == i2);
  for (auto &&iteration : {1, 2, 3, 4, 5}) {
    std::ostringstream oss;
    oss << "Current iteration: " << iteration;
    SCOPED_TRACE(oss.str());
    ASSERT_TRUE(i1 == i2);
    ASSERT_TRUE(*i1 == *i2);
  }
}

TEST(CustomRndView, FastMove) {
  const unsigned int seed = 1;
  auto v = custom_views::rnd(seed);
  auto i1 = v.begin();
  auto i2 = i1;
  for (int iteration = 0; auto &n : {0, 1, 1, 2, 6, 0, -1, -5, 2, -4, -2}) {
    std::ostringstream oss;
    oss << "Current iteration: " << iteration++ << " with n=" << n;
    SCOPED_TRACE(oss.str());
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