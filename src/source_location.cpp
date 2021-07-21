#include <version>
#ifndef __cpp_lib_source_location
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <iostream>
#include <source_location>
#include <string_view>

void log(std::string_view message,
         const std::source_location &location = std::source_location::current()) {
  std::cout << "file: "                          //
            << location.file_name() << "("       // was __FILE__
            << location.line() << ":"            // was __LINE__
            << location.column() << ") `"        // column was not accessible
            << location.function_name() << "`: " // macro was not portable before
            << message << '\n';
}

template <typename T> void fun(T x) { log(x); }

int main() {
  // Implicitly provide call location
  log("From main");
  fun("From fun");
  [] { log("From lambda"); }();
}

#endif