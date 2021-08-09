module; // global module fragment

#include <cassert> // not importable include

export module helloworld2;
#include <iostream> // importable include
export void hello(const char *name = "world");

module : private; // private module fragment (below not available to importers)
                  // equivalent to be isolated in a .mxx file
void hello(const char *name) {
  assert(name != nullptr);
  std::cout << "Hello " << name << "! (from a C++20 module)\n";
}