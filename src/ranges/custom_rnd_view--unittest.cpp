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
#include <random>
#include <ranges>
#include <sstream>

// There is a bug in GCC 10.3 (should be confirmed)
// Here is a workaround which disables some tests
#ifdef __GNUC__
#ifndef __clang__
#if __GNUC__ == 10 && __GNUC_MINOR__ <= 3
#define DISABLE_SOME_TEST
#endif
#endif
#endif
#ifdef DISABLE_SOME_TEST
#define CONDITIONAL(X) DISABLED_##X
#else
#define CONDITIONAL(X) X
#endif

TEST(CppRandom, SequenceIsConform) {
  // cf Note 6 in
  // https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
  const unsigned int seed = std::mt19937::default_seed;
  std::mt19937 engine{seed};
  auto reference = 4123659995;
  engine.discard(10000 - 1);
  auto generated = engine();
  EXPECT_EQ(generated, reference);
}

TEST(CppRandom, ArePortable) {
  const unsigned int seed = std::mt19937::default_seed;
  std::mt19937 engine{seed};
  std::vector results1{3499211612u, 581869302u, 3890346734u, 3586334585u};
  std::vector results2{545404204u, 4161255391u, 3922919429u, 949333985u};
  for (size_t i = 0; i < results1.size(); ++i) { // dropped in following tests
    auto generated = engine();
    EXPECT_EQ(generated, results1[i]);
  }
  for (size_t i = 0; i < results2.size(); ++i) { // used in following tests
    auto generated = engine();
    EXPECT_EQ(generated, results2[i]);
  }
}

namespace views = std::ranges::views;
namespace ranges = std::ranges;

// Parametrized fixture
class ParametrizedTest : public ::testing::TestWithParam<int> {};

using CustomRndView = ParametrizedTest;
TEST_P(CustomRndView, CONDITIONAL(DirectReverseComparaison)) {
  const unsigned int seed = std::mt19937::default_seed;
  auto n = GetParam();
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

// Parametrized fixture
class VectorsParametrizedTest
    : public ::testing::TestWithParam<std::tuple<int, std::vector<std::uint32_t>>> {
};

using CustomRndViewSequence = VectorsParametrizedTest;
TEST_P(CustomRndViewSequence, CONDITIONAL(Direct)) {
  const unsigned int seed = std::mt19937::default_seed;
  auto [n, output] = GetParam();
  std::size_t index = 0;
  for (auto const &i : custom_views::rnd(seed) | views::drop(4) | views::take(n)) {
    EXPECT_EQ(i, output[index]);
    index++;
  }
}

TEST_P(CustomRndViewSequence, CONDITIONAL(Reverse)) {
  const unsigned int seed = std::mt19937::default_seed;
  auto [n, output] = GetParam();
  std::size_t index = n;
  for (auto const &i :
       custom_views::rnd(seed) | views::drop(4) | views::take(n) | views::reverse) {
    EXPECT_EQ(i, output[index - 1]);
    index--;
  }
}

INSTANTIATE_TEST_SUITE_P(
    Jobs, CustomRndViewSequence,
    testing::Values(std::make_tuple(0, std::vector<std::uint32_t>{}),
                    std::make_tuple(1, std::vector{545404204u, 4161255391u,
                                                   3922919429u, 949333985u}),
                    std::make_tuple(2, std::vector{545404204u, 4161255391u,
                                                   3922919429u, 949333985u}),
                    std::make_tuple(3, std::vector{545404204u, 4161255391u,
                                                   3922919429u, 949333985u}),
                    std::make_tuple(4, std::vector{545404204u, 4161255391u,
                                                   3922919429u, 949333985u})));

TEST(CustomRndView, SymmetricMove) {
  const unsigned int seed = std::mt19937::default_seed;
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
  const unsigned int seed = std::mt19937::default_seed;
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