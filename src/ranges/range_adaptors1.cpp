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
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on
//#endregion

//#region [Headers]
#include <iomanip>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
//#endregion

auto inspector = [locations = std::set<int>{}](const auto &x,
                                               const int line) mutable {
  const bool print_details = false; // HINT change me to see data flow
  if (print_details || locations.insert(line).second) {
    std::cout << "at line" << std::setw(4) << line << " : "
              << type<std::decay_t<decltype(x)>>();
    if constexpr (requires(std::ostream & o, const decltype(x) &t) { o << t; }) {
      std::cout << " : " << x << '\n';
    } else {
      std::cout << " [Not printable value]\n";
    }
  }
};

int main() {
  namespace ranges = std::ranges;
  namespace views = std::views;

  std::string message = "The quick brown fox jumps over the lazy dog";

  auto r = message                                                        //
           | views::split(' ')                                            //
           | views::transform([](auto &&s) {                              //
               inspector(s, __LINE__);                                    //
               /* s is a ranges::split_view not a string_view */          //
               return std::string_view(&*s.begin(), ranges::distance(s)); //
             })                                                           //
           | views::filter([](const std::string_view &s) {                //
               inspector(s, __LINE__);                                    //
               return s.size() > 3;                                       //
             })                                                           //
           | views::transform([](auto &&s) {                              //
               inspector(s, __LINE__);                                    //
               return s.size();                                           //
             })                                                           //
           | views::take(4)                                               //
      ;

  ranges::copy(r, std::ostream_iterator<int>{std::cout, "\n"});
}

#endif