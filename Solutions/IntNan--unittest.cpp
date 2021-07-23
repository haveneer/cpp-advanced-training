#include <version>
#if !defined(__cpp_impl_three_way_comparison) || \
    !defined(__cpp_lib_three_way_comparison)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_impl_three_way_comparison),
                 STR(__cpp_lib_three_way_comparison)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

/* Subject: an IntNan class where number are ints or nan (IntNan{} is that nan value)
 * All these numbers must "comparable" except nan values (as nan for FP numbers)
 */

#include <compare>
#include <gtest/gtest.h>
#include <optional>

struct IntNan {
  std::optional<int> val = std::nullopt;

  bool operator==(const IntNan &that) const {
    if (!val || !that.val) {
      return false;
    }
    return *val == *that.val;
  }

  bool operator==(const int &that) const {
    if (!val) {
      return false;
    }
    return *val == that;
  }

  std::partial_ordering operator<=>(const IntNan &that) const {
    if (!val || !that.val) {
      // we can express the unordered state as a first class value
      return std::partial_ordering::unordered;
    }

    // <=> returns std::strong_ordering and implicitly converted to partial_ordering
    return *val <=> *that.val;
  }

  std::partial_ordering operator<=>(const int &that) const {
    if (!val) {
      return std::partial_ordering::unordered;
    }
    return *val <=> that;
  }
};

TEST(IntNan, OrderingWithIntNan) {
  EXPECT_EQ(IntNan{2} <=> IntNan{4}, std::partial_ordering::less);
  EXPECT_EQ(IntNan{2} <=> IntNan{}, std::partial_ordering::unordered);
  EXPECT_EQ(IntNan{} <=> IntNan{}, std::partial_ordering::unordered);
}

TEST(IntNan, OrderingWithInt) {
  EXPECT_EQ(IntNan{2} <=> 4, std::partial_ordering::less);
  EXPECT_EQ(IntNan{2} <=> 2, std::partial_ordering::equivalent);
  EXPECT_EQ(4 <=> IntNan{2}, std::partial_ordering::greater);
  EXPECT_EQ(2 <=> IntNan{2}, std::partial_ordering::equivalent);
}

TEST(IntNan, ComparaisonWithIntNan) {
  EXPECT_TRUE(IntNan{2} < IntNan{4});
  EXPECT_FALSE(IntNan{2} < IntNan{});
  EXPECT_FALSE(IntNan{2} == IntNan{});
  EXPECT_FALSE(IntNan{2} <= IntNan{});
}

TEST(IntNan, ComparaisonWithInt) {
  EXPECT_TRUE(IntNan{2} < 4);
  EXPECT_TRUE(IntNan{2} == 2);
  EXPECT_TRUE(4 >= IntNan{2});
  EXPECT_TRUE(4 != IntNan{2});
}

#endif