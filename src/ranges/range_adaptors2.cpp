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

//#region [Headers]
#include <concepts>
#include <iomanip>
#include <iostream>
#include <locale>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
//#endregion

namespace ranges = std::ranges;
namespace views = std::views;

template <std::ranges::input_range View>
auto make_string(View &&view) -> std::string {
  std::string result;
  result.reserve(ranges::distance(view));
  ranges::copy(view, std::back_inserter(result));
  return result;
}

int main() {

  std::string message =
      "Voici mon palindrome: \"Engage le jeu que je le gagne\"; est-il correct ?";
  //"Here is my palindrome: \"Sums are not set as a test on Erasmus\"; is it OK?";

  auto extractor = views::drop_while([](const char &c) { return c != '"'; })   //
                   | views::drop(1)                                            //
                   | views::take_while([](const char &c) { return c != '"'; }) //
      ;

  // auto loc = std::locale("fr_FR.UTF-8"); // if you have French locales
  auto loc = std::locale("C"); // default locale (always OK)
  auto cleanup =
      views::filter([loc](const char &c) { return std::isalpha(c, loc); })      //
      | views::transform([loc](const char &c) { return std::tolower(c, loc); }) //
      ;

  auto sentence = make_string(message | extractor);
  if (auto s = make_string(sentence | cleanup);
      s == make_string(views::reverse(s))) {
    std::cout << "'" << sentence << "' is a palindrome\n";
  } else {
    std::cout << "'" << sentence << "' is not a palindrome\n";
  }
}

#endif