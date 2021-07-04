//#region [Collapse all]
#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
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

struct YourType {
  std::string first_name;
  std::string last_name;
  std::size_t age;
};

template <> class std::formatter<YourType> {
public:
  template<typename FormatContext>
  constexpr auto parse(FormatContext &context) {
    const auto iter{context.begin()};
    if (iter != context.end() && *iter != '}')
      throw std::format_error{"Invalid specifier"};
    return iter;
  }

  template<typename FormatContext>
  auto format(const YourType &x, FormatContext &context) {
    auto to_upper = [&](std::string s) {
      std::transform(s.begin(), s.end(), s.begin(),
                     [](char ch) { return std::toupper(ch); });
      return s;
    };

    return std::format_to(context.out(), "{} {} ({})", x.first_name,
                          to_upper(x.last_name), x.age);
  }
};

int main() {
  YourType x{"John", "Doe", 42};

  std::cout << std::format("Who is {} ?", x) << std::endl;
}