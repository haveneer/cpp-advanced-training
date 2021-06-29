#include <version>
#ifndef __cpp_lib_ranges
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_ranges)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on
//#endregion

//#region [Headers]
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <type_traits>
//#endregion

int main() {
  namespace ranges = std::ranges;
  namespace views = std::views;
  
  std::map<std::string, int> v = {{"AB", 1}, {"CD", 2}};

  // The inner range type (ranges::range_reference_t<V>) cannot be a container type
  // (but can be reference to container). For example, it's not allowed to join a
  // transform_view of std::string prvalue.
  auto first_bad = [](const auto &p) /* std::string */ { return p.first; };
  // std::string cf type writer:
  // using MapValue = std::decay_t<decltype(v)>::value_type;
  // std::cout << type<std::invoke_result_t<decltype(first), MapValue>>() << "\n";
  // The good prototype:
  auto first_ok = [](const auto &p) -> const std::string & { return p.first; };

  for (auto &&x : v //
                    // | std::views::transform(first_bad) //
                      | std::views::transform(first_ok) //
                      // | std::views::keys // alternative is this case
                      | std::views::join //
  ) {
    std::cout << x << '\n';
  }
}

#endif