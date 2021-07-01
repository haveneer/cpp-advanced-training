//#region [Collapse all]
#include <cassert>
#include <cstdio>
#include <iostream>
#include <numbers> // C++20
#include <string>
#include <tuple>
#include <vector>
//#endregion

//#region [Compiler dependent <format> header]
#include <fmt/core.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std {
using namespace fmt;
}
#endif
//#endregion

int main() {
  std::vector<std::tuple<std::string, int, double, std::string>> data{
      {"John", 42, -std::numbers::pi,
       "Name:      John; answer: 0x002a=   42; number: -3.1416    ;"},
      {"Jean-Paul", 65535, 299792458,
       "Name: Jean-Paul; answer: 0xffff=65535; number: +2.9979e+08;"}};

  for (auto [name, answer, number, expected_result] : data) {

    std::string result =
        std::format("Name: {0:>9}; answer: {1:#06x}={1:5d}; number: {2:<+11.5g};",
                    name, answer, number);

    assert(result == expected_result);
    std::cout << result << std::endl;
  }
}