//#region [Collapse all]
#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
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

int main() {
  constexpr auto my_format = "This string contains exactly {0} characters";

  auto fixed_point = [](size_t n, auto &&f) {
    size_t next_n = f(n);
    while (n != next_n)
      n = std::exchange(next_n, f(next_n));
    return n;
  };
  size_t n =
      fixed_point(0, [&](size_t n) { return std::formatted_size(my_format, n); });

  std::vector<char> buffer;
  buffer.reserve(n + 1);
  const auto *original_ptr = buffer.data(); // save original point allocation
  buffer[n] = '?';                          // for buffer overflow check

  std::vformat_to(std::back_inserter(buffer), my_format, std::make_format_args(n));

  std::cout << std::string_view{std::data(buffer), n} << std::endl;
  assert(original_ptr == buffer.data()); // no reallocation
  assert(buffer[n] == '?');              // does not hit guard chat
}