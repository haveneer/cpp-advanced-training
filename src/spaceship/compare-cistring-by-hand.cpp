//#region [Includes and plateform specific]
#include <cassert>
#include <cstring>
#include <string>
using namespace std::string_literals;

int case_insensitive_compare(const char *str1, const char *str2) {
#ifdef _MSC_VER
  return _stricmp(str1, str2);
#else
  return strcasecmp(str1, str2);
#endif
}
//#endregion

/// CaseInsensitiveString inspired by http://wg21.link/p0515
/// Warning: p0515 is not conform with the C++20 standard
class CIString {
public:
  /// CIString cmp CIString
  friend bool operator==(const CIString &a, const CIString &b) {
    return case_insensitive_compare(a.m_str.c_str(), b.m_str.c_str()) == 0;
  }
  friend bool operator<(const CIString &a, const CIString &b) {
    return case_insensitive_compare(a.m_str.c_str(), b.m_str.c_str()) < 0;
  }
  friend bool operator!=(const CIString &a, const CIString &b) { return !(a == b); }
  friend bool operator<=(const CIString &a, const CIString &b) { return !(b < a); }
  friend bool operator>(const CIString &a, const CIString &b) { return b < a; }
  friend bool operator>=(const CIString &a, const CIString &b) { return !(a < b); }

  /// CIString cmp const char *
  friend bool operator==(const CIString &a, const char *b) {
    return case_insensitive_compare(a.m_str.c_str(), b) == 0;
  }
  friend bool operator<(const CIString &a, const char *b) {
    return case_insensitive_compare(a.m_str.c_str(), b) < 0;
  }
  friend bool operator!=(const CIString &a, const char *b) { return !(a == b); }
  friend bool operator<=(const CIString &a, const char *b) { return !(b < a); }
  friend bool operator>(const CIString &a, const char *b) { return b < a; }
  friend bool operator>=(const CIString &a, const char *b) { return !(a < b); }

  /// const char * cmp CIString
  friend bool operator==(const char *a, const CIString &b) {
    return case_insensitive_compare(a, b.m_str.c_str()) == 0;
  }
  friend bool operator<(const char *a, const CIString &b) {
    return case_insensitive_compare(a, b.m_str.c_str()) < 0;
  }
  friend bool operator!=(const char *a, const CIString &b) { return !(a == b); }
  friend bool operator<=(const char *a, const CIString &b) { return !(b < a); }
  friend bool operator>(const char *a, const CIString &b) { return b < a; }
  friend bool operator>=(const char *a, const CIString &b) { return !(a < b); }

public:
  CIString(std::string str) : m_str(std::move(str)) {}
  CIString(const char *str) : m_str(str) {}

private:
  std::string m_str;
};

int main() {
  assert(CIString("abc") < CIString("abcd"s));
  assert(CIString("abc"s) >= CIString("abc"));
  assert(CIString("HEllO"s) == CIString("heLLo"s));
  assert(CIString("l0"s) != CIString("1O"s));
}