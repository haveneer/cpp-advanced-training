//#region [Collapse all]
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
//#endregion

//#region [Compiler dependent <format> header]
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std {
using namespace fmt;
}
#endif
//#endregion

template <typename T> class std::formatter<std::vector<T>> {
public:
  template <typename FormatContext>
  constexpr auto parse(FormatContext &context) { // BETTER IF constexpr
    auto iter = std::begin(context);
    const auto end = std::end(context);
    if (iter != end && *iter != ':') {
      if (end - iter < 3)
        throw std::format_error("Delimiters specifiers should be 3 chars");
      left_delimiter = *iter++;
      entry_delimiter = *iter++;
      right_delimiter = *iter++;
    }
    if (iter != end && *iter != '}') {
      if (*iter == ':')
        ++iter;
      else
        throw std::format_error("Subtype format specifier should start with :");
    }
    context.advance_to(iter);
    return m_sub_formatter.parse(context);
  }

  template <typename FormatContext>
  auto format(const std::vector<T> &v, FormatContext &context) {
    auto out = context.out();
    *out++ = left_delimiter;
    if (v.size() > 0)
      m_sub_formatter.format(v[0], context);
    for (size_t i = 1; i < v.size(); ++i) {
      *out++ = entry_delimiter;
      m_sub_formatter.format(v[i], context);
    }
    *out++ = right_delimiter;
    return out;
  }

  char left_delimiter = '[';
  char entry_delimiter = ',';
  char right_delimiter = ']';
  std::formatter<T> m_sub_formatter;
};

int main() {
  std::vector<float> v = {1, 2, 3, 4};
  std::cout << std::format("Pretty vector: {}", v) << std::endl;
  // std::cout << std::format("Pretty vector: {:}", v) << std::endl; // error
  std::cout << std::format("Pretty vector: {::}", v) << std::endl;
  std::cout << std::format("Pretty vector: {:(;):}", v) << std::endl;
  std::cout << std::format("Pretty vector: {:(;)}", v) << std::endl;
  // std::cout << std::format("Pretty vector: {:|6.2f}", v) << std::endl; // error
  std::cout << std::format("Pretty vector: {::6.2f}", v) << std::endl;
  std::cout << std::format("Pretty vector: {:{|}:^6.2f}", v) << std::endl;

  std::vector<std::vector<int>> w = {{1, 2}, {3}, {4, 5}};
  std::cout << std::format("Pretty cascade vector: {}", w) << std::endl;
  std::cout << std::format("Pretty cascade vector: {:(;):[,]:^5}", w) << std::endl;
}