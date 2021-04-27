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

#include <ranges>
#include <sstream>

int main() {
  namespace ranges = std::ranges;
  namespace views = std::views;

  // #1 : Ok
  std::istringstream iss("The quick brown fox jumps over the lazy dog");
  for (auto &&x : ranges::istream_view<std::string>(iss))
    ;

  // #2 : without #3 it compiles
  auto r = ranges::istream_view<std::string>(iss)                 //
           | views::filter([](auto &&x) { return x.size() > 3; }) //
      ;

  // #3 : with #2 it does not compile :/
  //      Lazy evaluation and lazy compilation
  for (auto &&x : r)
    ;
  //#region [Error message]
  // clang-format off
  /* In file included from code/src/ranges/ranges_traps1.cpp:13:
   * /usr/include/c++/11/ranges: In instantiation of 'static constexpr auto std::ranges::filter_view<_Vp, _Pred>::_Iterator::_S_iter_cat() [with _Vp = std::ranges::basic_istream_view<std::__cxx11::basic_string<char>, char, std::char_traits<char> >; _Pred = main()::<lambda(auto:15&&)>]':
   * /usr/include/c++/11/ranges:1149:48:   required from 'struct std::ranges::filter_view<std::ranges::basic_istream_view<std::__cxx11::basic_string<char>, char, std::char_traits<char> >, main()::<lambda(auto:15&&)> >::_Iterator'
   * code/src/ranges/ranges_traps1.cpp:31:19:   required from here
   * /usr/include/c++/11/ranges:1131:17: error: no type named 'iterator_category' in 'struct std::iterator_traits<std::ranges::basic_istream_view<std::__cxx11::basic_string<char>, char, std::char_traits<char> >::_Iterator>'
   *  1131 |           using _Cat = typename iterator_traits<_Vp_iter>::iterator_category;
   *       |                 ^~~~
   * /usr/include/c++/11/ranges:1132:25: error: no type named 'iterator_category' in 'struct std::iterator_traits<std::ranges::basic_istream_view<std::__cxx11::basic_string<char>, char, std::char_traits<char> >::_Iterator>'
   *  1132 |           if constexpr (derived_from<_Cat, bidirectional_iterator_tag>)
   *       |                         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * /usr/include/c++/11/ranges:1134:30: error: no type named 'iterator_category' in 'struct std::iterator_traits<std::ranges::basic_istream_view<std::__cxx11::basic_string<char>, char, std::char_traits<char> >::_Iterator>'
   *  1134 |           else if constexpr (derived_from<_Cat, forward_iterator_tag>)
   *       |                              ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * /usr/include/c++/11/ranges:1135:41: error: inconsistent deduction for auto return type: 'std::bidirectional_iterator_tag' and then 'std::forward_iterator_tag'
   *  1135 |             return forward_iterator_tag{};
   *       | 
   */
  //#endregion
}

#endif