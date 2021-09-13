#include <version>
#if !defined(__cpp_constinita)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_constinit)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <iostream>

extern constinit int staticCompanion;
/* constinit */ auto staticMain = staticCompanion;

int main() { // What happens ?
  std::cout << "staticMain: " << staticMain << '\n';
}

#endif