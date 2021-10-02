#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif

#if !defined(__cpp_concepts) || !defined(__cpp_lib_coroutine)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_coroutine)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#endif
#include <iostream>
#include <list>
#include <set>
#include <vector>
//#endregion

#if 0
async_generator<int> f() {

  // code

  co_yield data;

  // code

  co_return data;
}

async_generator<int> f1() {

  // code

  auto data = co_await f2();

  // code

  co_return data;
}

async_generator<double> f2() {

  // code

  co_yield data;
}

#endif

int main() {}

#endif
