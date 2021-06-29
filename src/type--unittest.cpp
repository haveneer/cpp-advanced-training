//#region [Collapse all]
#include <gtest/gtest.h>
#include <iostream>

// oneline for embedding in example
// clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
// clang-format on

//#endregion

using namespace std::string_literals;
#ifdef _MSC_VER
#define TYPE "int"s
#else
#define TYPE "i"s
#endif

TEST(CheckType, simple) {
  EXPECT_EQ(type<int>(), TYPE);
  EXPECT_EQ(type<const int>(), TYPE + " const");
}
TEST(CheckType, pointer) {
  EXPECT_EQ(type<int *>(), TYPE + "*");
  EXPECT_EQ(type<int *const>(), TYPE + "* const");
  EXPECT_EQ(type<int const *>(), TYPE + " const*");
  EXPECT_EQ(type<int const *const>(), TYPE + " const* const");
}
TEST(CheckType, reference) {
  EXPECT_EQ(type<int &>(), TYPE + "&");
  EXPECT_EQ(type<int &&>(), TYPE + "&&");
  EXPECT_EQ(type<int const>(), TYPE + " const");
  EXPECT_EQ(type<int const &>(), TYPE + " const&");
  EXPECT_EQ(type<int const &&>(), TYPE + " const&&");
}
TEST(CheckType, extent) {
  EXPECT_EQ(type<int[]>(), TYPE + "[]");
  EXPECT_EQ(type<int const[]>(), TYPE + " const[]");
}
