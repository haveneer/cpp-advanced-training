//#region [Collapse all]
#include <iostream>
#include <tuple>
//#endregion

// C++17 style (9 LOC)
template <typename... Ts> auto f17(Ts... ts) {
  auto pack = std::make_tuple(std::forward<std::decay_t<Ts>>(ts)...);
  return [pack = std::move(pack)](std::string_view header) {
    std::apply( // tuple -> parameter pack
        [header](Ts... ts) {
          std::cout << header << ':', ((std::cout << " " << ts), ...) << '\n';
        },
        pack);
  };
}

// C++20 style (4 LOC)
template <typename... Ts> auto f20(Ts... ts) {
  return [... ts = std::move(ts)](std::string_view header) {
    std::cout << header << ':', ((std::cout << " " << ts), ...) << '\n';
  };
  // or simply [ts...](std::string_view header) { ... } if copy do not matter
}

int main() {
  auto v17 = f17(1, 'a', 3.14);
  v17("Hello data");

  auto v20 = f20(1, 'a', 3.14);
  v20("Hello data");
}
