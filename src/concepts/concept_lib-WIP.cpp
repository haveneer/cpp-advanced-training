#include <version>
#if !(defined(__cpp_concepts) && defined(__cpp_lib_concepts))
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Includes]
#include <concepts>
#include <string>
//#endregion

template <typename T>
concept destructible = std::is_nothrow_destructible_v<T>;
static_assert(destructible<std::string> && !destructible<void>);

template <typename T, typename... Args>
concept constructible_from = destructible<T> && std::is_constructible_v<T, Args...>;

static_assert(constructible_from<std::string, const char *> &&
              !constructible_from<std::string, void *>);

int main() {}

#endif