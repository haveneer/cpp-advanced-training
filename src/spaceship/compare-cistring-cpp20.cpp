#include <version>
#if !defined(__cpp_impl_three_way_comparison)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_impl_three_way_comparison)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Includes and plateform specific]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <string>
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

int case_insensitive_compare(const char *str1, const char *str2) {
#ifdef _MSC_VER
  return _stricmp(str1, str2);
#else
  return strcasecmp(str1, str2);
#endif
}
//#endregion

class CIString {
public:
  std::weak_ordering operator<=>(const CIString &b) const {
    return case_insensitive_compare(m_str.c_str(), b.m_str.c_str()) <=> 0;
  }
  bool operator==(const CIString &b) const { return *this <=> b == 0; }

  std::weak_ordering operator<=>(const char *b) const {
    return case_insensitive_compare(m_str.c_str(), b) <=> 0;
  }
  bool operator==(const char *b) const { return *this <=> b == 0; }

public:
  CIString(std::string str) : m_str(std::move(str)) {}
  CIString(const char *str) : m_str(str) {}

private:
  std::string m_str;
};

int main() {
  //#region [Basic tests]
  assert(CIString("abc") < CIString("abcd"));
  assert(CIString("abc") >= "abc");
  assert("abc" <= CIString("abcd")); // HINT note argument inversion
  assert("abc" != CIString("abcd")); //      first object is not CIString

  auto cmp1 = (CIString("HEllO") <=> CIString("heLLo"));
  std::cout << type<decltype(cmp1)>() << " : "
            << (cmp1 == std::weak_ordering::equivalent) << '\n';
  auto cmp2 = (CIString("HEllO") == CIString("heLLo"));
  std::cout << type<decltype(cmp2)>() << " : " << (cmp2 == true) << '\n';
  //#endregion
  //#region [Rewrite operators]
  // clang-format off
  CIString a("a");
  CIString b("b");
  const char *ca = "a";
  if (a == b) { }
  if (a != b) { }
  if (a > b) { }
  if (a < b) { }
  if (a >= b) { }
  if (a <= b) { }
  if (a == ca) { }
  if (a != ca) { }
  if (ca == a) { }
  if (ca != a) { }
  // clang-format on
  //#endregion
}

#endif