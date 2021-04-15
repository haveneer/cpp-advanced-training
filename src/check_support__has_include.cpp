//#region [Collapse all]
#include <iostream>
//#endregion// Standard system include

// Don't reformat !
// clang-format off

// Check if __has_include feature is available
#ifdef __has_include
#  if __has_include(<optional>)
#    include <optional>
#    define HAS_OPTIONAL 1 // define a macro with a value
#  else
#     // or a #error "std::optional is missing"
#     pragma message ( "std::optional is not available" ) // portable #warning
#     define HAS_OPTIONAL 0
#  endif
#endif

// clang-format on
//#endregion

// Récupération de la version de C++ si disponible

int main() {
#ifdef __cplusplus
  std::cout << "The current compiler supports C++ " << __cplusplus << '\n'; 
#else
  std::puts("The current compiler does not expose the supported C++ version"); 
#endif
  
#ifdef HAS_OPTIONAL
  std::optional<int> value; // = 1;
  if (value.has_value()) {
    std::cout << "'value' contains " << value.value() << '\n';
  } else {
    std::puts("'value' does not contains any value");
  }
#else
  std::puts("std::optional feature is not available");
#endif
}