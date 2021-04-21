#include <version>
#ifndef __cpp_concepts
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <iomanip>
#include <iostream>
#define DISPLAY(X) std::cout << std::setw(24) << #X << " : " << X << '\n';
//#endregion

template <typename T1, typename T2>
concept SmallerThan = (sizeof(T1) < sizeof(T2));
static_assert(SmallerThan<char, long>); // direct usage requires 2 template arguments

template <typename T>
concept ConstructibleAndSmallerThanLong = requires {
  {
    T {}
    } -> SmallerThan<long>; // concept usage requires only one (the second one)
};

int main() {
  DISPLAY(ConstructibleAndSmallerThanLong<char>);
  DISPLAY(ConstructibleAndSmallerThanLong<short>);
  DISPLAY(ConstructibleAndSmallerThanLong<int>);
  DISPLAY(ConstructibleAndSmallerThanLong<long>);
  DISPLAY(ConstructibleAndSmallerThanLong<float>);
  DISPLAY(ConstructibleAndSmallerThanLong<double>);

  SmallerThan<long> auto i = 'x'; // HINT use only second template argument
  // SmallerThan<double> auto f = 0.0; // error: deduced initializer does
  //                                   //        not satisfy placeholder constraints
}

#endif