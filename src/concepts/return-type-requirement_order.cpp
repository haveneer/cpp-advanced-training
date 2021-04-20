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
#define DISPLAY(X) std::cout << std::setw(20) << #X << " : " << X << '\n';
//#endregion

template <typename T1, typename T2>
concept IsSmallerThan = (sizeof(T1) < sizeof(T2));

template <typename T>
concept ValidConcept = requires {
  {
    T {}
    } -> IsSmallerThan<long>;
};

int main() {
  DISPLAY(ValidConcept<char>);
  DISPLAY(ValidConcept<short>);
  DISPLAY(ValidConcept<int>);
  DISPLAY(ValidConcept<long>);
  DISPLAY(ValidConcept<float>);
  DISPLAY(ValidConcept<double>);
}

#endif