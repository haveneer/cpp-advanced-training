//#region [Collapse all]
#include <iostream>
#include <sstream>
#include <string>

std::string version_string(int a, int b, int c) {
  std::ostringstream ss;
  ss << a << '.' << b << '.' << c;
  return ss.str();
}
//#endregion

int main() {
// more info, see https://sourceforge.net/p/predef/wiki/Compilers/
#ifdef __GNUG__
#ifdef __clang__
#ifdef __apple_build_version__
  std::string cxx_compiler = "AppleClang";
#else
  std::string cxx_compiler = "Clang";
#endif
  std::string cxx_compiler_version =
      version_string(__clang_major__, __clang_minor__, __clang_patchlevel__);
#else
  std::string cxx_compiler = "GCC";
  std::string cxx_compiler_version =
      version_string(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif
#elif defined(_MSC_VER)
  std::string cxx_compiler = "MSVC";
  long n = _MSC_FULL_VER;
  int major = n / 10000000L;
  n %= 10000000L;
  int minor = n / 100000L;
  n %= 100000L;
  int patch = n;
  std::string cxx_compiler_version = version_string(major, minor, patch);
#else
  std::string cxx_compiler = "unknown compiler";
  std::string cxx_compiler_version = "";
#endif

  std::string cxx_standard = "undefined";
#ifdef _MSC_VER
  const long std_release = _MSVC_LANG;
#else
  const long std_release = __cplusplus;
#endif

  // https://stackoverflow.com/a/56483887/12430075
  switch (std_release) {
  case 199711L:
    cxx_standard = "C++98";
    break;
  case 201103L:
    cxx_standard = "C++11";
    break;
  case 201402L:
    cxx_standard = "C++14";
    break;
  case 201703L:
    cxx_standard = "C++17";
    break;
  case 202002L:
    cxx_standard = "C++20";
    break;
  default:
    if (__cplusplus < 199711L)
      cxx_standard = "C++ pre-";
    else if (__cplusplus > 202002L)
      cxx_standard = "C++2b";
    else if (__cplusplus > 201703L)
      cxx_standard = "C++2a";
    else
      cxx_standard = "undefined";
  }

  std::cout << cxx_compiler << " " << cxx_compiler_version << " with "
            << cxx_standard << " standard"
            << " [" + std::to_string(std_release) + "]"
            << "\n";
}