module; // global module fragment
#include <iostream>

export module helloworld2;
export void hello(const char *name = "world");

module : private; // private module fragment (below not available to importers)
void hello(const char *name) {
  std::cout << "Hello " << name << "! (from a C++20 module)\n";
}