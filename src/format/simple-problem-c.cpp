//#region [Collapse all]
#include <cassert>
#include <cstdio>
#include <iostream>
#if __has_include(<numbers>)
#include <numbers> // C++20
#else
namespace std::numbers {
constexpr double pi = 3.1415926535897932;
}
#endif
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
//#endregion

int main() {
  std::vector<std::tuple<std::string, int, double, std::string>> data{
      {"John", 42, -std::numbers::pi,
       "Name:      John; answer: 0x002a=   42; number: -3.1416    ;"},
      {"Jean-Paul", 65535, 299792458,
       "Name: Jean-Paul; answer: 0xffff=65535; number: +2.9979e+08;"}};

  for (auto [name, answer, number, expected_result] : data) {

    const char *fmt =
        "Name: %9s; answer: %#06x=%5d; number: %+-11.5g;"; // shared format
    size_t sz = std::snprintf(nullptr, 0, fmt, name.c_str(), answer, answer, number);
    std::vector<char> buffer(sz + 1);               // Note: +1 for null terminator
    std::snprintf(std::data(buffer), buffer.size(), // never use sprintf (without n)
                  fmt, name.c_str(), answer, answer, number); // should return 'sz'
    std::string_view result{std::data(buffer), sz};

    assert(result == expected_result);
    std::cout << result << std::endl;
  }
}