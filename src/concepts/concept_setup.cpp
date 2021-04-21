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
struct MyStruct {
  int data;
};
//#endregion

#include <utility> // HINT #3 : maybe missing include

template <typename T> void swappable_as_function__setup(T x, T y) {
  // swap(x, y);   // HINT #1 : was an error: 'swap' was not declared in this scope
  std::swap(x, y); // HINT #3 : fix
}

// HINT #2 : any kind of explicit instantiation
template void swappable_as_function__setup(MyStruct, MyStruct);

int main() {
  MyStruct x, y;                      // HINT #2
  swappable_as_function__setup(x, y); // any kind of explicit instantiation
}

template <typename T> // HINT #4 : convert your setup as concept
concept swappable_as_function = requires(T x, T y) {
  std::swap(x, y);
};

// HINT #5 : add static assertions (validation AND invalidation examples)
static_assert(swappable_as_function<MyStruct>);
#include <vector>
static_assert(swappable_as_function<std::vector<int>>);
#include <mutex>
static_assert(!swappable_as_function<std::mutex>);

#endif